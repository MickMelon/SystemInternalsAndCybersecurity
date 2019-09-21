#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct machine {
    char name[8];
    int index, pin, status;
    char location[16];
};

void displayMenu();
int saveMachine(struct machine mach);
struct machine loadMachine(int id);

void displayMenu() {
    printf("*************************************************\n");
    printf("* Vending Machine Control Console               *\n");
    printf("*                Submitted by: Michael McMillan *\n");
    printf("*                Student ID: 1800833            *\n");
    printf("*                Abertay University             *\n");
    printf("*************************************************\n");
    printf("1. Add Machine\n");
    printf("2. Show All Machines\n");
    printf("3. Search By Index\n");
    printf("4. Delete Machine\n");
    printf("5. Update Status\n");
    printf("9. Exit\n");
    printf("*************************************************\n");
    printf("Enter your choice >>\n");

    struct machine mach;

    strcpy(mach.name, "First Machine");
    strcpy(mach.location, "Earth");
    mach.index = 1;
    mach.pin = 3;
    mach.status = 404;

    saveMachine(mach);
    printf("t");
    struct machine test = loadMachine(mach.index);

    printf("Machine is %s\n", test.location);
}

int saveMachine(struct machine mach) {
    FILE *file;
    char machFileName[20];
    sprintf(machFileName, "machines/%d.dat", mach.index);

    file = fopen(machFileName, "w");
    if (file == NULL) {
        fprintf(stderr, "\nError opening %d.dat file\n", mach.index);
        exit(1);
    }

    fwrite(&mach, sizeof(mach), 1, file);

    if (fwrite != 0) {
        printf("Machine saved successfully!\n");
    } else {
        printf("Failed to save machine!\n");
    }

    fclose(file);
printf("t");
    return 0;
}

struct machine loadMachine(int id) {
    FILE *file;
    struct machine mach;

    char machFileName[20];
    sprintf(machFileName, "machines/%d.dat", id);

    printf("opening file..");
    file = fopen(machFileName, "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening %d.dat file\n", id);
        exit(1);
    }

    printf("reading....");
    while (fread(&mach, sizeof(struct machine), 1, file)) {
        printf("id = %d location = %s\n", mach.index, mach.location);
    }

    fclose(file);

    return mach;
}

int main() {
    displayMenu();
}