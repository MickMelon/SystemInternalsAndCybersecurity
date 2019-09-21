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
void addMachine();
int getNextFreeIndex();

void displayMenu() {
    bool cont = true;
    while (cont) {
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

        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addMachine();
                break;

            case 2:
                showAllMachines();
                break;

            case 3:
                printf("Search by index clicked\n");
                break;

            case 4:
                printf("Delete machine clicked\n");
                break;

            case 5:
                printf("Update status clicked\n");
                break;

            case 9:
                printf("Shutting down program....\n");
                exit(1);

            default:
                printf("Invalid option selected.\n");
        }

        printf("Continue? [1/0] >>\n");
        scanf("%d", &choice);
        if (choice == 0) cont = false;
    }

    printf("Goodbye\n");

   // showAllMachines();

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

void addMachine() {
    struct machine mach;
    printf("Enter machine name >>\n");
    scanf("%s", &mach.name);

    printf("Enter machine location >>\n");
    scanf("%s", &mach.location);

    mach.pin = 0;
    mach.status = 0;
    mach.index = getNextFreeIndex();

    saveMachine(mach);

    printf("SUCCESS: Machine created with name %s and location %s (ID: %d)\n", mach.name, mach.location, mach.index);
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

int getNextFreeIndex() {
    // Declare the variables
    FILE *file;
    struct machine mach;
    int index = -2;

    // Attempt to open the machines file stream
    file = fopen("machines.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening dat file\n");
        exit(1);
    }

    // Read the entries until one with the matching index is found
    while (fread(&mach, sizeof(struct machine), 1, file)) {
        if (mach.index > index) index = mach.index;
    }

    // Close the stream
    fclose(file);

    return index + 1;
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