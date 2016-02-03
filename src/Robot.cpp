#include "WPILib.h"
#include <cstdlib>
#include <sstream>

#define ISJOYSTICK 0
#define DEBUG 1
#define DEADZONE 0.25
#define ARMERROR 0.1 //The max acceptable difference between the arms.

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
	enum joystick { //CONTROLLER
		MAINJOY = 0
	};

	enum buttons { //CONTROLLER

	};

	enum axis {  //CONTROLLER
		SHOOTBALL = 3, ARMDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors { //PWM
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTARM = 2, RIGHTARM = 3,
		LEFTROLLERS = 4, RIGHTROLLERS = 5,
	};

	enum inputs {  //ANALOG INPUT
		LEFTPOTCHANNEL = 0, RIGHTPOTCHANNEL = 1
	};

	enum digitalinputs { //DIGITAL INPUT
		MINARM = 0, MAXARM = 1, CHANNELA = 2, CHANNELB = 3
	};

	AnalogPotentiometer rightArmPotInput;
	AnalogPotentiometer leftArmPotInput;
	DigitalInput bottomSwitch;
	DigitalInput topSwitch;
	Encoder encoder;
	RobotDrive myRobot; // robot drive system
	Joystick controller; // only joystick
	JoystickButton rollerButton;
	Talon leftRollerMotor;
	Talon rightRollerMotor;
	Talon rightArmPotMotor;
	Talon leftArmPotMotor;

	const double pLeftGain = 0.405; //proportional speed constant
//	const double pRightGain = ;
	//TODO: Find right potentiometer's pGain
public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			leftRollerMotor(LEFTROLLERS),
			rightRollerMotor(RIGHTROLLERS),
			shooterAimMotor(SHOOTERAIM),
			encoder(CHANNELA, CHANNEL B, false, Encoder::EncodingType.k4X),
			bottomSwitch(MINARM),
			topSwitch(MAXARM,)
			rightArmPotInput(RIGHTPOTCHANNEL, 360, 10),
			//TODO: Find offset. either 12 (full scale of linear motion) or 3600 (full scale of angular motion)
			rightArmPotMotor(RIGHTARM),
			leftArmPotInput(LEFTPOTCHANNEL, 360, 10),
			leftArmPotMotor(LEFTARM)
)
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
		CameraServer::GetInstance()->SetQuality(50);
		std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
		//camera->SetExposureManual(50);
		//camera->SetBrightness(50);
		//camera->SetWhiteBalanceManual(0);
		CameraServer::GetInstance()->StartAutomaticCapture(camera);

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
		std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);

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

		int index = 0;
		double currentSetpoint; //holds desired setpoint
		const int size = 4; //number of setpoints. ground, lowered portcullis, raised portcullis, max
		const double setpoints[size] = {0.0, 0.5, 4.0, 4.5}; //setpoint locations
		//TODO: Find upper three setpoints
		currentSetpoint = setpoints[0]; //set to first setpoint

//		double rCurrentPosition = rightArmPotInput.GetAverageVoltage(); //get position value
//		motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
//		rightArmPotMotor.Set(motorSpeed); //drive elevator motor

//		double lCurrentPosition = rightArmPotInput.GetAverageVoltage(); //get position value
		//		motorSpeed = (currentPosition - currentSetpoint)*pGain; //convert position error to speed
		//		rightArmPotMotor.Set(motorSpeed); //drive elevator motor

		//TODO: See debug todo below.
		//TODO: Test the input we get from arm potentiometers to make sense of them.
		//TODO: Very cautiously give the motor an input and hope it doesn't break.
		//TODO: Add error checking for if the position of the arms differs by too much.
		//TODO: Add controls for moving the arms together.

		double lCurrentPosition = leftArmPotInput.Get();
		double rCurrentPosition = rightArmPotInput.Get();
		std::string potStatement;
		double armDifference = lCurrentPosition - rCurrentPosition; //accounts for difference in arm height

		//Sets motor speeds based on armMovement axis
		//If unequal. equalizes height of arm
		if ((fabs(armMovement) > .2) && fabs(armDifference) < ARMERROR) { //operator moving and arm heights equal
			//Left DART
			double lmotorSpeed = (lCurrentPosition - currentSetpoint)*pLeftGain; //convert position error to speed
			leftArmPotMotor.Set(lmotorSpeed);
			//Right Dart
			double rmotorSpeed = (rCurrentPosition - currentSetpoint)*pRightGain; //convert position error to speed
			rightArmPotMotor.Set(rmotorSpeed);
		} else if (fabs(armDifference) >= ARMERROR) { //operator moving and arm heights unequal
			//Try to move arms back together.
			currentSetpoint = (lCurrentPosition + rCurrentPosition) / 2;

			//Left DART
			double lmotorSpeed = (lCurrentPosition - currentSetpoint)*pLeftGain; //convert position error to speed
			leftArmPotMotor.Set(lmotorSpeed);
			//Right Dart
			double rmotorSpeed = (rCurrentPosition - currentSetpoint)*pRightGain; //convert position error to speed
			rightArmPotMotor.Set(rmotorSpeed);
		} else { //Operator not moving and arm heights equal
			//TODO: Find output to keep height
			//Add buttons for arm height here
			double lmotorSpeed = 0; //Left DART
			leftArmPotMotor.Set(lmotorSpeed);
			double rmotorSpeed = 0; //Right Dart
			rightArmPotMotor.Set(rmotorSpeed);
		}

#if DEBUG
		std::string aDirection = std::to_string(moveDirection);
		std::string aRotate = std::to_string(rotateAmount);
		std::string aArm = std::to_string(armMovement);
		std::string aLeftPos = std::to_string(lCurrentPosition);
		std::string aRightPos = std::to_string(rCurrentPosition);
		SmartDashboard::PutString("DB/String 3", ("Dir after: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
		SmartDashboard::PutString("DB/String 5", ("Arm after: " + aArm));
		SmartDashboard::PutString("DB/String 6", ("ArmPot: " + aLeftPos));
		SmartDashboard::PutString("DB/String 7", ("ArmPot: " + aRightPos));

		//Tell user if potentiometer is off
		double acceptableErrorLimit = 0.25;
		if ((bottomSwitch = 0) && (lCurrentPosition > acceptableErrorLimit) && (rCurrentPosition > acceptableErrorLimit)) {
			std::string potStatement = "BOTH "; //both pots off, left pot error printed first
			SmartDashboard::PutString("DB/String 8", ("PotError " + potStatement + lCurrentPosition + " " + rCurrentPosition));
		} else if ((bottomSwitch = 0) && (rCurrentPosition > acceptableErrorLimit)) {
			std::string potStatement = "RIGHT "; //right pot off
			SmartDashboard::PutString("DB/String 8 ", ("PotError " + potStatement + rCurrentPosition));
		} else if ((bottomSwitch = 0) && (lCurrentPosition > acceptableErrorLimit)) {
			std::string potStatement = "LEFT ";  //left pot off
			SmartDashboard::PutString("DB/String 8", ("PotError " + potStatement + rCurrentPosition));
		} else { //both pots accurate
			std::string potStatement = "NONE";
			SmartDashboard::PutString("DB/String 8", ("PotError " + potStatement))
			}
#endif

		myRobot.ArcadeDrive(moveDirection, rotateAmount, false);
#endif


	}

	void TestPeriodic()
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

START_ROBOT_CLASS(Robot)t
