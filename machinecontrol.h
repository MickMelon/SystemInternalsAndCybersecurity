// The machine struct
struct machine {
    char name[NAME_MAX_LENGTH];
    int index, pin, status;
    char location[LOCATION_MAX_LENGTH];
};

// Function definitions
int addMachine(char name[], int pin, char location[]);
int deleteMachine(int index);
struct machine* getMachine(int index);
struct machine* getAllMachines();
int initMachineControl();
int countLoadedMachines();
int updateMachineStatus(int index, int status);
int updateMachineName(int index, char name[NAME_MAX_LENGTH]);
int updateMachineLocation(int index, char location[LOCATION_MAX_LENGTH]);