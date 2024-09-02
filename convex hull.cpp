#include<iostream>
#include<time.h>
using namespace std;

#define size 11

struct angle {

    int x_diff;
    int y_diff;

};

struct coord {

    int x;
    int y;
    angle ang;

};

struct Node {

    coord c;
    Node* next;

};

int mod(int num) {

    if( num < 0 ) {
        return -num;
    }

    return num;

}

void pause() {

    getchar();

}

/*stack implementation*/
void addFirst(Node** head, coord num){

    Node* node = new Node();
    node->c.x = num.x;
    node->c.y = num.y;
    node->next = NULL;

    if( *head == NULL ){
        *head = node;
    }
    else{
        node->next = *head;
        *head = node;
    }

}

void removeFirst(Node** head){

    Node* temp = *head;
    *head = (*head) -> next;

    free(temp);

}

/*cross product of two 2-D vectors*/
int crossProduct(coord num3, coord num2, coord num1){

    coord v1;
    v1.x = num2.x - num1.x;
    v1.y = num2.y - num1.y;

    coord v2;
    v2.x = num3.x - num2.x;
    v2.y = num3.y - num2.y;

    int x = (v1.x)*(v2.y) - (v2.x)*(v1.y);
    
    if( x > 0 ){
        return 1;
    }
    else{
        return 0;
    }

}

/*angle from +ve X w.r.t start point*/
angle findAngle(coord start, coord curr) {

    angle ans;
    
    ans.y_diff = curr.y - start.y;
    ans.x_diff = curr.x - start.x;

    return ans;

}

/*comparing two angles*/
int larger(angle a1, angle a2) {

    if( a1.x_diff * a2.y_diff == a2.x_diff * a1.y_diff ) {
        
        if( a1.y_diff > a2.y_diff || mod(a1.x_diff) > mod(a2.x_diff) ) {
            return 1;
        }
        else {
            return 2;
        }

    }
    else if( a1.x_diff < 0 && a2.x_diff >= 0 ) {
        return 1;
    }
    else if( a1.x_diff >= 0 && a2.x_diff < 0 ) {
        return 2;
    }
    else if( a1.x_diff * a2.y_diff < a2.x_diff * a1.y_diff ) {
        return 1;
    }
    else {
        return 2;
    }

}

/*finding the start point*/
void findStart(coord* coordArr, int n) {

    int start = 0;
    for(int i=0; i<n; i++) {

        coord curr = coordArr[i];
        if( curr.y < coordArr[start].y ) {

            start = i;
        
        }
        else if( (curr.y == coordArr[start].y) && (curr.x < coordArr[start].x) ) {

            start = i;

        }

    }
    
    coord temp = coordArr[0];
    coordArr[0] = coordArr[start];
    coordArr[start] = temp;

}

/*storing angle of coordinates*/
void storeAngle(coord coordArr[]){

    findStart(coordArr, size);

    for(int i=0; i<size; i++){

        angle temp = findAngle(coordArr[0], coordArr[i]);
        coordArr[i].ang = temp;
    
    }

}

/*merging sorted parts of coordinates array*/
void mergeSortedParts(coord* arr, int start, int end) {

    int mid = (start + end) / 2;

    int size1 = mid - start + 1;
    int size2 = end - mid;

    coord* arr1 = new coord[size1];
    coord* arr2 = new coord[size2];

    for(int i = 0 ; i<size1; i++) {
        arr1[i] = arr[i + start];
    }

    for(int i = 0 ; i<size2 ; i++) {
        arr2[i] = arr[i + mid + 1];
    }

    int i = 0, j = 0;

    while(i < size1 && j < size2) {

        if( larger(arr1[i].ang, arr2[j].ang) == 2) {

            arr[start] = arr1[i];
            start++;
            i++;
        
        }
        else {

            arr[start] = arr2[j];
            start++;
            j++;

        }

    }

    while(i < size1) {

        arr[start] = arr1[i];
        start++;
        i++;

    }

    while(j < size2) {

        arr[start] = arr2[j];
        start++;
        j++;

    }

    delete []arr1;
    delete []arr2;

}

/*merge sorting on basis of angle and distance*/
void mergeSort(coord* arr, int start, int end) {

    // base case
    if(start >= end) {
        return;
    }

    int mid = (start + end) / 2;

    mergeSort(arr, start, mid);
    mergeSort(arr, mid + 1, end);

    mergeSortedParts(arr, start, end);

}

/*function to print stack*/
void printStack(Node* head) {

    while(head != NULL) {

        cout << "(" << head -> c.x << ", " << head -> c.y << ")" << endl;
        head = head -> next;

    }
    cout << endl;

}

/*function to find hull of given points*/
void findingHull(coord points[], Node** head){

    addFirst(head, points[0]);
    addFirst(head, points[1]);
    printStack(*head);
    pause();

    for(int i=2; i<size; i++){

        while(1){

            if( (*head) -> next == NULL ){

                addFirst(head, points[i]);
                printStack(*head);
                pause();
                break;
            
            }
            else{

                if(!crossProduct(points[i], (*head) -> c, (*head) -> next -> c)){
                    removeFirst(head);
                    printStack(*head);
                    pause();
                }
                else{
                    addFirst(head, points[i]);
                    printStack(*head);
                    pause();
                    break;
                }
            
            }

        }
    }
    
}

int main()
{
    coord coordArr[size] = {{3, 7}, {5, 4}, {9, 21}, {6, 14}, {0, 20}, {2, 0}, {-5, 10},
    {10, 8}, {0, 2}, {0, 0}, {4, 0}};
    
    // coord coordArr[size];
    // srand(time(NULL));
    // for(int i=0; i<size; i++){
    //     int n = rand()%10;
    //     int m = rand()%10;
    //     coordArr[i].x = n;
    //     coordArr[i].y = m;
    // }

    storeAngle(coordArr);
    mergeSort(coordArr, 1, size - 1);
    
    cout << "Given coordinates are:-" << endl << endl;
    for(int i=0; i<size; i++){
        cout << "(" << coordArr[i].x << ", " << coordArr[i].y << ")" << endl;
    }

    cout << endl;
    Node* head = NULL;

    cout << "Intermediate stacks of coordinates are:-" << endl << endl;
    findingHull(coordArr, &head);

    cout << "Coordinates of Convex Hull are:-" << endl << endl;
    while(head != NULL){
        cout << "(" << head -> c.x << ", " << head -> c.y << ")" << endl;
        head = head -> next;
    }

    return 0;

}