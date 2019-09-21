#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define MAX_MACHINES 5

struct machine {
    char name[8];
    int index, pin, status;
    char location[16];
};

void displayMenu();
void createMachine();
int saveMachine(struct machine mach);
struct machine* getMachine(int index);
void showAllMachines();
int getNextFreeIndex();
void loadMachines();
int countLoadedMachines();
void searchByIndex();

struct machine machines[MAX_MACHINES];

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
                createMachine();
                break;

            case 2:
                showAllMachines();
                break;

            case 3:
                searchByIndex();
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
}

void searchByIndex() {
    printf("Enter the index to search for >>\n");
    int choice;
    scanf("%d", &choice);

    if (choice < 1 || choice > MAX_MACHINES) {
        printf("ERROR: Invalid index entered.\n");
        return;
    }

    struct machine *mach = getMachine(choice);

    if (mach == NULL) {
        printf("No machine found.\n");
        return;
    }

    printf("Machine found!\nName: %s \nLocation: %s\nIndex: %d\nPin: %d\nStatus: %d\n",
        mach->name, mach->location, mach->index, mach->pin, mach->status);
}

void createMachine() {
    // Make sure there is space for another machine
    if (countLoadedMachines() >= MAX_MACHINES) {
        printf("ERROR: You cannot create a machine because the max limit of %d has been reached.\n", MAX_MACHINES);
        return;
    }

    struct machine mach;
    printf("Enter machine name >>\n");
    scanf("%s", &mach.name);

    printf("Enter machine location >>\n");
    scanf("%s", &mach.location);

    mach.pin = 0;
    mach.status = 0;
    mach.index = getNextFreeIndex();

    saveMachine(mach);
    loadMachines();

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
    int index = -1;

    // Attempt to open the machines file stream
    file = fopen("machines.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening dat file\n");
        exit(1);
    }

    // Read the entries until one with the matching index is found
    while (fread(&mach, sizeof(struct machine), 1, file)) {
        if (mach.index >= index) index = mach.index + 1;
    }

    // Close the stream
    fclose(file);

    // If index is -1 then there were no records found so index 1 can be used
    return index == -1 ? 1 : index;
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

struct machine* getMachine(int index) {
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index == index) {
            return &machines[i];
        }
    }

    return NULL;
}

void loadMachines() {
    // Declare the variables
    FILE *file;

    // Attempt to open the machines file stream
    file = fopen("machines.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening machines.dat file\n");
        exit(1);
    }

    // Read the entries until one with the matching index is found
    printf("reading...\n");
    size_t qty = fread(machines, sizeof(struct machine), MAX_MACHINES, file);
    if (qty == 0) {
        printf("No machines were loaded.");
    }

    // Close the stream
    fclose(file);
}

int countLoadedMachines() {
    int count = 0;
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index > 0) count++;
    }

    return count;
}

int main() {
    loadMachines();

    printf("There are %d machines\n", countLoadedMachines());

    displayMenu();
}