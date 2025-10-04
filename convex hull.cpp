// Convex Hull (Graham's Scan) — Long-form, OOP style, no STL algorithms for sorting
// Policy-driven, stable mergesort, custom stack with Rule of Five, canonical orientation.

// Collinearity policy: KeepExtremeOnly (default) keeps only extremes on edges (no interior collinear points).

// This code intentionally avoids std::sort and algorithm headers. It uses iostream only for IO.
// Data containers are raw dynamic arrays (new/delete).

// Author: Ayush (2025-10-04)

#include <iostream>
#include <cassert>
#include <cstdint>
#include <cstring> // memset
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::size_t;

// ============================= Utilities & Config =============================

#ifdef HULL_DEBUG
#define DBG(stmt) \
    do            \
    {             \
        stmt;     \
    } while (0)
#else
#define DBG(stmt) \
    do            \
    {             \
    } while (0)
#endif

// Optional: switch IO type
#ifdef USE_DOUBLE
using coord_t = double;
#else
using coord_t = long long;
#endif

// ============================= Geometry Primitives ============================

struct Point
{
    coord_t x{}, y{};
};

static inline bool equal_coord(coord_t a, coord_t b) noexcept
{
#ifdef USE_DOUBLE
    // A small epsilon for floating inputs
    const double eps = 1e-12;
    return (a > b ? a - b : b - a) <= eps;
#else
    return a == b;
#endif
}

static inline bool equal_point(const Point &a, const Point &b) noexcept
{
    return equal_coord(a.x, b.x) && equal_coord(a.y, b.y);
}

// Distance squared (for tie-breaks). Use widened integer where possible.
struct Distance
{
    static inline long double sq(const Point &a, const Point &b) noexcept
    {
        long double dx = static_cast<long double>(a.x) - static_cast<long double>(b.x);
        long double dy = static_cast<long double>(a.y) - static_cast<long double>(b.y);
        return dx * dx + dy * dy;
    }
};

// cross((b - a), (c - a)) widened. Return sign via long double for portability.
struct Orientation
{
    static inline long double orient(const Point &a, const Point &b, const Point &c) noexcept
    {
        long double bax = static_cast<long double>(b.x) - static_cast<long double>(a.x);
        long double bay = static_cast<long double>(b.y) - static_cast<long double>(a.y);
        long double cax = static_cast<long double>(c.x) - static_cast<long double>(a.x);
        long double cay = static_cast<long double>(c.y) - static_cast<long double>(a.y);
        return bax * cay - bay * cax; // >0: left turn, <0: right turn, 0: collinear
    }
};

// ============================= Collinearity Policy ============================

enum class CollinearPolicy
{
    KeepExtremeOnly,
    KeepAllOnEdges
};

// ============================= Custom Stack (Linked) ==========================

template <typename T>
class Stack
{
    struct Node
    {
        T val;
        Node *next;
        Node(const T &v, Node *n = nullptr) : val(v), next(n) {}
    };
    Node *head_;
    size_t sz_;

    void copy_from(const Stack &other)
    {
        // Copy to a temporary reversed list, then reverse to preserve order
        if (!other.head_)
        {
            head_ = nullptr;
            sz_ = 0;
            return;
        }
        // First, reverse-copy
        Node *tmp = nullptr;
        for (Node *p = other.head_; p; p = p->next)
        {
            tmp = new Node(p->val, tmp);
        }
        // Now reverse into head_
        head_ = nullptr;
        sz_ = 0;
        while (tmp)
        {
            push(tmp->val);
            Node *q = tmp;
            tmp = tmp->next;
            delete q;
        }
    }

public:
    Stack() : head_(nullptr), sz_(0) {}
    ~Stack() { clear(); }

    Stack(const Stack &o) : head_(nullptr), sz_(0) { copy_from(o); }
    Stack(Stack &&o) noexcept : head_(o.head_), sz_(o.sz_)
    {
        o.head_ = nullptr;
        o.sz_ = 0;
    }

    Stack &operator=(const Stack &o)
    {
        if (this != &o)
        {
            clear();
            copy_from(o);
        }
        return *this;
    }
    Stack &operator=(Stack &&o) noexcept
    {
        if (this != &o)
        {
            clear();
            head_ = o.head_;
            sz_ = o.sz_;
            o.head_ = nullptr;
            o.sz_ = 0;
        }
        return *this;
    }

    void clear() noexcept
    {
        Node *p = head_;
        while (p)
        {
            Node *n = p->next;
            delete p;
            p = n;
        }
        head_ = nullptr;
        sz_ = 0;
    }

    void push(const T &v)
    {
        head_ = new Node(v, head_);
        ++sz_;
    }

