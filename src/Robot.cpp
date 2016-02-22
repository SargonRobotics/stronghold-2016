#include "WPILib.h"
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
		SHOOTBALL = 1, GOALAIM = 2, ARMDOWN = 4, ARMUP = 6,
	};

	enum axis {

	};

#else
	enum joystick { //CONTROLLER
		MAINJOY = 0
	};

	enum buttons { //CONTROLLER
		GOALAIM = 3, LIGHTS = 4
	};

	enum axis {  //CONTROLLER
		SHOOTBALL = 3, ARMDIRECTION = 5, MOVE = 1, ROTATE = 0, PULLBALL = 2, SHOOTDIRECTION = 4
	};

#endif

	enum motors { //PWM
		LEFTDRIVE = 1, RIGHTDRIVE = 0,
		LEFTSHOOT =2, RIGHTSHOOT = 3,
		SHOOTSERVO = 7,
		ARM = 8, SHOOTER = 9
	};

	enum digitalinputs { //DIGITAL INPUT
		SHOOTERCHANNELA = 0, SHOOTERCHANNELB = 1,
		ARMCHANNELA = 2, ARMCHANNELB = 3
	};

	enum relays {
		LIGHTSWITCH = 3
	};

	Encoder shooterPos;
	Encoder armPos;
	RobotDrive myRobot; // robot drive system
	JoystickButton rollerButton;
#if JOYSTICK
	JoystickButton armUpButton;
	JoystickButton armDownButton;
#endif
	JoystickButton autoAimButton;
	Victor leftShootMotor;
	Victor rightShootMotor;
	Victor shooterAimMotor;
	Victor armMotor;
	Servo shootServo;
	Timer timer;
	//Camera
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;
	Relay lightSwitch;
	Joystick controller; // only joystick
	JoystickButton lightsButton;
	//std::shared_ptr<NetworkTable> contourTable;
	//std::shared_ptr<NetworkTable> linesTable;
	//std::string autoSelected;
	//std::vector<double> area;
	//std::vector<double> centerX;
	//std::vector<double> centerY;
	//std::vector<double> length;
	//std::vector<double> left;
	//std::vector<double> right;
	std::string autoSelected;

public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0-3
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
#if JOYSTICK
			armUpButton(&controller, ARMUP),
			armDownButton(&controller, ARMDOWN),
#endif
			autoAimButton(&controller, GOALAIM),
			leftShootMotor(LEFTSHOOT),
			rightShootMotor(RIGHTSHOOT),
			shooterPos(SHOOTERCHANNELA, SHOOTERCHANNELB, false, Encoder::EncodingType::k4X),
			armPos(ARMCHANNELA, ARMCHANNELB, false, Encoder::EncodingType::k4X),
			shooterAimMotor(SHOOTER),
			armMotor(ARM),
			shootServo(SHOOTSERVO),
			lightSwitch(LIGHTSWITCH),
			lightsButton(&controller, LIGHTS),
			timer()
	{
		myRobot.SetExpiration(0.1);
	}

