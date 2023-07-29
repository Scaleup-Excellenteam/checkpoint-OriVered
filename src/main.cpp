// Header files
#include<stdio.h>
#include<stdlib.h>
#include<openssl/conf.h>
#include<openssl/evp.h>
#include<openssl/err.h>
#include<string.h>

// Macro Definitions
#define NUM_LEVELS 12
#define NUM_CLASSES 10
#define NUM_TOP 10
#define NUM_COURSES 10
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define MAX_USER_INPUT 15

// Student Structure
struct Student {
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level;
    int class_num;
    int grades[NUM_COURSES];
    struct Student* nextStud;
};

// Node structure for the linked list of students
struct StudNode {
    struct Student* data;
    struct StudNode* next;
};

// School Structure - contains multi-dimensional arrays for different categories of students
struct School {
    struct StudNode* DB[NUM_LEVELS][NUM_CLASSES];
    struct StudNode* TOPS[NUM_LEVELS][NUM_COURSES];
    struct StudNode* FAILED[NUM_LEVELS];
};

// Global school structure
static struct School s;

// Function declarations for database initialization and management
void INITDB();
void PRINTDB();
void addStudToDB(struct StudNode* newStudent);
void addTopStud(struct Student*);
void addFailedStud(struct Student*);
void deleteStudent(struct School* school, struct Student* student);
void deleteStudNode(struct StudNode** head, struct Student* student);
void cleanupSchool();
void deleteNodesWithoutStudent(struct StudNode** head);
void deleteAllNodes(struct StudNode** head);

// Function declarations for error handling and encryption
void handleErrors();
int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);


// Function declarations for student handling
struct StudNode* createStudent(char* fname, char* lname, char* phone, int level, int class_num, int* grades);
struct Student* findStudentByFullName(char* fname, char* lname);
void insertStudent();
void editStudentByPhone();
void deleteStudentByPhone();

// Function declaration for user interaction
void menu();

// Function declarations for performance assessment
void averageOfLevelAndCourse();
void calculateLvlAverage(int level_num, int course_num);
void searchStudent();
void topOutstanding();
void failed();

// Main function
int main()
{
    INITDB(); // Initialize the database

    menu();   // Display menu options to the user

    cleanupSchool(); //Delete all Structure and cleanup memory
    return 0;
}

/**
 * Initializes the database by reading encrypted student data from a file,
 * decrypts it, and adds the decrypted student records to the database.
 */
void INITDB() {
    FILE* iptr = fopen("students_encrypted.txt", "rb");  // changed to binary mode
    if (iptr == NULL) {
        printf("failed to open input file! \n");
        return;
    }

    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level, class_num, grades[NUM_COURSES];
    struct StudNode* newStudent;


    unsigned char ciphertext[128];
    char decryptedtext[128];


    /* A 256 bit key */
    unsigned char key[32] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                           0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                           0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
                           0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31
    };

    /* A 128 bit IV */
    unsigned char iv[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                          0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
    };

    while (!feof(iptr))
    {
        int len;
        // read length of ciphertext
        fread(&len, sizeof(int), 1, iptr);
        // read the ciphertext
        fread(ciphertext, sizeof(unsigned char), len, iptr);

        // decrypt the ciphertext
        int decryptedtext_len = decrypt(ciphertext, len, key, iv, (unsigned char*)decryptedtext);

        // Null-terminating decrypted text
        decryptedtext[decryptedtext_len] = '\0';

        sscanf(decryptedtext, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            fname, lname, phone, &level, &class_num,
            &grades[0], &grades[1], &grades[2], &grades[3], &grades[4],
            &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]);

        newStudent = createStudent(fname, lname, phone, level, class_num, grades);

        addStudToDB(newStudent);
        addTopStud(newStudent->data);
        addFailedStud(newStudent->data);

    }

    fclose(iptr);
}


/**
 * Allows the user to insert a new student into the database by entering the student's details.
 * The function prompts the user to input the student's information and adds the new student to the database.
 */
