#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LEVELS 12
#define NUM_CLASSES 10
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

struct School {
    struct Student* DB[NUM_LEVELS][NUM_CLASSES];
};

static struct School s;

void INITDB();
struct Student* createStudent(char* fname, char* lname, char* phone, int level, int class_num, int* grades);
void menu();
void insertStudent();
void average();
void topOutstanding();
void topFailed();

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

    while (fscanf(iptr, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        fname, lname, phone, &level, &class_num,
        &grades[0], &grades[1], &grades[2], &grades[3], &grades[4],
        &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]) == 17) {
        struct Student* newStudent = createStudent(fname, lname, phone, level, class_num, grades);
        s.DB[level][class_num] = newStudent;
    }

    fclose(iptr);
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
        printf("Format: Firstname Lastname Phone Level Class_number Grades\n");
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
            struct Student* newStudent = createStudent(fname, lname, phone, level, class_num, grades);
            // addStudentToDB(newStudent);  // This function should be implemented according to your database setup

            printf("Student details entered successfully.\n");
            return;
        }
        else {
            printf("Incorrect format. Please try again.\n");
        }
    } while (1);
}

void average() {

}

void topOutstanding() {

}

void topFailed() {

}