#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_APPLICANTS 100
#define MAX_DAYS 3

struct Applicant {
    char name[50];
    int days[MAX_DAYS];
};

struct Applicant applicants[MAX_APPLICANTS];
int num_applicants = 0;

int is_day_full(int day) {
    int count = 0;
    for (int i = 0; i < num_applicants; i++) {
        if (applicants[i].days[day] == 1) {
            count++;
        }
    }
    return count >= 2;
}

void add_applicant() {
    if (num_applicants >= MAX_APPLICANTS) {
        printf("Maximum number of applicants reached.\n");
        return;
    }

    struct Applicant new_applicant;
    printf("Enter applicant name: ");
    scanf("%s", new_applicant.name);

    printf("Enter days available (Mondays=0, Wednesdays=1, Thursdays=2): ");
    char days_str[20];
    scanf("%s", days_str);
    char *day_token = strtok(days_str, " ");
    while (day_token != NULL) {
        int day = atoi(day_token);
        if (is_day_full(day)) {
            printf("Sorry, that day is already full.\n");
            return;
        }
        new_applicant.days[day] = 1;
        day_token = strtok(NULL, " ");
    }

    applicants[num_applicants++] = new_applicant;
    printf("Applicant added successfully.\n");
}

void list_applicants() {
    printf("List of applicants:\n");
    for (int i = 0; i < num_applicants; i++) {
        printf("%s: ", applicants[i].name);
        for (int j = 0; j < MAX_DAYS; j++) {
            if (applicants[i].days[j] == 1) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }
}

void delete_applicant() {
    char name[50];
    printf("Enter name of applicant to delete: ");
    scanf("%s", name);

    int found = 0;
    for (int i = 0; i < num_applicants; i++) {
        if (strcmp(applicants[i].name, name) == 0) {
            found = 1;
            for (int j = i; j < num_applicants - 1; j++) {
                applicants[j] = applicants[j+1];
            }
            num_applicants--;
            printf("Applicant deleted successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Applicant not found.\n");
    }
}

void modify_applicant() {
    char name[50];
    printf("Enter name of applicant to modify: ");
    scanf("%s", name);

    int found = 0;
    for (int i = 0; i < num_applicants; i++) {
        if (strcmp(applicants[i].name, name) == 0) {
            found = 1;
            printf("Enter new days available (Mondays=0, Wednesdays=1, Thursdays=2): ");
            char days_str[20];
            scanf("%s", days_str);
            memset(applicants[i].days, 0, sizeof(applicants[i].days));
            char *day_token = strtok(days_str, " ");
            while (day_token != NULL) {
                int day = atoi(day_token);
                if (is_day_full(day)) {
                    printf("Sorry, that day is already full.\n");
                    return;
                }
                applicants[i].days[day] = 1;
                day_token = strtok(NULL, " ");
            }
            printf("Applicant modified successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Applicant not found.\n");
    }
}

int main() {
    int choice;
    do {
        printf("\n");
        printf("1. Add applicant\n");
        printf("2. List applicants\n");
        printf("3. Delete applicant\n");
        printf("4. Modify applicant\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_applicant();
                break;
            case 2:
                list_applicants();
                break;
            case 3:
                delete_applicant();
                break;
            case 4:
                modify_applicant();
                break;
            case 5:
                printf("Exiting program.\n");
                break;
            default:
            printf("Invalid choice.\n"); break; } } while (choice != 5);

return 0;
}