/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\zjia0                                            */
/*    Created:      Fri May 27 2022                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// L1 - Intake Disks
// L2 - Reverse Intake
// R1 - Toggle flywheel
// R2 - Fire

// A - drive backwards
// Y + Right - End of Game Mechanism

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller
// MotorDriveL1         motor         19
// MotorDriveL2         motor         20
// MotorDriveR1         motor         14
// MotorDriveR2         motor         15
// MotorIntake          motor         9
// Flywheel2            motor         1
// MotorIndexer         motor         10
// MotorEog             motor         5
// LimitSwitchA         limit         A
// Optical3             optical       3
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "math.h"
#include "vex.h"
#include <cmath>

using namespace vex;

const int chassisSpeed = 100;
const int intakeSpeed = 70;

bool weAreOnBlue = false;
bool flywheelRunning = false;
bool invControls = false;

competition Competition;

int intakeState;
typedef enum { OFF, INTAKE, REVERSE } IntakeState;

int flywheelPct = 100;
int state = 0;
const int STATE_MAX = 2;

void drawControllerInfo() {

  Controller1.Screen.clearScreen();

  Controller1.Screen.setCursor(1, 17);
  Controller1.Screen.print("<%d/%d>", state + 1, STATE_MAX + 1);

  Controller1.Screen.setCursor(1, 1);

  if (state == 0) {
    Controller1.Screen.print("Battery: %d%%", Brain.Battery.capacity());
    Controller1.Screen.newLine();

  } else if (state == 1) {
    Controller1.Screen.print("Drive Motors");

    Controller1.Screen.newLine();
    Controller1.Screen.print(
        "L1, L2: %d C°, %d C°",
        (int)MotorDriveL1.temperature(temperatureUnits::celsius),
        (int)MotorDriveL1.temperature(temperatureUnits::celsius));
    Controller1.Screen.newLine();
    Controller1.Screen.print(
        "R1, R2: %d C°, %d C°",
        (int)MotorDriveR1.temperature(temperatureUnits::celsius),
        (int)MotorDriveR2.temperature(temperatureUnits::celsius));
    Controller1.Screen.newLine();
  } else if (state == 2) {
    Controller1.Screen.print("Flywheel Motors");

    Controller1.Screen.newLine();
    Controller1.Screen.print(
        "%d C°", (int)Flywheel2.temperature(temperatureUnits::celsius));
    Controller1.Screen.newLine();

    Controller1.Screen.print("%d RPM",
                             (int)Flywheel2.velocity(velocityUnits::rpm));

    Controller1.Screen.print(" %d%", flywheelPct);
  }
}

void nextScreen() {
  if (++state > STATE_MAX) {
    state = 0;
  }
  drawControllerInfo();
}

void prevScreen() {
  if (--state < 0) {
    state = STATE_MAX;
  }
  drawControllerInfo();
}

bool buttonX;
bool buttonB;
void expand() {
  MotorEog.spinFor(directionType::fwd, 480, rotationUnits::deg, 100,
                   velocityUnits::pct);
  MotorEog.stop(brakeType::brake);
}

void updateFlywheel() {
  if (flywheelRunning) {
    Flywheel2.spin(directionType::rev, flywheelPct, percentUnits::pct);
  } else {
    Flywheel2.stop(brakeType::coast);
  }
}

void flywheelOn() {
  flywheelRunning = true;
  updateFlywheel();
}

void flywheelOff() {
  flywheelRunning = false;
  updateFlywheel();
}

void updateIntake() {
  switch (intakeState) {
  case REVERSE:
    MotorIntake.spin(directionType::fwd, intakeSpeed, percentUnits::pct);
    break;
  case INTAKE:
    MotorIntake.spin(directionType::rev, intakeSpeed, percentUnits::pct);
    break;
  case OFF:
    MotorIntake.stop(brakeType::coast);
    break;
  }
}

void intakeOn() {
  intakeState = INTAKE;
  updateIntake();
}

void intakeReverse() {
  intakeState = REVERSE;
  updateIntake();
}

void intakeOff() {
  intakeState = OFF;
  updateIntake();
}

void fire() { MotorIndexer.spin(directionType::rev, 100, percentUnits::pct); }

void drivercontrol() {
  printf("Starting driver control\r\n");

  drawControllerInfo();

  Controller1.ButtonLeft.pressed([]() -> void {
    prevScreen();
    printf("Prev screen\n");
  });
  Controller1.ButtonRight.pressed([]() -> void {
    nextScreen();
    printf("Next screen\n");
    vex::color c = Optical3.color();
    if (c == vex::color::red) {
      printf("Red\n");
    } else if (c == vex::color::blue) {
      printf("Blue\n");
    } else {
      printf("Neither red or blue\n");
    }
  });

  LimitSwitchA.pressed([]() -> void {
    MotorIndexer.spin(directionType::rev, 20, percentUnits::pct);
    MotorIndexer.stop(brakeType::brake);
  });

  Controller1.ButtonA.pressed([]() -> void { invControls = !invControls; });

  Controller1.ButtonR1.pressed([]() -> void {
    flywheelRunning = !flywheelRunning;
    updateFlywheel();
  });

  Controller1.ButtonR2.pressed([]() -> void { fire(); });

  Controller1.ButtonL2.pressed([]() -> void {
    if (intakeState != REVERSE) {
      intakeState = REVERSE;
    } else if (intakeState == REVERSE) {
      intakeState = OFF;
    }
    updateIntake();
  });

  Controller1.ButtonL1.pressed([]() -> void {
    if (intakeState != INTAKE) {
      intakeState = INTAKE;
    } else if (intakeState == INTAKE) {
      intakeState = OFF;
    }
    updateIntake();
  });

  Controller1.ButtonY.pressed([]() -> void { expand(); });

  Controller1.ButtonUp.pressed([]() -> void {
    flywheelPct += 10;
    if (flywheelPct > 100)
      flywheelPct = 100;
    updateFlywheel();
  });
  Controller1.ButtonDown.pressed([]() -> void {
    flywheelPct -= 10;
    if (flywheelPct < 50)
      flywheelPct = 50;
    updateFlywheel();
  });

  while (true) {
    // DRIVE CODE DISABLER
    // continue;

    auto pos3 = Controller1.Axis3.position();
    auto pos1 = Controller1.Axis1.position();
    if (invControls) {
      pos3 *= -1;
    }
    // auto pos2 = Controller1.Axis2.position&U();

    auto pctSteer = pos1 * 0.45;

    auto pctL = pos3 * (100 - std::abs(pctSteer)) / 100 + pctSteer;
    auto pctR = pos3 * (100 - std::abs(pctSteer)) / 100 + -pctSteer;

    if (std::abs(pctL) >= 5) {
      MotorDriveL1.spin(vex::directionType::fwd, pctL * (chassisSpeed / 100),
                        vex::velocityUnits::pct);
      MotorDriveL2.spin(vex::directionType::fwd, pctL * (chassisSpeed / 100),
                        vex::velocityUnits::pct);
    } else {
      MotorDriveL1.stop(vex::brakeType::coast);
      MotorDriveL2.stop(vex::brakeType::coast);
    }
    if (std::abs(pctR) >= 5) {
      MotorDriveR1.spin(vex::directionType::fwd, pctR * (chassisSpeed / 100),
                        vex::velocityUnits::pct);
      MotorDriveR2.spin(vex::directionType::fwd, pctR * (chassisSpeed / 100),
                        vex::velocityUnits::pct);
    } else {
      MotorDriveR1.stop(vex::brakeType::coast);
      MotorDriveR2.stop(vex::brakeType::coast);
    }

    if (intakeState == REVERSE) {
      vex::color c = Optical3.color();
      vex::color ourColor = weAreOnBlue ? color::blue : color::red;
      if (c == ourColor) {
        intakeState = OFF;
        updateIntake();
      }
    }
  }
}

void rollerUntil(vex::color c) {
  const int timeout = 3000;
  int startTime = vex::timer::system();
  MotorIntake.spin(directionType::fwd, 100, velocityUnits::pct);
  while (Optical3.color() != c) {
    if (vex::timer::system() > startTime + timeout) {
      break;
    }
  }
  MotorIntake.stop(brakeType::brake);
}

void roller(double degrees) {
  MotorIntake.spinFor(degrees, rotationUnits::deg, 100, velocityUnits::pct, true);
}

float wheelCircumference = 4 * M_PI; // inches
const float defaultAutonSpeed = 50;
const float driveGearRatio = 1/1;  

void drive(float inchesL, float inchesR, float speedL, float speedR) {
  MotorDriveL1.spinFor(directionType::fwd, inchesL / wheelCircumference * driveGearRatio, rotationUnits::rev, speedL, velocityUnits::pct, false);
  MotorDriveR1.spinFor(directionType::fwd, inchesR / wheelCircumference * driveGearRatio, rotationUnits::rev, speedR, velocityUnits::pct, true);
  MotorDriveL2.spinFor(directionType::fwd, inchesL / wheelCircumference * driveGearRatio, rotationUnits::rev, speedL, velocityUnits::pct, false);
  MotorDriveR2.spinFor(directionType::fwd, inchesR / wheelCircumference * driveGearRatio, rotationUnits::rev, speedR, velocityUnits::pct, true);
}
void drive(float inches, float speed) { drive(inches, inches, speed, speed); }
void drive(float inches) { drive(inches, inches, defaultAutonSpeed, defaultAutonSpeed); }

void turn(float units, float autonSpeed) {
  MotorDriveL1.spinFor(directionType::fwd, units / wheelCircumference * driveGearRatio, rotationUnits::rev, autonSpeed, velocityUnits::pct, false);
  MotorDriveR1.spinFor(directionType::fwd, -units / wheelCircumference * driveGearRatio, rotationUnits::rev, autonSpeed, velocityUnits::pct, true);
  MotorDriveL2.spinFor(directionType::fwd, units / wheelCircumference * driveGearRatio, rotationUnits::rev, autonSpeed, velocityUnits::pct, false);
  MotorDriveR2.spinFor(directionType::fwd, -units / wheelCircumference * driveGearRatio, rotationUnits::rev, autonSpeed, velocityUnits::pct, true);
}
void turn(float units) { turn(units, defaultAutonSpeed); };

void autonomous() {
  LimitSwitchA.pressed([]() -> void {
    MotorIndexer.spin(directionType::rev, 20, percentUnits::pct);
    MotorIndexer.stop(brakeType::brake);
  });

  /* Brian's Auton API 
  *    
  *  drive(inchesL, inchesR, speedL, speedR)
  *  drive(inches, speed)
  *  drive(inches) - Uses default auton speed of 50
  *
  *  turn(units, autonSpeed) - Unitless, use trial of error to determine units
  *  turn(units) - Unitless, use trial of error to determine input. Uses default auton speed of 50
  * 
  *  intakeOn() - Turns on intake
  *  intakeReverse() - Reverses intake
  *  intakeOff() - Turns off intake
  *
  *  roller(degrees) - Spins the intake a certain amount of degrees (can be negative)
  */

  flywheelOn(); // Turns flywheel on
  intakeOn(); // Turns intake on

  drive(24); // Drives forward 1 tile (24 inches)
  turn(10); // Turns 10 (unitless) to the right
   
  fire(); // Shoots
  
  flywheelOff(); // Turns intake on
  intakeOff(); // Turns intake off
}

void redrawTeamSelMenu() {
  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("YOU ARE ON:");
  Controller1.Screen.setCursor(2, 1);
  if (!weAreOnBlue) {
    Controller1.Screen.print("-> RED");
  } else {
    Controller1.Screen.print("   RED");
  }
  Controller1.Screen.setCursor(3, 1);
  if (weAreOnBlue) {
    Controller1.Screen.print("-> BLUE");
  } else {
    Controller1.Screen.print("   BLUE");
  }
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  redrawTeamSelMenu();

  static bool up = false;
  static bool down = false;

  while (true) {
    bool cUp = Controller1.ButtonUp.pressing();
    if (cUp && !up) {
      weAreOnBlue = false;
      redrawTeamSelMenu();
    }
    up = cUp;

    bool cDown = Controller1.ButtonDown.pressing();
    if (Controller1.ButtonDown.pressing() && !down) {
      weAreOnBlue = true;
      redrawTeamSelMenu();
    }
    down = cDown;

    if (Controller1.ButtonA.pressing()) {
      break;
    }
  }

  Competition.drivercontrol(drivercontrol);
  Competition.autonomous(autonomous);
}