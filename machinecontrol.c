#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"

// The machine struct
struct machine {
    char name[NAME_MAX_LENGTH];
    int index, pin, status;
    char location[LOCATION_MAX_LENGTH];
};


// Function definitions
int addMachine(char name[NAME_MAX_LENGTH], int pin, char location[LOCATION_MAX_LENGTH]);
int deleteMachine(int index);
struct machine* getMachine(int index);
struct machine* getAllMachines();
int initMachineControl();
int countLoadedMachines();
int saveAllMachines();
int saveMachine(struct machine mach);
int loadMachines();
int getNextFreeIndex();
int resetInMemoryMachines();
int resetMachine(struct machine* mach);
int updateMachineStatus(int index, int status);
int updateMachineName(int index, char name[NAME_MAX_LENGTH]);
int updateMachineLocation(int index, char location[LOCATION_MAX_LENGTH]);
int updateMachinePin(int index, int pin);
int isValidPin(int pin);
int setPinStatus(int pin, int status);
int setMachineStatuses();
void turnMachinesOff();

// All the machines are loaded into memory when the
// application is started.
struct machine machines[MAX_MACHINES];

/**
 * Carries out all initial tasks required to set up
 * the machine control.
 */ 
int initMachineControl() {
    printf("Initialising machine control...\n");
    loadMachines();
    setMachineStatuses();
    printf("Machine control initialised and loaded %d machines\n", countLoadedMachines());
}

/**
 * Turns all the machines off. Designed to run on program exit.
 */ 
void turnMachinesOff() {
    printf("Turning machines off....\n");
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (isValidPin(machines[i].pin)) {
            setPinStatus(machines[i].pin, 0);
        }
    }
}

/**
 * Initially sets the status of all machines.
 */ 
int setMachineStatuses() {
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (isValidPin(machines[i].pin)) {
            setPinStatus(machines[i].pin, machines[i].status);
        } else {
            printf("WARNING: Machine index %d has an invalid GPIO pin!\n", machines[i].index);
        }
    }

    return 1;
}

/**
 * Adds a machine to the data file.
 */
int addMachine(char name[NAME_MAX_LENGTH], int pin, char location[LOCATION_MAX_LENGTH]) {
    struct machine mach;

    printf("Name is now %s and %s", name, location);

    strncpy(mach.name, name, NAME_MAX_LENGTH);
    strncpy(mach.location, location, LOCATION_MAX_LENGTH);
    mach.pin = pin;
    mach.status = 0;
    mach.index = getNextFreeIndex();

    saveMachine(mach);
    loadMachines();

    return 1;
}

/**
 * Deletes a machine from the data file.
 */
int deleteMachine(int index) {
    for (int i = 0; i < MAX_MACHINES; i++) {
        printf("ID: %d Name: %s\n", machines[i].index, machines[i].name);
    }
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index == index) {
            resetMachine(&machines[i]);

            saveAllMachines();
            return 1;
        }
    }

    return 0;
}

/**
 * Checks if a pin is a valid GPIO pin
 */
int isValidPin(int pin) {
    int validPins[28] = {3, 5, 7, 11, 13, 15, 19, 21, 23, 27, 29, 31, 33, 35, 37, 8, 10, 12, 16, 18,
        22, 24, 26, 28, 32, 36, 38, 40};

    for (int i = 0; i < 28; i++) {
        if (validPins[i] == pin) {
            return 1;
        }
    }

    return 0;
}

/**
 * Gets a machine by the index.
 */
struct machine* getMachine(int index) {
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index == index) {
            return &machines[i];
        }
    }

    return NULL;
}

/**
 * Gets all the machines.
 */
struct machine* getAllMachines() {
    return machines;
}

/**
 * Gets the next available index.
 */ 
int getNextFreeIndex() {
    int index = -1;
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index >= index) {
            index = machines[i].index + 1;
        }
    }

    // If index is less than 1 then there were no records found so index 1 can be used
    return index < 1 ? 1 : index;
}

/**
 * Resets all the values in the machines array.
 */ 
int resetInMemoryMachines() {
    for (int i = 0; i < MAX_MACHINES; i++) {
        resetMachine(&machines[i]);
    }

    return 1;
}

/**
 * Resets all the values in the machine to 0.
 */ 
int resetMachine(struct machine* machine) {
    machine->index = 0;
    strcpy(machine->name, "0");
    strcpy(machine->location, "0");
    machine->status = 0;
    machine->pin = 0;

    return 1;
}

/**
 * Loads all the machines from the data file into memory
 * in the machines global var.
 */ 
int loadMachines() {
    FILE *file;

    // Reset the machine array
    resetInMemoryMachines();    

    // Attempt to open the machines file stream
    file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "\nError opening data file\n");
        return 0;
    }

    // Read the entries into the machines global
    fread(machines, sizeof(struct machine), MAX_MACHINES, file);

    // Close the stream
    fclose(file);    

    return 1;
}

/**
 * Saves a single machine to the data file.
 */ 
int saveMachine(struct machine mach) {
    FILE *file;

    // Try to open the file stream
    file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        fprintf(stderr, "\nError opening data file\n");
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

/**
 * Updates a machine's status.
 */ 
int updateMachineStatus(int index, int status) {
    struct machine* mach;

    if (status > 1 || status < 0) {
        return 0;
    }

    mach = getMachine(index);
    if (mach == NULL) {
        return 0;
    }

    mach->status = status;

    setPinStatus(mach->pin, mach->status);

    saveAllMachines();

    return 1;
}

/**
 * Sets the status of a GPIO pin.
 */ 
int setPinStatus(int pin, int status) {
    if (!isValidPin(pin) || status > 1 || status < 0) return 0;

    char cmd[100];
    sprintf(cmd, "sudo gpio -g mode %d out", pin);
    system(cmd);
    sprintf(cmd, "sudo gpio -g write %d %d", pin, status);
    system(cmd);

    return 1;
}

/**
 * Updates a machine's pin.
 */ 
int updateMachinePin(int index, int pin) {
    struct machine* mach;

    if (pin < 1 || pin > 40) {
        return 0;
    }

    mach = getMachine(index);
    if (mach == NULL) {
        return 0;
    }

    mach->pin = pin;

    saveAllMachines();

    return 1;
}

/**
 * Updates a machine's name.
 */ 
int updateMachineName(int index, char name[NAME_MAX_LENGTH]) {
    struct machine* mach;

    mach = getMachine(index);
    if (mach == NULL) {
        return 0;
    }

    strncpy(mach->name, name, NAME_MAX_LENGTH);

    saveAllMachines();

    return 1;
}

/**
 * Updates a machine's location.
 */ 
int updateMachineLocation(int index, char location[LOCATION_MAX_LENGTH]) {
    struct machine* mach;

    mach = getMachine(index);
    if (mach == NULL) {
        return 0;
    }

    strncpy(mach->location, location, LOCATION_MAX_LENGTH);

    saveAllMachines();

    return 1;
}

/**
 * Saves all the machines that are loaded in memory in the
 * data file.
 */ 
int saveAllMachines() {
    FILE *file;

    file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        fprintf(stderr, "\nError opening data file\n");
        return 0;
    }

    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index < 1) continue;

        fwrite(&machines[i], sizeof(machines[i]), 1, file);
    }

    fclose(file);

    // Refresh the machines array
    loadMachines();

    return 1;
}

/**
 * Counts all the machines that are loaded into memory.
 */ 
int countLoadedMachines() {
    int count = 0;
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].index > 0) count++;
    }

    return count;
}