    void pop()
    {
        assert(head_ && "pop on empty stack");
        Node *n = head_->next;
        delete head_;
        head_ = n;
        --sz_;
    }

    T &top()
    {
        assert(head_ && "top on empty stack");
        return head_->val;
    }
    const T &top() const
    {
        assert(head_ && "top on empty stack");
        return head_->val;
    }

    T &second_from_top()
    {
        assert(sz_ >= 2 && "second_from_top requires size>=2");
        return head_->next->val;
    }
    const T &second_from_top() const
    {
        assert(sz_ >= 2 && "second_from_top requires size>=2");
        return head_->next->val;
    }

    bool empty() const noexcept { return sz_ == 0; }
    size_t size() const noexcept { return sz_; }
};

// ============================= Stable Merge Sort ==============================
// Works on raw arrays with comparator functor cmp(a,b)->bool
template <typename T, typename Cmp>
static void merge_sort_impl(T *a, T *aux, int l, int r, const Cmp &cmp)
{
    if (r - l <= 1)
        return;
    int m = (l + r) >> 1;
    merge_sort_impl(a, aux, l, m, cmp);
    merge_sort_impl(a, aux, m, r, cmp);
    int i = l, j = m, k = l;
    while (i < m && j < r)
    {
        if (cmp(a[i], a[j]))
            aux[k++] = a[i++];
        else
            aux[k++] = a[j++];
    }
    while (i < m)
        aux[k++] = a[i++];
    while (j < r)
        aux[k++] = a[j++];
    for (int t = l; t < r; ++t)
        a[t] = aux[t];
}

template <typename T, typename Cmp>
static void merge_sort(T *a, int n, const Cmp &cmp)
{
    if (n <= 1)
        return;
    T *aux = new T[n];
    merge_sort_impl(a, aux, 0, n, cmp);
    delete[] aux;
}

// ============================= Polar Comparator ===============================

struct PolarOrder
{
    Point p0; // anchor
    bool operator()(const Point &a, const Point &b) const noexcept
    {
        long double o = Orientation::orient(p0, a, b);
        if (o > 0)
            return true; // a before b
        if (o < 0)
            return false; // b before a
        // collinear: nearer first (we'll prune later to keep farthest per ray)
        return Distance::sq(p0, a) < Distance::sq(p0, b);
    }
};

// ============================= ConvexHullBuilder ==============================

class ConvexHullBuilder
{
    CollinearPolicy policy_;
    Point *pts_; // input points (owned)
    int n_;      // number of points

    Point *hull_; // result buffer (owned)
    int hsz_;     // hull size

public:
    explicit ConvexHullBuilder(CollinearPolicy p = CollinearPolicy::KeepExtremeOnly)
        : policy_(p), pts_(nullptr), n_(0), hull_(nullptr), hsz_(0) {}

    ~ConvexHullBuilder()
    {
        delete[] pts_;
        delete[] hull_;
    }

    void setPolicy(CollinearPolicy p) noexcept { policy_ = p; }

    // Takes ownership of a copy
    void setPoints(const Point *p, int n)
    {
        delete[] pts_;
        n_ = n;
        pts_ = new Point[n_];
        for (int i = 0; i < n_; ++i)
            pts_[i] = p[i];
    }

    const Point *data() const noexcept { return hull_; }
    int size() const noexcept { return hsz_; }

private:
    static bool less_yx(const Point &a, const Point &b) noexcept
    {
        if (!equal_coord(a.y, b.y))
            return a.y < b.y;
        return a.x < b.x;
    }

    // Remove exact duplicates in-place, return new size (O(n^2) worst; fine for demo; can sort first if desired)
    static int dedup_exact(Point *a, int n)
    {
        int w = 0;
        for (int i = 0; i < n; ++i)
        {
            bool seen = false;
            for (int j = 0; j < w; ++j)
            {
                if (equal_point(a[i], a[j]))
                {
                    seen = true;
                    break;
                }
            }
            if (!seen)
                a[w++] = a[i];
        }
        return w;
    }

    void select_anchor_swap0()
    {
        // Put lowest (y, then x) at index 0
        int idx = 0;
        for (int i = 1; i < n_; ++i)
        {
            if (less_yx(pts_[i], pts_[idx]))
                idx = i;
        }
        if (idx != 0)
        {
            Point tmp = pts_[0];
            pts_[0] = pts_[idx];
            pts_[idx] = tmp;
        }
    }

