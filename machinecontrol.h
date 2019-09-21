int addMachine(char name[], int pin, char location[]);
int deleteMachine(int index);
struct machine* getMachine(int index);
struct machine* getAllMachines();
int initMachineControl();
int countLoadedMachines();
int saveMachines();