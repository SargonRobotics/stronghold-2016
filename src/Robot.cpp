#include "WPILib.h"
#include <cstdlib>
#include <sstream>

#define ISJOYSTICK 0
#define DEBUG 1
#define DEADZONE 0.25

class Robot: public IterativeRobot {
#if ISJOYSTICK
	enum joystick {
		MAINJOY = 0
	};

	enum buttons {
		SHOOTBALL = 1, ARMUP = 6, ARMDOWN = 4
	};

	enum axis {

	};

#else
	enum joystick {
		MAINJOY = 0
	};

	enum buttons {
		LIGHTS = 2, ARMDOWN = 4, ARMUP = 5, GOALAIM = 7
	};

	enum axis {
		SHOOTBALL = 3, PULLBALL = 2, SHOOTDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors {
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTSHOOT = 2, RIGHTSHOOT = 3,
		SHOOTCONTROL = 7,
		ARM = 8, SHOOTER = 9
	};

	enum limit_switches { //DIGITAL INPUT
		SHOOTERCHANNELA = 0, SHOOTERCHANNELB = 1, TOPLIMIT = 4
};

	DigitalInput topReset;
	RobotDrive myRobot; // robot drive system
	Joystick controller; // only joystick
	JoystickButton rollerButton;
	JoystickButton autoAimButton;
	Victor shooterAimMotor;
	Encoder shooterPos;
	Victor rightShootMotor;
	Victor leftShootMotor;
	Servo shootServo;

public:

	Robot():
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			autoAimButton(&controller, GOALAIM),
			shooterAimMotor(SHOOTER),
			shooterPos(SHOOTERCHANNELA, SHOOTERCHANNELB, false, Encoder::EncodingType::k4X),
			topReset(TOPLIMIT)

	{
		myRobot.SetExpiration(0.1);
		shooterPos.SetMinRate(10);
		//myRobot.SetInvertedMotor()
		//myRobot.SetInvertedMotor(LEFTDRIVE, true);
	}

private:
	LiveWindow *lw = LiveWindow::GetInstance();
	SendableChooser *chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	void RobotInit() {

	}

