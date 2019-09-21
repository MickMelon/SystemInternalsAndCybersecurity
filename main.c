#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#include "machinecontrol.h"

#define MAX_MACHINES 5

struct machine {
    char name[8];
    int index, pin, status;
    char location[16];
};

void displayMenu();
void selectCreateMachine();
void selectShowAllMachines();
void selectSearchByIndex();
void selectDeleteMachine();

struct machine machines[MAX_MACHINES];

/**
 * Displays the menu and handles user selection.
 */ 
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
                selectCreateMachine();
                break;

            case 2:
                selectShowAllMachines();
                break;

            case 3:
                selectSearchByIndex();
                break;

            case 4:
                selectDeleteMachine();
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

/**
 * Called when the user selects the "Search by Index" menu option.
 */ 
void selectSearchByIndex() {
    struct machine *mach;
    int choice;

    printf("Enter the index to search for >>\n");    
    scanf("%d", &choice);

    if (choice < 1 || choice > MAX_MACHINES) {
        printf("ERROR: Invalid index entered.\n");
        return;
    }

    mach = getMachine(choice);
    if (mach == NULL) {
        printf("No machine found.\n");
        return;
    }

    printf("Machine found!\nName: %s \nLocation: %s\nIndex: %d\nPin: %d\nStatus: %d\n",
        mach->name, mach->location, mach->index, mach->pin, mach->status);
}

/**
 * Called when the user selects the "Delete Machine" menu option.
 */ 
void selectDeleteMachine() {
    int choice;

    if (countLoadedMachines() < 1) {
        printf("ERROR: There are no machines to delete.\n");
        return;
    }
    
    printf("Enter machine index >>\n");
    scanf("%d", &choice);

    if (deleteMachine(choice) == 0) {
        printf("ERROR: No machine could be found with that index.\n");
        return;
    }

    printf("SUCCESS: Machine deleted.\n");
    saveMachines();
}

/**
 * Called when the user selects the "Create Machine" option.
 */ 
void selectCreateMachine() {
    // Make sure there is space for another machine
    if (countLoadedMachines() >= MAX_MACHINES) {
        printf("ERROR: You cannot create a machine because the max limit of %d has been reached.\n",
            MAX_MACHINES);
        return;
    }

    char name[8];
    char location[16];
    int index, pin;

    printf("Enter machine name >>\n");
    scanf("%s", &name);

    printf("Enter machine location >>\n");
    scanf("%s", &location);

    printf("Enter machine PIN >>\n");
    scanf("%d", &pin);

    if (addMachine(name, pin, location) == 0) {
        printf("ERROR: Failed to add machine.\n");
        return;
    }

    printf("SUCCESS: Machine was created.\n");
}

/**
 * Called when the user selects the "Show All Machines" menu option.
 */ 
void selectShowAllMachines() {
    // Read the entries until one with the matching index is found
    printf("Index       Name        Pin         Status          Location\n");
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index < 1) continue;

        printf("%d          %s          %d          %d          %s\n", machines[i].index,
            machines[i].name, machines[i].pin, machines[i].status, machines[i].location);
    }
}

/**
 * Called when the program is run.
 */ 
int main() {
    initMachineControl();
    displayMenu();
}