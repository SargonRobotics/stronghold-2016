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
		SHOOTBALL = 3, SHOOTDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors { //PWM
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTSHOOT = 2, RIGHTSHOOT = 3,
		SHOOTSERVO = 7,
		ARM = 8, SHOOTER = 9
	};

	enum inputs {  //ANALOG INPUT
		LEFTPOTCHANNEL = 0, RIGHTPOTCHANNEL = 1
	};

	enum digitalinputs { //DIGITAL INPUT
			SHOOTERCHANNELA = 0, SHOOTERCHANNELB = 1,
			ARMCHANNELA = 2, ARMCHANNELB = 3,
			TOPLIMIT = 4
		};

	enum relays {
		LIGHTSWITCH = 3
	};

	DigitalInput topReset;
	Encoder shooterPos;
	Encoder armPos;
	RobotDrive myRobot; // robot drive system
	Joystick controller; // only joystick
	JoystickButton rollerButton;
	Victor leftShootMotor;
	Victor rightShootMotor;
	Victor armMotor;
	Victor shooterAimMotor;
	Servo shootServo;
	Timer timer;
	Relay lightSwitch;

public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0 and 1
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			leftShootMotor(LEFTSHOOT),
			rightShootMotor(RIGHTSHOOT),
			shooterPos(SHOOTERCHANNELA, SHOOTERCHANNELB, true, Encoder::EncodingType::k4X),
			armPos(ARMCHANNELA, ARMCHANNELB, false, Encoder::EncodingType::k4X),
			shooterAimMotor(SHOOTER),
			topReset(TOPLIMIT),
			armMotor(ARM),
			shootServo(SHOOTSERVO),
			lightSwitch(LIGHTSWITCH),
			timer()
	{
		myRobot.SetExpiration(0.1);
		//myRobot.SetInvertedMotor()
		//myRobot.SetInvertedMotor(LEFTDRIVE, true);
	}

private:
	LiveWindow *lw = LiveWindow::GetInstance();

	void RobotInit() {
		CameraServer::GetInstance()->SetQuality(50);
		std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
		//camera->SetExposureManual(50);
		//camera->SetBrightness(50);
		//camera->SetWhiteBalanceManual(0);
		CameraServer::GetInstance()->StartAutomaticCapture("cam0");
	}

	void AutonomousInit() {
			timer.Reset();
			timer.Start();
			shootServo.Set(0);
			lightSwitch.Set(Relay::kOn);
			SmartDashboard::PutBoolean("DB/LED 3", true);
		}

		void AutonomousPeriodic() {
			std::string cTime = std::to_string(timer.Get());
			SmartDashboard::PutString("DB/String 0", cTime);
			std::string autoSelected = SmartDashboard::GetString("DB/String 9", "Auto Selection");
			double currentTime = timer.Get();

			if (autoSelected == "short") {
				if(currentTime < 5){
					myRobot.ArcadeDrive(-0.5, 0, false); //Drive over defense
				} else {
					myRobot.ArcadeDrive(0, 0, false);
				}
			} else if(autoSelected == "portcullis") {
	//			TODO: test
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
	//			TODO: test
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
	//			TODO: test
				if(currentTime < 1.5){
					myRobot.ArcadeDrive(-0.5, 0, false); //drives up to defense
				} else if(currentTime < 3){
				//Move shooter down //lowers shooter to make the defense passable
				} else if(currentTime < 5){
					myRobot.ArcadeDrive(-0.5, 0, false); //drives over defense
				} else {
					myRobot.ArcadeDrive(0, 0, false);
				}
			} else if(autoSelected == "mid"){
	//			TODO: test
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
				if (currentTime < 3) {
				myRobot.ArcadeDrive(0, 0, false);
				DriverStation::ReportError("You are a dirty skrub!");
				}
			}
		}

	void TeleopInit() {
	}

	void TeleopPeriodic() {

	}

	void TestPeriodic() {
		lw->Run();
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
