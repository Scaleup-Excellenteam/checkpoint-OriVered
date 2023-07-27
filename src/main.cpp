#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LEVELS 12
#define NUM_CLASSES 10
#define NUM_TOP 10
#define NUM_COURSES 10
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define MAX_USER_INPUT 15

struct Student {
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level;
    int class_num;
    int grades[NUM_COURSES];
    struct Student* nextStud;
};

struct Tops {
    struct Student* top[NUM_TOP];
};

struct School {
    struct Student* DB[NUM_LEVELS][NUM_CLASSES];
    struct Tops course[NUM_LEVELS][NUM_COURSES];
};

static struct School s;

void INITDB();
void PRINTDB();
struct Student* createStudent(char* fname, char* lname, char* phone, int level, int class_num, int* grades);
void menu();
void insertStudent();
void average();
void topOutstanding();
void topFailed();
void addTopStud(struct Student*);

int main()
{
    INITDB();

    menu();

    return 0;
}
void INITDB() {
    FILE* iptr = fopen("students_with_class.txt", "r");
    if (iptr == NULL) {
        printf("failed to open input file! \n");
        return;
    }

    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level, class_num, grades[NUM_COURSES];
    struct Student* newStudent;

    while (fscanf(iptr, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        fname, lname, phone, &level, &class_num,
        &grades[0], &grades[1], &grades[2], &grades[3], &grades[4],
        &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]) == 15) {
        newStudent = createStudent(fname, lname, phone, level, class_num, grades);

        addTopStud(newStudent);


        // Link the new student to the existing list of students
        newStudent->nextStud = s.DB[level-1][class_num-1];
        s.DB[level-1][class_num-1] = newStudent;

    }

    fclose(iptr);
}

void PRINTDB() {
    int i, j, k;

    for (i = 0; i < NUM_LEVELS; i++) {
        for (j = 0; j < NUM_CLASSES; j++) {
            struct Student* student = s.DB[i][j];
            while (student != NULL) {
                printf("%s %s %s %d %d ", student->fname, student->lname, student->phone, student->level + 1, student->class_num + 1);
                for (k = 0; k < NUM_COURSES; k++) {
                    printf("%d ", student->grades[k]);
                }
                printf("\n");

                student = student->nextStud;
            }
        }
    }
}

void addTopStud(struct Student* student)
{
    int level = student->level;
    int crs,plc;

    for (crs = 0; crs < NUM_COURSES; crs++)
        for (plc = 0; plc < NUM_TOP; plc++) {

            if (s.course[level-1][crs].top[plc] == NULL){
                s.course[level-1][crs].top[plc] = student;
                break;
            }
            else if (student->grades[plc] > s.course[level-1][crs].top[plc]->grades[crs])
            {
                s.course[level-1][crs].top[crs] = student;
                break;
            }
        }
}

struct Student* createStudent(char* fname, char* lname, char* phone, int level, int class_num, int* grades) {
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
    newStudent->nextStud = NULL;

    return newStudent;
}

void menu() {
    char userChoice;

    do {
        printf("Enter the command:\n");
        printf("i - Insert student\n");
        printf("a - Compute average\n");
        printf("o - Show top 10 outstanding students\n");
        printf("f - Show top 10 failed students\n");
        printf("e - Exit\n");

        scanf(" %c", &userChoice);

        switch (userChoice) {
        case 'i':
            insertStudent();
            break;
        case 'a':
            average();
            break;
        case 'o':
            topOutstanding();
            break;
        case 'f':
            topFailed();
            break;
        case 'e':
            printf("Exiting program.\n");
            break;
        default:
            printf("Unrecognized command. Please try again.\n");
        }
        printf("\n");
    } while (userChoice != 'e');
}

void insertStudent() {
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int level, class_num, grades[NUM_COURSES];
    char userInput[100];

    do {
        printf("Enter the student's details or type 'e' to exit.\n");
        printf("Format: Firstname Lastname Phone Level: 1-12 Class_number: 1-10 10-Grades\n");
        printf("Example: Dinorah Wonderly 0560344096 9 9 97 64 82 89 99 92 75 84 72 63\n");

        fgets(userInput, sizeof(userInput), stdin);

        // Exit the function if the user enters 'e'
        if (userInput[0] == 'e') {
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
            struct Student* newStudent = createStudent(fname, lname, phone, level-1, class_num-1, grades);

            addTopStud(newStudent);

            // Add the student to the start of the corresponding linked list in the school database
            newStudent->nextStud = s.DB[level - 1][class_num - 1];
            s.DB[level - 1][class_num - 1] = newStudent;

            printf("Student details entered successfully.\n");
            return;
        }
        else {
            printf("Incorrect format. Please try again.\n");
        }
    } while (1);
}

void average() {
    char avgInput[100];
    int level_num;
    int course_num;

    do {
        printf("Enter the level num and the course num or type 'e' to exit.\n");
        printf("Format: Level: 1-12 Course: 1-10\n");
        printf("Example: 9 9\n");

        fgets(avgInput, sizeof(avgInput), stdin);

        // Exit the function if the user enters 'e'
        if (avgInput[0] == 'e') {
            return;
        }

        int num = sscanf(avgInput, "%d %d", &level_num, &course_num);
        if (num == 2) {
            int level_avg = 0; // Initialize level_avg
            int student_counter = 0; // Initialize student_counter

            for (int i = 0; i < NUM_CLASSES; i++) {
                struct Student* student = s.DB[level_num - 1][i];
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

            return;
        }
        else {
            printf("Incorrect input. Please try again.\n");
        }
    } while (1);
}

void topOutstanding() {
    int level, course;

    do {
        printf("Enter the level and course number or 'e' to exit:\n");
        printf("Format: Level: 1-12 Course: 1-10\n");
        printf("Example: 9 9\n");
        char input[100];
        fgets(input, sizeof(input), stdin);

        // Exit the function if the user enters 'e'
        if (input[0] == 'e') {
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

    struct Tops* t = &s.course[level-1][course-1];

    printf("Top students for Level %d, Course %d:\n", level, course);
    for (int i = 0; i < NUM_TOP; i++) {
        struct Student* topStudent = t->top[i];
        if (topStudent == NULL) {
            printf("No student in position %d\n", i + 1);
        }
        else {
            printf(" %d) %s %s - %d \n", i + 1, topStudent->fname, topStudent->lname, topStudent->grades[course-1]);
        }
    }
}


void topFailed() {

}