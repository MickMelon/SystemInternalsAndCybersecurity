#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "machinecontrol.h"

// Function definitions
void displayMenu();
void selectCreateMachine();
void selectShowAllMachines();
void selectSearchByIndex();
void selectDeleteMachine();

char* safeStringInput(char* input, int length);
int safeIntInput(int *input);
void clearBuffer();

void clearBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

char* safeStringInput(char* input, int length) {
    

    if (fgets(input, length, stdin)) {
        input[strcspn(input, "\n")] = 0;
        return input;
    }

    return NULL;
}

int safeIntInput(int* input) {
    long longInput;
    char buf[1024];

    if (!fgets(buf, 1024, stdin)) {
        printf("ERROR: Did not receive any input.\n");
        return 0;
    }

    char *endPtr;
    errno = 0;
    longInput = strtol(buf, &endPtr, 10);

    // Check if out of range
    if (errno == ERANGE) {
        printf("ERROR: The number you entered was too large or small.\n");
        clearBuffer();
        return 0;
    } else if (endPtr == buf) {
        // No character was read
        printf("ERROR: No character was read.\n");
        clearBuffer();
        return 0;
    } else if (*endPtr && *endPtr != '\n') {
        printf("ERROR: End of line.\n");
        clearBuffer();
        return 0;
    } else if (longInput > INT_MAX || longInput < INT_MIN) {
        printf("ERROR: Number is not within acceptable int range.\n");
        clearBuffer();
        return 0;
    }

    *input = (int) longInput;
    return 1;
}

/**
 * Displays the menu and handles user selection.
 */ 
void displayMenu() {
    while (1) {
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
        

        int input;
        do {
            printf("Enter your choice >>\n");
        } while (safeIntInput(&input) == 0);

        switch (input) {
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

        printf("Continue? (0 to exit) >>\n");
        safeIntInput(&input);
        if (input == 0) {
            printf("Goodbye");
            exit(1);
        }
    }
}

/**
 * Called when the user selects the "Search by Index" menu option.
 */ 
void selectSearchByIndex() {
    struct machine *mach;
    int choice;

    // Request user input
    printf("Enter the index to search for >>\n");    
    safeIntInput(&choice);

    // Check that the input was valid
    if (choice < 1) {
        printf("ERROR: Invalid index entered.\n");
        return;
    }

    // Try to receive the machine
    mach = getMachine(choice);
    if (mach == NULL) {
        printf("No machine found.\n");
        return;
    }

    // If the machine was found, display it
    printf("Machine found!\nName: %s \nLocation: %s\nIndex: %d\nPin: %d\nStatus: %d\n",
        mach->name, mach->location, mach->index, mach->pin, mach->status);
}

/**
 * Called when the user selects the "Delete Machine" menu option.
 */ 
void selectDeleteMachine() {
    // Check if there are any machines
    if (countLoadedMachines() < 1) {
        printf("ERROR: There are no machines to delete.\n");
        return;
    }
    
    // Request user input
    printf("Enter machine index >>\n");
    int input;
    safeIntInput(&input);

    if (input < 1 || input > 100) {
        printf("ERROR: Machine index must be between 1 and 100.\n");
        return;
    } 
    

    // Attempt to delete the machine
    if (deleteMachine(input) == 0) {
        printf("ERROR: No machine could be found with that index.\n");
        return;
    }

    // Machine deleted so display success message
    printf("SUCCESS: Machine deleted.\n");
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

    char name[16];
    char location[32];
    int pin;

    // Request machine name
    do {
        printf("Enter machine name >>\n");        
    } while (safeStringInput(name, 16) == NULL || name[0] == '\0');    
    printf("Entered %s ref is %s\n", name, &name);

    // Request machine location
    do {
        printf("Enter machine location >>\n");
    } while (safeStringInput(location, 32) == NULL || location[0] == '\0');
    printf("Entered %s\n", location);

    // Request machine pin
    do {
        printf("Enter machine PIN >>\n");
    } while (safeIntInput(&pin) == 0 || pin < 0 || pin > 100);
    printf("Entered %d\n", pin);    

    // Attempt to add the machine
    if (addMachine(name, pin, location) == 0) {
        printf("ERROR: Failed to add machine.\n");
        return;
    }

    // Machine was added so show success message
    printf("SUCCESS: Machine was created.\n");
}

/**
 * Called when the user selects the "Show All Machines" menu option.
 */ 
void selectShowAllMachines() {
    struct machine* machines;
    machines = getAllMachines();

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
   // scanf("%*[^\n]%*c");
    initMachineControl();
    displayMenu();
}