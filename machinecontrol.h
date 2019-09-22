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