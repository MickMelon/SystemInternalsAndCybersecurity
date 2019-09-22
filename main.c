#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "defines.h"
#include "machinecontrol.h"

// Function definitions
void displayMenu();
void selectCreateMachine();
void selectShowAllMachines();
void selectSearchByIndex();
void selectDeleteMachine();
void selectUpdateStatus();
void selectUpdateName();
void selectUpdateLocation();
void selectUpdatePin();
char* safeStringInput(char* input, int length);
int safeIntInput(int *input);

/**
 * Gets an input from the user safely. This will not allow
 * the user to enter a string that is too long.
 */ 
char* safeStringInput(char* input, int length) {
    if (fgets(input, length, stdin)) {
        printf("The input is %s", input);
        char* newline = strchr(input, '\n');

        if (!newline) {
            printf("ERROR: Input is too long\n");

            // Clear the buffer
            while (!newline && fgets(input, length, stdin)) {
                newline = strchr(input, '\n');
            }

            return NULL;
        }

        input[strcspn(input, "\n")] = 0;
        return input;
    }

    return NULL;
}

/**
 * Safely gets an int from the user input.
 */ 
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
        return 0;
    } else if (endPtr == buf) {
        // No character was read
        printf("ERROR: No character was read.\n");
        return 0;
    } else if (*endPtr && *endPtr != '\n') {
        printf("ERROR: End of line.\n");
        return 0;
    } else if (longInput > INT_MAX || longInput < INT_MIN) {
        printf("ERROR: Number is not within acceptable int range.\n");
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
        printf("6. Update Name\n");
        printf("7. Update Location\n");
        printf("8. Update Pin\n");
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
                selectUpdateStatus();
                break;

            case 6:
                selectUpdateName();
                break;

            case 7:
                selectUpdateLocation();
                break;

            case 8:
                selectUpdatePin();
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

    char name[NAME_MAX_LENGTH];
    char location[LOCATION_MAX_LENGTH];
    int pin;

    // Request machine name
    do {
        printf("Enter machine name (max %d chars) >>\n", NAME_MAX_LENGTH);
    } while (safeStringInput(name, NAME_MAX_LENGTH) == NULL || name[0] == '\0');    
    printf("Entered %s ref is %s\n", name, &name);

    // Request machine location
    do {
        printf("Enter machine location (max %d chars) >>\n", LOCATION_MAX_LENGTH);
    } while (safeStringInput(location, LOCATION_MAX_LENGTH) == NULL || location[0] == '\0');
    printf("Entered %s\n", location);

    // Request machine pin
    do {
        printf("Enter machine PIN (must be 1-40) >>\n");
    } while (safeIntInput(&pin) == 0 || (pin < 1 || pin > 40));
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
 * Called when the user selects the "Update Status" menu option.
 */ 
void selectUpdateStatus() {
    int index, status;

    // Request index
    do {
        printf("Enter the index to update status for >>\n");    
    } while (safeIntInput(&index) == 0);

    // Check if the machine exists
    if (getMachine(index) == NULL) {
        printf("ERROR: No machine was found with that index.\n");
        return;
    }

    // Request status
    do {
        printf("Enter the status (1 or 0) >>\n"); 
    } while (safeIntInput(&status) == 0 || (status < 0 || status > 1));

    // Attempt to update machine status
    if (updateMachineStatus(index, status) == 0) {
        printf("ERROR: Could not update machine status.\n");
        return;
    }

    printf("SUCCESS: Machine status was updated.\n");
}

/**
 * Called when the user selects the "Update Name" menu option.
 */ 
void selectUpdateName() {
    int index;
    char name[NAME_MAX_LENGTH];
    struct machine* mach;

    // Request index
    do {
        printf("Enter the index to update name for >>\n");    
    } while (safeIntInput(&index) == 0);

    // Check if the machine exists
    mach = getMachine(index);
    if (mach == NULL) {
        printf("ERROR: No machine was found with that index.\n");
        return;
    }

    // Request new name
    do {
        printf("Enter a new name for the machine (current: '%s') >>\n", mach->name);
    } while (safeStringInput(name, NAME_MAX_LENGTH) == NULL || name == '\0');

    // Attempt to update the name
    if (updateMachineName(index, name) == 0) {
        printf("ERROR: Unable to update the machine name.\n");
        return;
    }

    printf("SUCCESS: Machine name updated.\n");
}

/**
 * Called when the user selects the "Update Location" menu option.
 */
void selectUpdateLocation() {
    int index;
    char location[LOCATION_MAX_LENGTH];
    struct machine* mach;

    // Request index
    do {
        printf("Enter the index to update location for >>\n");    
    } while (safeIntInput(&index) == 0);

    // Check if the machine exists
    mach = getMachine(index);
    if (mach == NULL) {
        printf("ERROR: No machine was found with that index.\n");
        return;
    }

    // Request new location
    do {
        printf("Enter a new location for the machine (current: '%s') >>\n", mach->location);
    } while (safeStringInput(location, LOCATION_MAX_LENGTH) == NULL || location == '\0');

    // Attempt to update the location
    if (updateMachineLocation(index, location) == 0) {
        printf("ERROR: Unable to update the machine location.\n");
        return;
    }

    printf("SUCCESS: Machine location updated.\n");
}

void selectUpdatePin() {
    int index, pin;
    struct machine* mach;

    // Request index
    do {
        printf("Enter the index to update pin for >>\n");    
    } while (safeIntInput(&index) == 0);

    // Check if the machine exists
    mach = getMachine(index);
    if (mach == NULL) {
        printf("ERROR: No machine was found with that index.\n");
        return;
    }

    // Request pin
    do {
        printf("Enter the pin (must be 1-40) (current: %d) >>\n", mach->pin); 
    } while (safeIntInput(&pin) == 0 || (pin < 1 || pin > 40));

    // Attempt to update machine pin
    if (updateMachinePin(index, pin) == 0) {
        printf("ERROR: Could not update machine pin.\n");
        return;
    }

    printf("SUCCESS: Machine pin was updated.\n");
}

/**
 * Called when the program is run.
 */ 
int main() {
    initMachineControl();
    displayMenu();
}