void insertStudent() {
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level, class_num, grades[NUM_COURSES];
    char userInput[100];

    do {
        printf("Enter the student's details or type 'q' to exit.\n");
        printf("Format: Firstname Lastname Phone Level: 1-12 Class_number: 1-10 10-Grades\n");
        printf("Example: Dinorah Wonderly 0560344096 9 9 97 64 82 89 99 92 75 84 72 63\n");

        fgets(userInput, sizeof(userInput), stdin);

        // Exit the function if the user enters 'q'
        if (userInput[0] == 'q') {
            return;
        }

        int num = sscanf(userInput, "%s %s %s %d %d", fname, lname, phone, &level, &class_num);
        int i;
        if (num == 5) {
            // Split grades from the rest of the string
            char* gradesStr = userInput;
            for (i = 0; i < 5; i++) {
                gradesStr = strchr(gradesStr, ' ') + 1;
            }

            // Parse grades
            int numGrades = 0;
            char* grade = strtok(gradesStr, " ");
            while (grade != NULL && numGrades < NUM_COURSES) {
                grades[numGrades++] = atoi(grade);
                grade = strtok(NULL, " ");
            }

            // Create the new student
            struct StudNode* newStudent = createStudent(fname, lname, phone, level - 1, class_num - 1, grades);
            addStudToDB(newStudent);
            addTopStud(newStudent->data);
            addFailedStud(newStudent->data);

            printf("Student details entered successfully.\n");
            return;
        }
        else {
            printf("Incorrect format. Please try again.\n");
        }
    } while (1);
}

/**
 * Adds a new student to the specified level and class in the database.
 *
 * @param newStudent The new student to be added to the database.
 */
void addStudToDB(struct StudNode* newStudent) {
    int level = newStudent->data->level;
    int class_num = newStudent->data->class_num;
    // Link the new student to the existing list of students
    newStudent->next = s.DB[level - 1][class_num - 1];
    s.DB[level - 1][class_num - 1] = newStudent;
}

/**
 * Adds a student to the top students list based on their performance in a particular course.
 *
 * @param student The student to be added to the top students list.
 */
void addTopStud(struct Student* student)
{
    int level = student->level;
    int crs;

    for (crs = 0; crs < NUM_COURSES; crs++)
    {
        struct StudNode* newStudNode = (struct StudNode*)malloc(sizeof(struct StudNode));
        newStudNode->data = student;
        newStudNode->next = NULL;

        struct StudNode* currentStud = s.TOPS[level - 1][crs];
        struct StudNode* prevStud = NULL;

        // Find the correct position to insert new node
        while (currentStud != NULL && student->grades[crs] < currentStud->data->grades[crs])
        {
            prevStud = currentStud;
            currentStud = currentStud->next;
        }

        // If the list was empty or new node is to be inserted before the first node
        if (prevStud == NULL)
        {
            newStudNode->next = s.TOPS[level - 1][crs];
            s.TOPS[level - 1][crs] = newStudNode;
        }
        else
        {
            newStudNode->next = currentStud;
            prevStud->next = newStudNode;
        }
    }
}


/**
 * Adds a student to the list of failed students based on their performance in all courses.
 *
 * @param student The student to be added to the list of failed students.
 */
void addFailedStud(struct Student* student)
{
    int level = student->level;
    int fail = 0;

    for (int crs = 0; crs < NUM_COURSES; crs++)
    {
        if (student->grades[crs] < 56)
            fail = 1;
    }

    if (fail)
    {
        struct StudNode* newStudNode = (struct StudNode*)malloc(sizeof(struct StudNode));
        newStudNode->data = student;
        newStudNode->next = NULL;

        struct StudNode* currentStud = s.FAILED[level - 1];
        struct StudNode* prevStud = NULL;

        // Find the correct position to insert new node
        while (currentStud != NULL && strcmp(student->lname, currentStud->data->lname) > 0)
        {
            prevStud = currentStud;
            currentStud = currentStud->next;
        }

        // If the list was empty or new node is to be inserted before the first node
        if (prevStud == NULL)
        {
            newStudNode->next = s.FAILED[level - 1];
            s.FAILED[level - 1] = newStudNode;
        }
        else
        {
            newStudNode->next = currentStud;
            prevStud->next = newStudNode;
        }
    }
}


/**
 * Prints the entire content of the database, showing all students' details.
 */
