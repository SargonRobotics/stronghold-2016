#include "WPILib.h"

class Robot: public IterativeRobot {
	enum joystick {
		MAINJOY = 0
	};

	enum buttons {
		SHOOTBALL = 1, ARMUP = 6, ARMDOWN = 4
	};

	enum motors {
		LEFTDRIVE = 0,
		RIGHTDRIVE = 1,
		LEFTARM = 2,
		RIGHTARM = 3,
		LEFTROLLERS = 4,
		RIGHTROLLERS = 5
	};

//	enum limit_switches {
//		MINARM = 0, LEFTARM = 1, RIGHTARM = 2
//	};

	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	JoystickButton rollerButton;
	Talon leftRollerMotor;
	Talon rightRollerMotor;
public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			stick(MAINJOY),
			rollerButton(&stick, SHOOTBALL),
			leftRollerMotor(LEFTROLLERS),
			rightRollerMotor(RIGHTROLLERS)
	{
		myRobot.SetExpiration(0.1);
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
		chooser = new SendableChooser();
		chooser->AddDefault(autoNameDefault, (void*) &autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*) &autoNameCustom);
		SmartDashboard::PutData("Auto Modes", chooser);
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
		std::cout << "Auto selected: " << autoSelected << std::endl;

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
		//Driving
		myRobot.ArcadeDrive(stick);

		//Control for ball capture/shooting.
//		if (rollerButton.Get()){
//			leftRollerMotor.Set(1);
//			rightRollerMotor.Set(1);
//		}
//		else {
//			leftRollerMotor.Set(-1);
//			rightRollerMotor.Set(-1);
//		}

		//Arm control.
	}

	void TestPeriodic() {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot)