	/**
	 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
	 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
	 * Dashboard, remove all of the chooser code and uncomment the GetString line to get the auto name from the text box
	 * below the Gyro
	 *
	 * You can add additional auto modes by adding additional comparisons to the if-else structure below with additional strings.
	 * If using the SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit() {

	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {
	}

	void TeleopPeriodic() {
#if JOYSTICK
		//Driving
		myRobot.ArcadeDrive(controller);

		//Control for ball capture/shooting.
		//		if (rollerButton.Get()){
		//			leftRollerMotor.Set(1);
		//			rightRollerMotor.Set(1);
		//		}
		//		else {
		//			leftRollerMotor.Set(-1);
		//			rightRollerMotor.Set(-1);
		//		}

#else
//		Setting up the axes
//		double rightTrigger = controller.GetRawAxis(SHOOTBALL);
//		double rightStickY = controller.GetRawAxis(ARMDIRECTION);
		double moveDirection = controller.GetRawAxis(MOVE);
		double rotateAmount = controller.GetRawAxis(ROTATE);
		double shooterMovement = controller.GetRawAxis(SHOOTDIRECTION);
//		Get button
		bool goalAim = controller.GetRawButton(GOALAIM);

#if DEBUG
		std::string bDirection = std::to_string(moveDirection);
		std::string bRotate = std::to_string(rotateAmount);
		std::string bArm = std::to_string(shooterMovement);
		SmartDashboard::PutString("DB/String 0", ("Dir before: " + bDirection));
		SmartDashboard::PutString("DB/String 1", ("Rot before: " + bRotate));
		SmartDashboard::PutString("DB/String 2", ("Arm before: " + bArm));
#endif

		moveDirection = createDeadzone(moveDirection);
		rotateAmount = createDeadzone(rotateAmount);
		shooterMovement = createDeadzone(shooterMovement);
#if JOYSTICK
		//One trigger version
		if(shootState > 0.5){
			rightShootMotor.Set(1);
			leftShootMotor.Set(1);
		} else {
			rightShootMotor.Set(-0.6);
			leftShootMotor.Set(-0.6);
		}
#else

		//		Shooter Code
				double shootState = controller.GetRawAxis(SHOOTBALL);
				double pullState = controller.GetRawAxis(PULLBALL);
		//		Get shooter encoder angle
				double shooterDistance = shooterPos.GetDistance();
		//		TODO: find max value
				double shooterMax = 1000;
		//		TODO: uncomment once limit switch is installed, verify functionality
		/*		if (topReset.Get()) {
					if(shooterMovement < 0) {
						shooterMovement = 0;
						DriverStation::ReportError("Warning! Shooter at breaking point!");
					}
					shooterPos.Reset();
				} */
				shooterAimMotor.Set(shooterMovement);
		//		//Auto aim and shoot
				double distanceConstant = 500; //arbitrary
		//		TODO: camera testing
				//double goalAngle = ;
				//double aimValue = (length * distanceConstant * goalAngle);
				/*if (autoAimButton.Get()) {
					lightSwitch.Set(Relay::kOn);
					double current = shooterPos.GetDistance();
					if (current - 25 < aimValue) {
						shooterAimMotor.Set(-1);
					} else if (current - 5 < aimValue) {
						shooterAimMotor.Set(-0.5);
					} else if (current + 25 > aimValue) {
						shooterAimMotor.Set(1);
					} else if (current + 5 > aimValue) {
						shooterAimMotor.Set(0.5);
					} else {
						SmartDashboard::PutBoolean("DB/LED 0", true);
					}*/

				std::string getDistance = std::to_string(shooterDistance);
				std::string raw = std::to_string(rightShootMotor.GetRaw());
				SmartDashboard::PutString("DB/String 6", ("Distance: " + getDistance));
				SmartDashboard::PutString("DB/String 7", ("Get Raw: " + raw));

		//		Two trigger version
				if (shootState > 0.5 && pullState < 0.5) {
					shootServo.Set(0);
					if (SmartDashboard::GetBoolean("DB/LED 0", false)) {
						DriverStation::ReportError("I'm still aiming for the stars");
						rightShootMotor.Set(0);
						leftShootMotor.Set(0);
					} else {
						rightShootMotor.Set(1);
						//rightShootMotor.SetRaw(12);
						leftShootMotor.Set(1);
					}
				} else if (shootState < 0.5 && pullState > 0.5) {
					rightShootMotor.Set(-0.5);
					leftShootMotor.Set(-0.5);
				} else {
					rightShootMotor.Set(0);
					leftShootMotor.Set(0);
					shootServo.Set(100);
				}
#endif

#if DEBUG
		std::string aDirection = std::to_string(moveDirection);
		std::string aRotate = std::to_string(rotateAmount);
		std::string aArm = std::to_string(shooterMovement);
		std::string motorSpeed = std::to_string(leftShootMotor.Get());
		SmartDashboard::PutString("DB/String 3", ("Dir after: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
		SmartDashboard::PutString("DB/String 5", ("Arm after: " + aArm));
		SmartDashboard::PutString("DB/String 7", ("Speed: " + motorSpeed));
#endif
		myRobot.ArcadeDrive(moveDirection, rotateAmount, false);
#endif
	}

	void TestPeriodic() {
		lw->Run();
	}

	double createDeadzone(double amount, double deadzone=DEADZONE){
		if(fabs(amount) < deadzone){
			amount = 0;
		} else {
			if(amount < 0){
				amount += deadzone;
			} else {
				amount -= deadzone;
			}
			amount /= (1 - deadzone);
		}
		return amount;
	}
};

START_ROBOT_CLASS(Robot)
