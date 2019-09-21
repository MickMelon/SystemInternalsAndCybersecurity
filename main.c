#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

struct machine {
    char name[8];
    int index, pin, status;
    char location[16];
};

void displayMenu();
int saveMachine(struct machine mach);
struct machine loadMachine(int id);
void showAllMachines();

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

    showAllMachines();

    /*struct machine mach;

    strcpy(mach.name, "Second Machine");
    strcpy(mach.location, "Mars");
    mach.index = 23;
    mach.pin = 3;
    mach.status = 404;

    saveMachine(mach);

    struct machine test = loadMachine(23);

    if (test.index == -1) {
        printf("fucked\n");
    } else {
        printf("Machine is %s\n", test.location);
    }*/
}

int saveMachine(struct machine mach) {
    FILE *file;

    file = fopen("machines.dat", "a");
    if (file == NULL) {
        fprintf(stderr, "\nError opening machines.dat file\n");
        exit(1);
    }

    fwrite(&mach, sizeof(mach), 1, file);

    if (fwrite != 0) {
        printf("Machine saved successfully!\n");
    } else {
        printf("Failed to save machine!\n");
    }

    fclose(file);

    return 0;
}

void showAllMachines() {
    // Declare the variables
    FILE *file;
    struct machine mach;

    // Attempt to open the machines file stream
    file = fopen("machines.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening dat file\n");
        exit(1);
    }

    // Read the entries until one with the matching index is found
    printf("Index   Name    Pin     Status      Location\n");
    while (fread(&mach, sizeof(struct machine), 1, file)) {
        printf("%d      %s      %d      %d      %s\n", mach.index, mach.name, mach.pin, mach.status, mach.location);
    }

    // Close the stream
    fclose(file);
}

struct machine loadMachine(int id) {
    // Declare the variables
    FILE *file;
    struct machine mach;
    bool exists = false;

    // Attempt to open the machines file stream
    file = fopen("machines.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening %d.dat file\n", id);
        exit(1);
    }

    // Read the entries until one with the matching index is found
    while (fread(&mach, sizeof(struct machine), 1, file)) {
        if (mach.index == id) {
            exists = true;
            break;
        }
    }

    // Close the stream
    fclose(file);

    if (!exists) {
        printf("The machine could not be found with index %d\n", id);
        mach.index = -1;
    } else {
        printf("id = %d location = %s\n", mach.index, mach.location);  
    }    
      
    return mach;
}

int main() {
    displayMenu();
}