# School Database Management System

This is a C program developed as part of the Check point project under the ScaleUp Velocity program for academic purposes. It provides an efficient way to manage a school's student database, implemented using structures and linked lists. It also incorporates encryption functionality using OpenSSL libraries for secure data handling.

## Key Features

1. **Database Initialization and Management:** The functions `INITDB()`, `PRINTDB()`, `addStudToDB()`, `addTopStud()`, `addFailedStud()`, `deleteStudent()`, `deleteStudNode()` handle various aspects of database management including initialization, addition, and deletion of students in/from the database.

2. **Student Handling:** The functions `createStudent()`, `findStudentByFullName()`, `insertStudent()`, `editStudentByPhone()`, `deleteStudentByPhone()` are responsible for creating a new student, searching a student by full name, inserting a new student, editing student data by phone number and deleting student by phone number respectively.

3. **Performance Assessment:** The functions `averageOfLevelAndCourse()`, `calculateLvlAverage()`, `searchStudent()`, `topOutstanding()`, `failed()` are used for assessing student performance at various levels and courses, searching students, and finding top outstanding and failed students.

4. **Error Handling and Encryption:** The functions `handleErrors()` and `decrypt()` are for error handling and decrypting student data using OpenSSL libraries.

5. **User Interaction:** The `menu()` function is used for displaying various menu options to the user.

## Memory Management

Memory management in this project is handled primarily by the standard C library functions `malloc` and `free`. These functions are used to dynamically allocate and deallocate memory for Student and StudNode structures when students are added or removed from the database.

Dynamically allocated memory provides flexibility and efficiency, enabling the program to handle varying numbers of students without wastage of memory resources. The structures `Student` and `StudNode` are stored in a linked list, a dynamic data structure that can grow or shrink as required.

It's crucial to note that any memory that is allocated is also responsibly freed when no longer required to prevent memory leaks, thus ensuring efficient memory management.

### Mempry Repoet
```bash
==332909== 
==332909== HEAP SUMMARY:
==332909==     in use at exit: 0 bytes in 0 blocks
==332909==   total heap usage: 1,471,364 allocs, 1,471,364 frees, 79,625,896 bytes allocated
==332909== 
==332909== All heap blocks were freed -- no leaks are possible
==332909== 
==332909== For lists of detected and suppressed errors, rerun with: -s
==332909== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

```

## Initialization

**Note:** The `INITDB()` function, which initializes the database, may take several minutes to complete. Please be patient as the database is set up with the required data structures and initial data.

## How to Compile and Run

1. Compile:

If you have a makefile, you can use the make command in your terminal:
```bash
make
```
This will generate an executable file.

2. Run:

You can run the generated executable as follows:
```bash
./school
```

## Disclaimer
This project is intended for educational purposes only. It does not handle real-life student data. The encryption function is used as a demonstration and might not meet security standards needed for a production system. Always ensure proper security measures when dealing with real-world, sensitive data.