void PRINTDB() {
    int i, j, k;

    for (i = 0; i < NUM_LEVELS; i++) {
        for (j = 0; j < NUM_CLASSES; j++) {
            struct StudNode* studNode = s.DB[i][j];
            while (studNode != NULL) {
                struct Student* student = studNode->data;
                printf("%s %s %s %d %d ", student->fname, student->lname, student->phone, student->level + 1, student->class_num + 1);
                for (k = 0; k < NUM_COURSES; k++) {
                    printf("%d ", student->grades[k]);
                }
                printf("\n");

                studNode = studNode->next;
            }
        }
    }
}


/**
 * Creates a new student record and returns the corresponding StudentNode.
 *
 * @param fname     The first name of the student.
 * @param lname     The last name of the student.
 * @param phone     The phone number of the student.
 * @param level     The level of the student (1-12).
 * @param class_num The class number of the student (1-10).
 * @param grades    An array containing the grades of the student for different courses.
 * @return A new StudentNode containing the student's information.
 */
struct StudNode* createStudent(char* fname, char* lname, char* phone, int level, int class_num, int* grades) {
    struct StudNode* newStudNode = (struct StudNode*)malloc(sizeof(struct StudNode));
    struct Student* newStudent = (struct Student*)malloc(sizeof(struct Student));
    int i;
    strcpy(newStudent->fname, fname);
    strcpy(newStudent->lname, lname);
    strcpy(newStudent->phone, phone);
    newStudent->level = level;
    newStudent->class_num = class_num;
    for (i = 0; i < NUM_COURSES; i++) {
        newStudent->grades[i] = grades[i];
    }
    newStudNode->data = newStudent;
    newStudNode->next = NULL;

    return newStudNode;
}

/**
 * Displays a menu and allows the user to interact with the database by choosing various commands.
 * The user can insert a new student, compute averages, view top students, view failed students,
 * search for a student, edit student details, and delete a student from the database.
 */
void menu() {
    char userChoice;

    do {
        printf("Enter the command:\n");
        printf("i - Insert student\n");
        printf("a - Compute average\n");
        printf("o - Show top 10 outstanding students\n");
        printf("d - Delete student\n");
        printf("e - edit student grade\n");
        printf("s - search student\n");
        printf("f - Show level failed students\n");
        printf("q - Exit\n");

        scanf(" %c", &userChoice);

        switch (userChoice) {
        case 'i':
            insertStudent();
            break;
        case 'a':
            averageOfLevelAndCourse();
            break;
        case 'o':
            topOutstanding();
            break;
        case 'f':
            failed();
            break;
        case 'e':
            editStudentByPhone();
            break;
        case 's':
            searchStudent();
            break;
        case 'd':
            deleteStudentByPhone();
            break;
        case 'q':
            printf("Exiting program.\n");
            break;
        default:
            printf("Unrecognized command. Please try again.\n");
        }
        printf("\n");
    } while (userChoice != 'q');
}

/**
 * Searches for a student in the database using their first name and last name.
 * If the student is found, their details are displayed; otherwise, a message is shown indicating that the student was not found.
 */
void searchStudent() {
    char nameInput[MAX_NAME_LEN * 2 + 1];
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];

    do {
        printf("Enter the first and last name of the student or type 'q' to exit.\n");
        printf("Format: Firstname Lastname\n");
        printf("Example: John Doe\n");

        fgets(nameInput, sizeof(nameInput), stdin);

        // Exit the function if the user enters 'q'
        if (nameInput[0] == 'q') {
            return;
        }

        int num = sscanf(nameInput, "%s %s", fname, lname);
        if (num == 2) {
            struct Student* student = findStudentByFullName(fname, lname);
            if (student != NULL) {
                // The student was found
                printf("%s %s %s %d %d ", student->fname, student->lname, student->phone, student->level + 1, student->class_num + 1);
                int k;
                for (k = 0; k < NUM_COURSES; k++) {
                    printf("%d ", student->grades[k]);
                }
                return;
            }
            else {
                printf("Student %s %s was not found.\n", fname, lname);
                return;
            }
        }
        else {
            printf("Incorrect input. Please try again.\n");
        }
    } while (1);
}


/**
 * Searches for a student in the database by their first name and last name.
 * It iterates through all levels and classes to find the student with the provided name.
 * If a student with matching first name and last name is found, it returns a pointer to the student.
 * Otherwise, it returns NULL to indicate that the student is not found in the database.
 *
 * @param fname The first name of the student to find.
 * @param lname The last name of the student to find.
 * @return A pointer to the Student structure representing the found student, or NULL if not found.
 */
