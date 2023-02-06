using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor MotorDriveL1;
extern motor MotorDriveL2;
extern motor MotorDriveR1;
extern motor MotorDriveR2;
extern motor MotorIntake;
extern motor Flywheel2;
extern motor MotorIndexer;
extern motor MotorEog;
extern limit LimitSwitchA;
extern optical Optical3;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );