#define MAX_MACHINES 5

// The machine struct
struct machine {
    char name[8];
    int index, pin, status;
    char location[16];
};

// Function definitions
int addMachine(char* name[], int pin, char* location[]);
int deleteMachine(int index);
struct machine* getMachine(int index);
struct machine* getAllMachines();
int initMachineControl();
int countLoadedMachines();