struct Student* findStudentByFullName(char* fname, char* lname) {
    int lvl, cls;

    // Iterate through all levels and classes to find the student
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        for (cls = 0; cls < NUM_CLASSES; cls++) {
            struct StudNode* currentStud = s.DB[lvl][cls];

            while (currentStud != NULL) {
                if (strcmp(currentStud->data->fname, fname) == 0 && strcmp(currentStud->data->lname, lname) == 0) {
                    return currentStud->data;
                }

                currentStud = currentStud->next;
            }
        }
    }

    return NULL; // Return NULL if student is not found
}

/**
 * Searches for a student in the database using their first name and last name.
 * If the student is found, their details are displayed; otherwise, a message is shown indicating that the student was not found.
 */
void calculateLvlAverage(int level_num, int course_num) {
    int level_avg = 0; // Initialize level_avg
    int student_counter = 0; // Initialize student_counter

    for (int i = 0; i < NUM_CLASSES; i++) {
        struct Student* student = s.DB[level_num - 1][i]->data;
        while (student != NULL) {
            level_avg += student->grades[course_num - 1]; // add grade to level_avg
            student_counter++; // add to level student counter
            student = student->nextStud;
        }
    }

    // Check if there are any students in this level and course
    if (student_counter > 0) {
        printf("Average for Level %d, Course %d: %.2f\n", level_num, course_num, (float)level_avg / student_counter);
    }
    else {
        printf("No students in Level %d, Course %d\n", level_num, course_num);
    }
}

/**
 * Prompts the user to input a level number and a course number.
 * Calculates and displays the average grade of students at the specified level and course.
 * The user can terminate the function by typing 'q'.
 * The function calls the calculateLvlAverage(level_num, course_num) function to perform the calculation.
 *
 * @param None.
 * @return None.
 */
void averageOfLevelAndCourse() {
    char avgInput[100];
    int level_num;
    int course_num;

    do {
        printf("Enter the level num and the course num or type 'q' to exit.\n");
        printf("Format: Level: 1-12 Course: 1-10\n");
        printf("Example: 9 9\n");

        fgets(avgInput, sizeof(avgInput), stdin);

        // Exit the function if the user enters 'q'
        if (avgInput[0] == 'q') {
            return;
        }

        int num = sscanf(avgInput, "%d %d", &level_num, &course_num);
        if (num == 2) {
            calculateLvlAverage(level_num, course_num);
            return;
        }
        else {
            printf("Incorrect input. Please try again.\n");
        }
    } while (1);
}

/**
 * Prompts the user to input a level number and a course number.
 * Calculates and displays the average grade of students at the specified level and course.
 * The user can terminate the function by typing 'q'.
 * The function calls the calculateLvlAverage(level_num, course_num) function to perform the calculation.
 *
 * @param None.
 * @return None.
 */
void deleteStudentByPhone() {
    char phoneInput[MAX_PHONE_LEN];
    char phoneToDel[MAX_PHONE_LEN];
    int lvl, cls;

    do {
        printf("Enter the phone number of the student to delete or type 'q' to exit.\n");
        printf("Format: 10 digits\n");
        printf("Example: 0572233920\n");

        fgets(phoneInput, sizeof(phoneInput), stdin);

        // Exit the function if the user enters 'q'
        if (phoneInput[0] == 'q') {
            return;
        }

        int num = sscanf(phoneInput, "%s", phoneToDel);
        if (num == 1) {
            // Iterate through all levels and classes to find the student
            for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
                for (cls = 0; cls < NUM_CLASSES; cls++) {
                    struct StudNode* currentStud = s.DB[lvl][cls];

                    while (currentStud != NULL) {
                        if (strcmp(currentStud->data->phone, phoneToDel) == 0) {
                            // If student is at the head of the list
                            deleteStudent(&s, currentStud->data);
                            printf("Student with phone %s deleted.\n", phoneToDel);
                            return;
                        }
                        currentStud = currentStud->next;
                    }
                }
            }

            printf("Student with phone %s not found.\n", phoneToDel);
            return;
        }
        else {
            printf("Incorrect input. Please try again.\n");
        }
    } while (1);
}

/**
 * Prompts the user to input a phone number and then edits the grades of a specific student.
 * Searches for the student with the provided phone number and allows the user to enter the course number and new grade.
 * If the course number is valid, updates the student's grade for that course.
 * The user can terminate the function by typing 'q'.
 *
 * @param None.
 * @return None.
 */
