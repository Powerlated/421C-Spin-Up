#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor MotorDriveL1 = motor(PORT19, ratio18_1, false);
motor MotorDriveL2 = motor(PORT20, ratio18_1, false);
motor MotorDriveR1 = motor(PORT14, ratio18_1, true);
motor MotorDriveR2 = motor(PORT15, ratio18_1, true);
motor MotorIntake = motor(PORT9, ratio18_1, false);
motor Flywheel2 = motor(PORT1, ratio6_1, true);
motor MotorIndexer = motor(PORT10, ratio18_1, false);
motor MotorEog = motor(PORT5, ratio18_1, false);
limit LimitSwitchA = limit(Brain.ThreeWirePort.A);
optical Optical3 = optical(PORT3);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}