private:

	void RobotInit() {
		//CameraServer::GetInstance()->SetQuality(50);
		//std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
		//camera->SetExposureManual(50);
		//camera->SetBrightness(50);
		//camera->SetWhiteBalanceManual(0);
		//CameraServer::GetInstance()->StartAutomaticCapture("cam0");

		//contourTable = NetworkTable::GetTable("GRIP/myContoursReport");
		//linesTable = NetworkTable::GetTable("GRIP/myContoursReport");

		//while (true) {
			//area = contourTable->GetNumberArray("myContoursReport/area", llvm::ArrayRef<double>());
			//centerX = contourTable->GetNumberArray("myContoursReport/centerX", llvm::ArrayRef<double>());
			//centerY = contourTable->GetNumberArray("myContoursReport/centerY", llvm::ArrayRef<double>());
			//unsigned int currentArea = area.size();
			//Wait(1);
			//length = linesTable->GetNumberArray("myLinesReport/length", llvm::ArrayRef<double>());
			//left = linesTable->GetNumberArray("myCountoursReport/x1", llvm::ArrayRef<double>());
			//right = linesTable->GetNumberArray("myCountoursReport/x2", llvm::ArrayRef<double>());

			//std::cout << "GRIP area: " << currentArea << std::endl;
			//std::cout << "GRIP X: " << currentArea << std::endl;
			//std::cout << "GRIP Y: " << centerY << std::endl;
		//}
	}

	void AutonomousInit() {
		timer.Reset();
		timer.Start();
		shootServo.Set(0);
	}

	void AutonomousPeriodic() {
		//TODO: Add short, portcullis, drawbridge, cheval de frise, and sally port
		std::string cTime = std::to_string(timer.Get());
		SmartDashboard::PutString("DB/String 0", cTime);
		autoSelected = SmartDashboard::GetString("DB/String 9", "Auto Selection");
		lightSwitch.Set(Relay::kOn);

		double currentTime = timer.Get();

		if (autoSelected == "short") {
			if(currentTime < 5){
				myRobot.ArcadeDrive(-0.5, 0, false); //Drive over defense
			} else {
				myRobot.ArcadeDrive(0, 0, false);
			}
		} else if(autoSelected == "portcullis") {
			if(currentTime < 1.5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drive up to defense
			} else if(currentTime < 3){
				armMotor.Set(1);
			} else if(currentTime < 5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drives under defense
			} else {
				myRobot.ArcadeDrive(0, 0, false);
			}
		} else if(autoSelected == "drawbridge"){
			if(currentTime < 1.5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drive up to defense
			} else if(currentTime < 3){
			//Put encoder move arm down //lowers arm
			} else if(currentTime < 3.5){
				myRobot.ArcadeDrive(0.5, 0, false); //drives backward to lower door
			} else if(currentTime < 4){
			//Move arm down further //lowers door under robot
			} else if(currentTime < 6){
				myRobot.ArcadeDrive(-0.5, 0, false); //drives over defense
			} else {
				myRobot.ArcadeDrive(-0.5, 0, false);
			}
		} else if(autoSelected == "cheval de frise"){
			if(currentTime < 1.5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drives up to defense
			} else if(currentTime < 3){
			//Move shooter down //lowers shooter to make the defense passable
			} else if(currentTime < 5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drives over defense
			} else {
				myRobot.ArcadeDrive(0, 0, false);
			}
		} else if(autoSelected == "sally port"){
			if(currentTime < 1.5){
				myRobot.ArcadeDrive(-0.5, 0, false);
			} else if(currentTime < 3){
				//TODO: use sensor for getting in position
			}
		} else if(autoSelected == "mid"){
			//Need to be positioned on end of field
			int ballNum = 6;
			if(currentTime < 0.5){
				myRobot.ArcadeDrive(0, -0.5, false); //rotate parallel to field
			} else if(currentTime < 3.5){
				myRobot.ArcadeDrive(-0.5, 0, false); //drive to end of field
			} else if(currentTime < 4){
				myRobot.ArcadeDrive(0, 0.5, false); //rotates facing enemy courtyard
			} else if(currentTime < 4.5){
				myRobot.ArcadeDrive(-0.75, 0, false); //moves to be ahead of ball
			} else if(currentTime < 5){
				myRobot.ArcadeDrive(-0.5, 0, false); //rotates parallel to field
			}
			for(int i = 0; i < ballNum; i++){
				timer.Reset();
				timer.Start();
				if(currentTime < 0.5){
					myRobot.ArcadeDrive(-0.5, 0, false); //drive next to ball
				} else if(currentTime < 1){
					myRobot.ArcadeDrive(0, 0.5, false); //rotates to face ball
				} else if(currentTime < 1.5){
					myRobot.ArcadeDrive(-0.5, 0, false); //hits ball towards our side
				} else if(currentTime < 2){
					myRobot.ArcadeDrive(0.5, 0, false); //drives back
				} else if(currentTime < 2.5){
					myRobot.ArcadeDrive(0, -0.5, false); //rotates back
				}
			}
		} else {
			myRobot.ArcadeDrive(0, 0, false);
			DriverStation::ReportError("You are a dirty skrub!");
		}
	}

	void TeleopInit() {
	}

	void TeleopPeriodic() {
#if JOYSTICK
//		Driving
		myRobot.ArcadeDrive(controller);

//		One trigger shooter
		if(shootState > 0.5){
			rightShootMotor.Set(1);
			leftShootMotor.Set(1);
		} else {
			rightShootMotor.Set(-0.6);
			leftShootMotor.Set(-0.6);
		}

//		Arm Code
		if (armUpButton.Get() && (armDownButton.Get = 0)) {
			armMotor.Set(1);
		} else if (armDownButton.Get() && (armUpButton.Get = 0)) {
			armMotor.Set(-1);
		} else {
			armMotor.Set(0);
		}
#else
		//Setting up the axes
		double rightTrigger = controller.GetRawAxis(SHOOTBALL);
		double moveDirection = controller.GetRawAxis(MOVE);
		double rotateAmount = controller.GetRawAxis(ROTATE);
		double shooterMovement = controller.GetRawAxis(SHOOTDIRECTION);
		double armMovement = controller.GetRawAxis(ARMDIRECTION);

#if DEBUG
		std::string bDirection = std::to_string(moveDirection);
		std::string bRotate = std::to_string(rotateAmount);
		std::string bShooter = std::to_string(shooterMovement);
		std::string bArm = std::to_string(armMovement);
		SmartDashboard::PutString("DB/String 0", ("Dir before: " + bDirection));
		SmartDashboard::PutString("DB/String 1", ("Rot before: " + bRotate));
		SmartDashboard::PutString("DB/String 2", ("Shooter before: " + bShooter));
		SmartDashboard::PutString("DB/String 3", ("Arm before: " + bArm));
#endif

		moveDirection = createDeadzone(moveDirection);
		rotateAmount = createDeadzone(rotateAmount);
		shooterMovement = createDeadzone(shooterMovement);
		armMovement = createDeadzone(armMovement);

#if DEBUG
		std::string aDirection = std::to_string(moveDirection);
		std::string aRotate = std::to_string(rotateAmount);
		std::string aArm = std::to_string(armMovement);
		SmartDashboard::PutString("DB/String 3", ("Dir after: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
		SmartDashboard::PutString("DB/String 5", ("Arm after: " + aArm));
#endif
//		Shooter Code
		double shootState = controller.GetRawAxis(SHOOTBALL);
		double pullState = controller.GetRawAxis(PULLBALL);
//		Get shooter encoder angle
		double shooterCount = shooterPos.Get();
		double shooterDistance = shooterPos.GetDistance();

		std::string getIt = std::to_string(shooterCount);
		std::string getDistance = std::to_string(shooterDistance);
		SmartDashboard::PutString("DB/String 6", ("Get: " + getIt));
		SmartDashboard::PutString("DB/String 7", ("Distance: " + getDistance));
		double shooterMin = 0;
		double shooterMax = 1000;

//		Two trigger version
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

//		Auto aim and shoot
		double aimValue = 4; //arbitrary
		if (autoAimButton.Get()) {
			double current = shooterPos.GetDistance();
			if ((current - 3 > aimValue) && (current > shooterMin)) {
				shooterAimMotor.Set(-1);
			} else if ((current + 3 < aimValue) && (current < shooterMax)) {
				shooterAimMotor.Set(1);
			} else {
				shooterAimMotor.Set(0);
			}
		} else {
			if (shooterMovement == 0) {
				shooterAimMotor.Set(0);
			} else {
				shooterAimMotor.Set(shooterMovement);
			}
		}

//		Arm Code
		double armCount = armPos.Get();
		double armDistance = armPos.GetDistance();
		double armMin = 0;
		double armMax = 1000;

		if (armMovement == 0) {

			armMotor.Set(0);
		} else {
			armMotor.Set(armMovement);
		}
#endif
	myRobot.ArcadeDrive(moveDirection, rotateAmount, false);
	if (lightsButton.Get() == 1) {
		lightSwitch.Set(Relay::kOn);
	} else {
		lightSwitch.Set(Relay::kOff);
	}
	}

	void TestPeriodic() {

	}

	double createDeadzone(double amount, double deadzone = DEADZONE) {
		if (fabs(amount) < deadzone) {
			amount = 0;
		} else {
			if (amount < 0) {
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