void editStudentByPhone() {
    char phoneInput[MAX_PHONE_LEN];
    char phoneToEdit[MAX_PHONE_LEN];
    int courseNum;
    int newGrade;
    int lvl, cls;

    do {
        printf("Enter the phone number of the student to edit or type 'q' to exit.\n");
        printf("Format: 10 digits\n");
        printf("Example: 0572233920\n");

        fgets(phoneInput, sizeof(phoneInput), stdin);

        // Exit the function if the user enters 'q'
        if (phoneInput[0] == 'q') {
            return;
        }

        int num = sscanf(phoneInput, "%s", phoneToEdit);
        if (num == 1) {
            // Iterate through all levels and classes to find the student
            for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
                for (cls = 0; cls < NUM_CLASSES; cls++) {
                    struct StudNode* currentStud = s.DB[lvl][cls];

                    while (currentStud != NULL) {
                        if (strcmp(currentStud->data->phone, phoneToEdit) == 0) {
                            printf("Enter course number to edit (0-%d): ", NUM_COURSES - 1);
                            scanf("%d", &courseNum);
                            printf("Enter new grade for course %d: ", courseNum);
                            scanf("%d", &newGrade);
                            // Check if entered courseNum is valid
                            if (courseNum >= 0 && courseNum < NUM_COURSES) {
                                currentStud->data->grades[courseNum] = newGrade;
                                printf("Grade for course %d has been updated to %d.\n", courseNum, newGrade);
                            }
                            else {
                                printf("Invalid course number.\n");
                            }
                            return;
                        }

                        currentStud = currentStud->next;
                    }
                }
            }

            printf("Student with phone %s not found.\n", phoneToEdit);
            return;
        }
        else {
            printf("Incorrect input. Please try again.\n");
        }
    } while (1);
}

/**
 * Prompts the user to input a level and a course number.
 * Displays the top students for the specified level and course based on their grades.
 * Uses data stored in the s.TOPS data structure to find the top students.
 * The user can terminate the function by typing 'q'.
 *
 * @param None.
 * @return None.
 */
void topOutstanding() {
    int level, course;

    do {
        printf("Enter the level and course number or 'q' to exit:\n");
        printf("Format: Level: 1-12 Course: 1-10\n");
        printf("Example: 9 9\n");
        char input[100];
        fgets(input, sizeof(input), stdin);

        // Exit the function if the user enters 'q'
        if (input[0] == 'q') {
            return;
        }

        int num = sscanf(input, "%d %d", &level, &course);

        // Validate input
        if (num != 2 || level < 1 || level > NUM_LEVELS || course < 1 || course > NUM_COURSES) {
            printf("Invalid level or course number. Please try again.\n");
        }
        else {
            break;
        }
    } while (1);


    printf("Top students for Level %d, Course %d:\n", level, course);
    struct StudNode* topList = s.TOPS[level - 1][course - 1];
    if (topList == NULL) {
        printf("No student in this Course \n");
    }

    for (int i = 0; i < NUM_TOP; i++) {
        struct Student* topStudent = topList->data;
        if (topStudent == NULL) {
            printf("No student in position %d\n", i + 1);
        }
        else {
            printf(" %d) %s %s - %d \n", i + 1, topStudent->fname, topStudent->lname, topStudent->grades[course - 1]);
        }
        topList = topList->next;
    }
}

/**
 * Prompts the user to input a level number and then displays the names of students who have failed at that level.
 * Uses data stored in the s.FAILED data structure to find the failed students.
 * The user can terminate the function by typing 'q'.
 *
 * @param None.
 * @return None.
 */
void failed() {
    int level;

    do {
        printf("Enter the level or 'q' to exit:\n");
        printf("Format: Level: 1-12 \n");
        printf("Example: 9\n");
        char input[100];
        fgets(input, sizeof(input), stdin);

        // Exit the function if the user enters 'q'
        if (input[0] == 'q') {
            return;
        }

        int num = sscanf(input, "%d", &level);

        // Validate input
        if (num != 1 || level < 1 || level > NUM_LEVELS ) {
            printf("Invalid level. Please try again.\n");
        }
        else {
            break;
        }
    } while (1);


    printf("Failed students for Level %d:\n", level);
    struct StudNode* failedList = s.FAILED[level - 1];
    if (failedList == NULL) {
        printf("No student failed \n");
    }

    while (failedList != NULL) {
        struct Student* failStudent = failedList->data;
        printf("%s %s\n", failStudent->fname, failStudent->lname);
        failedList = failedList->next;
    }
}