    // After sorting by angle from pts_[0], prune collinear runs keeping farthest
    // Result is written back into pts_[0..m-1], returns m
    int prune_collinear_runs()
    {
        if (n_ <= 1)
            return n_;
        int w = 1;
        for (int i = 1; i < n_;)
        {
            int j = i;
            while (j + 1 < n_ && Orientation::orient(pts_[0], pts_[i], pts_[j + 1]) == 0.0L)
            {
                ++j;
            }
            // pts_[i..j] are collinear w.r.t anchor. Because of nearer-first comparator,
            // the farthest is at j. Keep that single farthest.
            pts_[w++] = pts_[j];
            i = j + 1;
        }
        return w;
    }

public:
    void build()
    {
        // Clear previous hull
        delete[] hull_;
        hull_ = nullptr;
        hsz_ = 0;
        if (n_ == 0)
            return;
        if (n_ == 1)
        {
            hull_ = new Point[1];
            hull_[0] = pts_[0];
            hsz_ = 1;
            return;
        }

        // 0) Deduplicate exact duplicates to avoid degenerate behavior
        n_ = dedup_exact(pts_, n_);
        if (n_ == 1)
        {
            hull_ = new Point[1];
            hull_[0] = pts_[0];
            hsz_ = 1;
            return;
        }

        // 1) Select anchor
        select_anchor_swap0();
        const Point anchor = pts_[0];

        // 2) Stable sort points [1..n_-1] by polar angle around anchor
        PolarOrder cmp{anchor};
        merge_sort(pts_ + 1, n_ - 1, cmp);

        // 3) Prune collinear rays from anchor (keep farthest only)
        n_ = prune_collinear_runs();
        if (n_ == 2)
        { // All points collinear with anchor
            hull_ = new Point[2];
            hull_[0] = pts_[0];
            hull_[1] = pts_[1];
            hsz_ = 2;
            return;
        }

        // 4) Graham scan using custom Stack
        Stack<Point> st;
        st.push(pts_[0]);
        st.push(pts_[1]);

        for (int i = 2; i < n_; ++i)
        {
            const Point &p = pts_[i];
            // For KeepExtremeOnly: pop while turn is not strictly left (<= 0)
            // For KeepAllOnEdges : pop while right turn (< 0) only
            while (st.size() >= 2)
            {
                const Point &b = st.top();
                const Point &a = st.second_from_top();
                long double o = Orientation::orient(a, b, p);
                bool pop_it = (policy_ == CollinearPolicy::KeepExtremeOnly) ? (o <= 0.0L) : (o < 0.0L);
                if (!pop_it)
                    break;
                DBG(cout << "Pop (" << b.x << "," << b.y << ")\n";)
                st.pop();
            }
            st.push(p);
            DBG(cout << "Push (" << p.x << "," << p.y << ")\n";)
        }

        // 5) Materialize stack into hull array (stack is LIFO; we need to reverse)
        hsz_ = static_cast<int>(st.size());
        hull_ = new Point[hsz_];
        for (int i = hsz_ - 1; i >= 0; --i)
        {
            hull_[i] = st.top();
            st.pop();
        }
    }
};

// ============================= Demo / Main ====================================

static void print_points(const Point *p, int n, const char *label)
{
    cout << label << " (" << n << "):\n";
    for (int i = 0; i < n; ++i)
    {
        cout << p[i].x << " " << p[i].y << "\n";
    }
}

int main()
{
    // Read from stdin if provided; else use a sample set
    // Input format:
    // n
    // x1 y1
    // ...
    int n;
    if (cin >> n)
    {
        if (n <= 0)
        {
            cout << "0\n";
            return 0;
        }
        Point *pts = new Point[n];
        for (int i = 0; i < n; ++i)
        {
            cin >> pts[i].x >> pts[i].y;
        }
        ConvexHullBuilder hull(CollinearPolicy::KeepExtremeOnly);
        hull.setPoints(pts, n);
        hull.build();
        const Point *h = hull.data();
        int m = hull.size();
        cout << m << "\n";
        for (int i = 0; i < m; ++i)
        {
            cout << h[i].x << " " << h[i].y << "\n";
        }
        delete[] pts;
        return 0;
    }
    else
    {
        // Fallback demo dataset
        Point demo[] = {
            {3, 7}, {5, 4}, {9, 21}, {6, 14}, {0, 20}, {2, 0}, {-5, 10}, {10, 8}, {0, 2}, {0, 0}, {4, 0}, {4, 0}, {0, 0}, {2, 0}, // duplicates to test dedup
            {5, 4},
            {6, 14} // duplicates to test dedup
        };
        n = static_cast<int>(sizeof(demo) / sizeof(demo[0]));
        ConvexHullBuilder hull(CollinearPolicy::KeepExtremeOnly);
        hull.setPoints(demo, n);
        hull.build();
        const Point *h = hull.data();
        int m = hull.size();
        print_points(demo, n, "Input");
        print_points(h, m, "Hull");
    }
    return 0;
}
