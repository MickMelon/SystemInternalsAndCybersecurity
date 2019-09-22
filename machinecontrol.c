#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"

// The machine struct
struct machine {
    char name[16];
    int index, pin, status;
    char location[32];
};


// Function definitions
int addMachine(char name[], int pin, char location[]);
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

// All the machines are loaded into memory when the
// application is started.
struct machine machines[MAX_MACHINES];

/**
 * Carries out all initial tasks required to set up
 * the machine control.
 */ 
int initMachineControl() {
    printf("Initialising machine control...");
    loadMachines();
    printf("Machine control initialised and loaded %d machines\n", countLoadedMachines());
}

/**
 * Adds a machine to the data file.
 */
int addMachine(char name[], int pin, char location[]) {
    struct machine mach;

    printf("Name is now %s and %s", name, location);

    strcpy(mach.name, name);
    strcpy(mach.location, location);
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