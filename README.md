# 🌟 Convex Hull Implementation using Graham's Scan 🌟

This project implements the **Graham's Scan algorithm** from scratch to compute the convex hull of a set of 2D points. The convex hull is the smallest convex polygon that encloses all the points in a given set. This implementation avoids the use of any inbuilt functions, providing a deeper understanding of algorithmic logic and efficient handling of large datasets while ensuring accuracy.

## ✨ Features
- 🚀 **Custom Convex Hull Calculation**: Implements the Graham's Scan algorithm step-by-step without using pre-built libraries or functions.
- 📊 **Large Dataset Handling**: The code is optimized to manage and compute convex hulls for large sets of 2D points.
- 🔍 **Complete Control Over Algorithm**: Direct implementation of sorting, stack operations, and vector mathematics from scratch, ensuring a strong grasp of the underlying logic.

## 🔎 Algorithm Overview
Graham's Scan is a classical algorithm used to compute the convex hull for a set of 2D points. The algorithm works by:
1. 🔹 Finding the point with the lowest y-coordinate (and leftmost if ties).
2. 🔸 Sorting the remaining points based on the polar angle they form with the anchor point using merge sort.
3. 🔺 Implemented a stack-like functionality using the linked list structure to iteratively build the convex hull by checking the orientation of triplets of points.

## ⚙️ Requirements
- 💻 **C++ Compiler**: The code is written in C++ and does not depend on any external libraries. Any standard C++ compiler (like g++, clang++) can be used to compile and run the code.

## 🔧 Practical Use Cases

Convex hulls have practical applications in various fields, such as geographic data analysis, pathfinding in robotics, collision detection in gaming, and data clustering, where defining boundary regions is important for efficiency and accuracy.

## 📚 Learning Outcomes
Through this project, you can:
- 📈 **Master the Graham's Scan algorithm** by implementing each part from scratch.
- 💡 **Deepen your understanding** of computational geometry and the mathematical concepts behind convex hulls.
- 🛠️ **Strengthen problem-solving skills** by avoiding inbuilt functions and creating efficient solutions manually.

## 📬 Contact
Feel free to reach out if you have any questions or feedback! 😄
