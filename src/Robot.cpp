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

	};

	enum axis {
		SHOOTBALL = 3, PULLBALL = 4, ARMDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors {
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTARM = 2, RIGHTARM = 3,
		LEFTROLLERS = 4, RIGHTROLLERS = 5,
		SHOOTERAIM = 6,
		LEFTSHOOT = 7, RIGHTSHOOT = 8
	};

	enum inputs {
		LEFTPOTCHANNEL = 0, RIGHTPOTCHANNEL = 1
	};

//	enum limit_switches {
//		MINARM = 0, LEFTARM = 1, RIGHTARM = 2
//	};

	AnalogInput rightArmPotInput;
	AnalogInput leftArmPotInput;
	RobotDrive myRobot; // robot drive system
	Joystick controller; // only joystick
	JoystickButton rollerButton;
	Talon leftRollerMotor;
	Talon rightRollerMotor;
	Talon shooterAimMotor;
	Talon rightArmPotMotor;
	Talon leftArmPotMotor;
	Talon rightShootMotor;
	Talon leftShootMotor;
public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			leftRollerMotor(LEFTROLLERS),
			rightRollerMotor(RIGHTROLLERS),
			shooterAimMotor(SHOOTERAIM),
			rightArmPotInput(RIGHTPOTCHANNEL),
			rightArmPotMotor(RIGHTARM),
			leftArmPotInput(LEFTPOTCHANNEL),
			leftArmPotMotor(LEFTARM),
			rightShootMotor(RIGHTSHOOT),
			leftShootMotor(LEFTSHOOT)
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
	/*	chooser = new SendableChooser();
		chooser->AddDefault(autoNameDefault, (void*) &autoNameDefault);
		chooser->AddObject(autoNameCustom, (void*) &autoNameCustom);
		SmartDashboard::PutData("Auto Modes", chooser);
		CameraServer::GetInstance()->SetQuality(50);
		std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
		//camera->SetExposureManual(50);
		//camera->SetBrightness(50);
		//camera->SetWhiteBalanceManual(0);
		CameraServer::GetInstance()->StartAutomaticCapture(camera); */

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
		//Setting up the axes
		double rightTrigger = controller.GetRawAxis(SHOOTBALL);
		double rightStickY = controller.GetRawAxis(ARMDIRECTION);
		double moveDirection = controller.GetRawAxis(MOVE);
		double rotateAmount = controller.GetRawAxis(ROTATE);
		double armMovement = controller.GetRawAxis(ARMDIRECTION);
		double shootState = controller.GetRawAxis(SHOOTBALL);
		double pullState = controller.GetRawAxis(PULLBALL);

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
		if(shootState > 0.5){
			rightShootMotor.Set(1);
			leftShootMotor.Set(1);
		} else {
			rightShootMotor.Set(0);
			leftShootMotor.Set(0);
		}

		if(pullState > 0.5){
			rightShootMotor.Set(-0.6);
			leftShootMotor.Set(-0.6);
		} else {
			rightShootMotor.Set(0);
			leftShootMotor.Set(0);
		}
#endif
		double rCurrentPosition = rightArmPotInput.GetAverageVoltage(); //get position value
//		motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
//		rightArmPotMotor.Set(motorSpeed); //drive elevator motor

		double lCurrentPosition = rightArmPotInput.GetAverageVoltage(); //get position value
		//		motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
		//		rightArmPotMotor.Set(motorSpeed); //drive elevator motor

		//TODO: See debug todo below.
		//TODO: Test the input we get from arm potentiometers to make sense of them.
		//TODO: Very cautiously give the motor an input and hope it doesn't break.
		//TODO: Add error checking for if the position of the arms differs by too much.
		//TODO: Add controls for moving the arms together.

#if DEBUG
		std::string aDirection = std::to_string(moveDirection);
		std::string aRotate = std::to_string(rotateAmount);
		std::string aArm = std::to_string(armMovement);
		std::string aRightPos = std::to_string(rCurrentPosition);
		//TODO: Add output for other arm potentiometer
		SmartDashboard::PutString("DB/String 3", ("Dir after: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
		SmartDashboard::PutString("DB/String 5", ("Arm after: " + aArm));
		SmartDashboard::PutString("DB/String 6", ("ArmPot: " + aRightPos));
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
