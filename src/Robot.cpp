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
		GOALAIM = 3
	};

	enum axis {
		SHOOTBALL = 3, PULLBALL = 2, ARMDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors {
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTARM = 2, RIGHTARM = 3,
		LEFTSHOOT = 4, RIGHTSHOOT = 5,
		SHOOTERAIM = 6, SHOOTCONTROL = 7
	};

	enum limit_switches { //DIGITAL INPUT
		MINARM = 0, MAXARM = 1, CHANNELA = 2, CHANNELB = 3,
};

	DigitalInput bottomSwitch;
	DigitalInput topSwitch;
	RobotDrive myRobot; // robot drive system
	Joystick controller; // only joystick
	JoystickButton rollerButton;
	JoystickButton autoAimButton;
	Talon shooterAimMotor;
	Encoder shooterAngle;
	Talon rightShootMotor;
	Talon leftShootMotor;
	Servo shootServo;
	//Arm
	Talon leftArm;
	Talon rightArm;
public:

	Robot():
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			autoAimButton(&controller, GOALAIM),
			shooterAimMotor(SHOOTERAIM),
			shooterAngle(CHANNELA, CHANNELB, false, Encoder::EncodingType::k4X),
			bottomSwitch(MINARM),
			topSwitch(MAXARM),
			rightArm(RIGHTARM),
			leftArm(LEFTARM)
	{
		myRobot.SetExpiration(0.1);
		shooterAngle.SetMinRate(10);
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
		autoSelected = *((std::string*) chooser->GetSelected());
		//std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);

		if (autoSelected == autoNameCustom) {
			//Custom Auto goes here
		} else {
			//Default Auto goes here
		}
	}

	void AutonomousPeriodic() {
		if (autoSelected == autoNameCustom) {
			//Custom Auto goes here
		} else {
			//Default Auto goes here
		}
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
		double armMovement = controller.GetRawAxis(ARMDIRECTION);
		double shootState = controller.GetRawAxis(SHOOTBALL);
		double pullState = controller.GetRawAxis(PULLBALL);
//		Get button
		bool goalAim = controller.GetRawButton(GOALAIM);
//		Get encoder angle
		double count = shooterAngle.Get();


#if DEBUG
		std::string bDirection = std::to_string(moveDirection);
		std::string bRotate = std::to_string(rotateAmount);
		std::string bArm = std::to_string(armMovement);
		SmartDashboard::PutString("DB/String 0", ("Dir before: " + bDirection));
		SmartDashboard::PutString("DB/String 1", ("Rot before: " + bRotate));
		SmartDashboard::PutString("DB/String 2", ("Arm before: " + bArm));
#endif

		moveDirection = createDeadzone(moveDirection);
		rotateAmount = createDeadzone(rotateAmount);
		armMovement = createDeadzone(armMovement);
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
		//Two trigger version
		if (shootState > 0.5 && pullState < 0.5) {
			rightShootMotor.Set(1);
			leftShootMotor.Set(1);
			shootServo.Set(100);
		} else if (shootState < 0.5 && pullState > 0.5) {
			rightShootMotor.Set(-0.3);
			leftShootMotor.Set(-0.3);
		} else {
			rightShootMotor.Set(0);
			leftShootMotor.Set(0);
			shootServo.Set(0);
		}
#endif

#if DEBUG
		std::string aDirection = std::to_string(moveDirection);
		std::string aRotate = std::to_string(rotateAmount);
		std::string aArm = std::to_string(armMovement);
		std::string motorSpeed = std::to_string(leftShootMotor.Get());
		std::string printAngle = std::to_string(count);
		SmartDashboard::PutString("DB/String 3", ("Dir after: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
		SmartDashboard::PutString("DB/String 5", ("Arm after: " + aArm));
		SmartDashboard::PutString("DB/String 7", ("Speed: " + motorSpeed));
		SmartDashboard::PutString("DB/String 8", ("Angle: " + printAngle));
#endif
		myRobot.ArcadeDrive(moveDirection, rotateAmount, false);
#endif

		//Arm control.
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