/**
 * Decrypts the provided ciphertext using AES-256 in CBC mode.
 * The function takes ciphertext, key, IV (initialization vector), and ciphertext length as input.
 * It returns the plaintext after decryption.
 *
 * @param ciphertext The encrypted data to be decrypted.
 * @param ciphertext_len The length of the ciphertext.
 * @param key The encryption key used for decryption.
 * @param iv The initialization vector used for decryption.
 * @param plaintext The buffer to store the decrypted plaintext.
 * @return The length of the decrypted plaintext.
 */
int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output. */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at this stage. */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

/**
 * Deletes the specified Student node from a linked list.
 *
 * @param head    A pointer to the head of the linked list.
 * @param student The Student object to be deleted from the linked list.
 */
void deleteStudNode(struct StudNode** head, struct Student* student) {
    struct StudNode* currentStud = *head;
    struct StudNode* prevStud = NULL;

    while (currentStud != NULL) {
        if (currentStud->data == student) {
            // If student is at the head of the list
            if (prevStud == NULL) {
                *head = currentStud->next;
            }
            else {
                prevStud->next = currentStud->next;
            }

            free(currentStud);
            return;
        }

        prevStud = currentStud;
        currentStud = currentStud->next;
    }
}

/**
 * Deletes the specified Student from the School database and related data structures.
 *
 * @param school  The School structure representing the database.
 * @param student The Student object to be deleted from the database.
 */
void deleteStudent(struct School* school, struct Student* student) {
    int lvl, cls, crs;

    // Delete from tops structure
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        for (crs = 0; crs < NUM_COURSES; crs++) {
            deleteStudNode(&school->TOPS[lvl][crs], student);
        }
    }

    // Delete from FAILED structure
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
            deleteStudNode(&school->FAILED[lvl], student);
    }

    // Delete from DB structure
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        for (cls = 0; cls < NUM_CLASSES; cls++) {
            deleteStudNode(&school->DB[lvl][cls], student);
        }
    }


    // Free the student
    free(student);

}

/**
 * Deletes all StudNodes from the given linked list without freeing the associated Student data.
 *
 * @param head A pointer to the head of the linked list.
 */
void deleteNodesWithoutStudent(struct StudNode** head) {
    struct StudNode* currentStud = *head;
    struct StudNode* nextStud;

    while (currentStud != NULL) {
        nextStud = currentStud->next;
        currentStud->data = NULL;
        free(currentStud);  // free the node
        currentStud = nextStud;
    }

    *head = NULL;
}

/**
 * Deletes all StudNodes from the given linked list without freeing the associated Student data.
 *
 * @param head A pointer to the head of the linked list.
 */
void deleteAllNodes(struct StudNode** head) {
    struct StudNode* currentStud = *head;
    struct StudNode* nextStud;

    while (currentStud != NULL) {
        nextStud = currentStud->next;
        free(currentStud->data);  // free the student
        free(currentStud);  // free the node
        currentStud = nextStud;
    }

    *head = NULL;
}

/**
 * Cleans up all allocated memory in the School structure.
 */
void cleanupSchool() {
    int lvl, cls, crs;

    // Delete all nodes from TOPS structure without freeing student
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        for (crs = 0; crs < NUM_COURSES; crs++) {
            deleteNodesWithoutStudent(&s.TOPS[lvl][crs]);
        }
    }

    // Delete all nodes from FAILED structure without freeing student
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        deleteNodesWithoutStudent(&s.FAILED[lvl]);
    }

    // Delete all students and nodes from DB structure
    for (lvl = 0; lvl < NUM_LEVELS; lvl++) {
        for (cls = 0; cls < NUM_CLASSES; cls++) {
            deleteAllNodes(&s.DB[lvl][cls]);
        }
    }

    // Done cleaning up, set the global school structure to a known state
    memset(&s, 0, sizeof(struct School));
}


/**
 * Handles OpenSSL errors and prints error messages to the standard error stream.
 */
void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}
