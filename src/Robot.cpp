#include "WPILib.h"
#include <sstream>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highhui.hpp>

#define ISJOYSTICK 0
#define DEBUG 1
#define DEADZONE 0.25
#define ARMERROR 0.1 //The max acceptable difference between the arms.
#define PIDMODE 0

class Robot: public IterativeRobot
{
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
	enum joystick
	{
		MAINJOY = 0
	};

	enum buttons
	{
		ARMACCEL = 1, LIGHTS = 2, GOALAIM = 7, ARMDOWN = 6, ARMUP = 5
	};

	enum axis
	{
		SHOOTBALL = 3, MOVE = 1, ROTATE = 0, PULLBALL = 2, SHOOTDIRECTION = 5, ARMDIRECTION = 5
	};

#endif

	enum motors
	{
		LEFTDRIVE = 1, RIGHTDRIVE = 0,
		LEFTSHOOT = 2, RIGHTSHOOT = 3,
		SHOOTSERVO = 7,
		ARM = 8, SHOOTER = 9
	};

	enum digitalinputs
	{
		SHOOTERCHANNELA = 0, SHOOTERCHANNELB = 1,
		ARMCHANNELA = 2, ARMCHANNELB = 3,
		TOPLIMIT = 4
	};

	enum relays
	{
		LIGHTSWITCH = 3
	};

	Encoder shooterPos;
	Encoder armPos;
	RobotDrive myRobot; // robot drive system
	JoystickButton rollerButton;
	JoystickButton armUpButton;
	JoystickButton armDownButton;
	JoystickButton autoAimButton;
	JoystickButton armAccelButton;
	Victor leftShootMotor;
	Victor rightShootMotor;
	Victor shooterAimMotor;
	Victor armMotor;
	Servo shootServo;
	DigitalInput limitSwitch;
	Timer timer;
	DigitalInput topReset;

//  Camera
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;
	Relay lightSwitch;
	Joystick controller; // only joystick
	JoystickButton lightsButton;
	std::shared_ptr<NetworkTable> contourTable;
	std::shared_ptr<NetworkTable> linesTable;
	std::string autoSelected;
	std::vector<double> area;
	std::vector<double> centerX;
	std::vector<double> centerY;
	std::vector<double> length;
	std::vector<double> left;
	std::vector<double> right;

public:

	Robot() :
			myRobot(LEFTDRIVE, RIGHTDRIVE),	// initialize the RobotDrive to use motor controllers on ports 0-3
			controller(MAINJOY),
			rollerButton(&controller, SHOOTBALL),
			armUpButton(&controller, ARMUP),
			armDownButton(&controller, ARMDOWN),
			autoAimButton(&controller, GOALAIM),
			armAccelButton(&controller, ARMACCEL),
			leftShootMotor(LEFTSHOOT),
			rightShootMotor(RIGHTSHOOT),
			//TODO: FIND THE RIGHT SAMPLING RATE WITHIN TEN PERCENT
			shooterPos(SHOOTERCHANNELA, SHOOTERCHANNELB, true, Encoder::EncodingType::k4X),
			armPos(ARMCHANNELA, ARMCHANNELB, true, Encoder::EncodingType::k4X),
			shooterAimMotor(SHOOTER),
			topReset(TOPLIMIT),
			armMotor(ARM),
			shootServo(SHOOTSERVO),
			limitSwitch(TOPLIMIT),
			lightSwitch(LIGHTSWITCH),
			lightsButton(&controller, LIGHTS),
			timer()
	{
		myRobot.SetExpiration(0.1);
	}

private:
	LiveWindow *lw = LiveWindow::GetInstance();

	void RobotInit()
	{
		CameraServer::GetInstance()->SetQuality(50);
		std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
		camera->SetExposureManual(0);
		camera->SetBrightness(0);
		camera->SetWhiteBalanceManual(0);
		CameraServer::GetInstance()->StartAutomaticCapture("cam0");
		//cv::VideoCapture camera(0);
		//cv::Mat image;
		//char key;

		//contourTable = NetworkTable::GetTable("GRIP/myContoursReport");
		//linesTable = NetworkTable::GetTable("GRIP/myContoursReport");
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
	}

	void AutonomousInit()
	{
		timer.Reset();
		timer.Start();
		shootServo.Set(0);
		lightSwitch.Set(Relay::kOn);
		SmartDashboard::PutBoolean("DB/LED 3", true);
	}

	void AutonomousPeriodic()
	{
		std::string cTime = std::to_string(timer.Get());
		SmartDashboard::PutString("DB/String 0", cTime);
		autoSelected = SmartDashboard::GetString("DB/String 9", "Auto Selection");
		double currentTime = timer.Get();
		double armDistance = armPos.Get();

		if (autoSelected == "short")
		{
					if(currentTime < 4.5)
					{
						myRobot.ArcadeDrive(0.5, 0, false); //Drive over defense
					}
					else
					{
						myRobot.ArcadeDrive(0, 0, false);
					}
		}
		else if (autoSelected == "moat")
		{
			if(currentTime < 5)
			{
				myRobot.ArcadeDrive(0.35, 0, false); //Drive over defense
			}
			else
			{
				myRobot.ArcadeDrive(0, 0, false);
			}
		}
		else if(autoSelected == "portcullis")
		{
//			TODO: test
			//double lowerArm = armPD(10, armDistance);
			//armMotor.Set(lowerArm);

			if(currentTime < 1.5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drive up to defense
			}
			else if(currentTime < 3 && armDistance > 1330)
			{
				//double raiseArm = armPD(1335, armDistance);
				//armMotor.Set(raiseArm);
			}
			else if(currentTime < 5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drives under defense
			}
			else
			{
				myRobot.ArcadeDrive(0, 0, false);
			}
		}
		else if(autoSelected == "drawbridge")
		{
//			TODO: test
			if(currentTime < 1.5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drive up to defense
			}
			else if(currentTime < 3){
			//Put encoder move arm down //lowers arm
			}
			else if(currentTime < 3.5)
			{
				myRobot.ArcadeDrive(-0.5, 0, false); //drives backward to lower door
			}
			else if(currentTime < 4)
			{
				//Move arm down further //lowers door under robot
			}
			else if(currentTime < 6)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drives over defense
			}
			else
			{
				myRobot.ArcadeDrive(0.5, 0, false);
			}
		}
		else if(autoSelected == "cheval de frise")
		{
//			TODO: test
			if(currentTime < 1.5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drives up to defense
			}
			else if(currentTime < 3)
			{
			//Move shooter down //lowers shooter to make the defense passable
			}
			else if(currentTime < 5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drives over defense
			}
			else
			{
				myRobot.ArcadeDrive(0, 0, false);
			}
		}
		else if(autoSelected == "mid")
		{
//			TODO: test
			//Need to be positioned on end of field
			int ballNum = 6;
			if(currentTime < 0.5)
			{
				myRobot.ArcadeDrive(0, -0.5, false); //rotate parallel to field
			}
			else if(currentTime < 3.5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //drive to end of field
			}
			else if(currentTime < 4)
			{
				myRobot.ArcadeDrive(0, 0.5, false); //rotates facing enemy courtyard
			}
			else if(currentTime < 4.5)
			{
				myRobot.ArcadeDrive(-0.75, 0, false); //moves to be ahead of ball
			}
			else if(currentTime < 5)
			{
				myRobot.ArcadeDrive(0.5, 0, false); //rotates parallel to field
			}
			for(int i = 0; i < ballNum; i++)
			{
				timer.Reset();
				timer.Start();

				if(currentTime < 0.5)
				{
					myRobot.ArcadeDrive(0.5, 0, false); //drive next to ball
				}
				else if(currentTime < 1)
				{
					myRobot.ArcadeDrive(0, 0.5, false); //rotates to face ball
				}
				else if(currentTime < 1.5)
				{
					myRobot.ArcadeDrive(0.5, 0, false); //hits ball towards our side
				}
				else if(currentTime < 2)
				{
					myRobot.ArcadeDrive(-0.5, 0, false); //drives back
				}
				else if(currentTime < 2.5)
				{
					myRobot.ArcadeDrive(0, -0.5, false); //rotates back
				}
			}
		}
		else
		{
			if (currentTime < 3)
			{
			myRobot.ArcadeDrive(0, 0, false);
			DriverStation::ReportError("You are a dirty skrub!");
			}
		}
	}

	void TeleopInit()
	{
		SmartDashboard::PutBoolean("DB/LED 3", false);
	}

	void TeleopPeriodic()
	{
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
#else
		//Setting up the axes
		double moveDirection = controller.GetRawAxis(MOVE);
		double rotateAmount = controller.GetRawAxis(ROTATE);
		double shooterMovement = controller.GetRawAxis(SHOOTDIRECTION);
		double armMovement = controller.GetRawAxis(ARMDIRECTION);

#if DEBUG
		std::string bMove = std::to_string(moveDirection);
		std::string aMove = std::to_string(createDeadzone(moveDirection, 0.6));

		SmartDashboard::PutString("DB/String 1", ("Move before: " + bMove));
		SmartDashboard::PutString("DB/String 2", ("Move after: " + aMove));
#endif

		moveDirection = createDeadzone(-moveDirection, 0.95);
		rotateAmount = createDeadzone(-rotateAmount, 0.5);
		shooterMovement = createDeadzone(-shooterMovement, 1);
		armMovement = createDeadzone(armMovement, 1);

#if DEBUG
		std::string aDirection = std::to_string(shootServo.Get());
		std::string aRotate = std::to_string(rotateAmount);
		SmartDashboard::PutString("DB/String 3", ("Servo: " + aDirection));
		SmartDashboard::PutString("DB/String 4", ("Rot after: " + aRotate));
#endif

		myRobot.ArcadeDrive(moveDirection, rotateAmount, false); //Makes the robot drive

/*		Camera Light Code
		bool lightLogic;
		if (lightLogic == 1)
		{
			lightSwitch.Set(Relay::kOn);
			SmartDashboard::PutBoolean("DB/LED 3", true);
		} else {
			lightSwitch.Set(Relay::kOff);
			SmartDashboard::PutBoolean("DB/LED 3", false);
		} //		Camera Light Code
		bool lightLogic;
		if (lightLogic == 1)
		{
			lightSwitch.Set(Relay::kOn);
			SmartDashboard::PutBoolean("DB/LED 3", true);
		} else {
			lightSwitch.Set(Relay::kOff);
			SmartDashboard::PutBoolean("DB/LED 3", false);
		} */

//		Shooter Code
		double shootState = controller.GetRawAxis(SHOOTBALL);
		double pullState = controller.GetRawAxis(PULLBALL);
		double shooterMax = 3020;
		double shooterMin = 0;

//		Intake and Shooting
//		Two trigger version
		if (shootState > 0.5 && pullState < 0.5)
		{
			rightShootMotor.Set(0.6);
			leftShootMotor.Set(0.6);

			timer.Start();

#if DEBUG
				std::string timerVal = std::to_string(timer.Get());
				SmartDashboard::PutString("DB/String 8", ("Timer: " + timerVal));
#endif

			if(timer.Get() > 0.5)
			{
				shootServo.Set(0);
				timer.Stop();
			}

		}
		else if (shootState < 0.5 && pullState > 0.5)
		{
			rightShootMotor.Set(-0.5);
			leftShootMotor.Set(-0.5);
		}
		else
		{
			rightShootMotor.Set(0);
			leftShootMotor.Set(0);
			shootServo.Set(100);

			timer.Reset();
		}

		//SmartDashboard::PutString("DB/String 8", area);
		//SmartDashboard::PutString("DB/String 9", length);

//		Moving Shooter Code
		if(limitSwitch.Get() == 0)
		{
			shooterAimMotor.Set(shooterMovement);
		}
		else if(limitSwitch.Get() == 1 && shooterMovement < 0)
		{
			shooterAimMotor.Set(shooterMovement);
		}
		else if(limitSwitch.Get() == 1 && shooterMovement > 0)
		{
			DriverStation::ReportError("Trying to exceed limit switch");
		}
		else
		{
			shooterAimMotor.Set(0);
		}

#if DEBUG
		std::string shooterValue = std::to_string(shooterMovement);
		std::string limitval = std::to_string(limitSwitch.Get());
		SmartDashboard::PutString("DB/String 5", ("Shooter: " + shooterValue));
		SmartDashboard::PutString("DB/String 7", ("Limit: " + limitval));
#endif

#endif

//		Arm Code
		double armDistance = armPos.Get();

		std::string getArm = std::to_string(armDistance);
		SmartDashboard::PutString("DB/String 6", ("Distance: " + getArm));

		double armMin = 0;
		double armMax = 1345;

		if ((armDistance > armMin) && (armDistance < armMax))
		{
			SmartDashboard::PutBoolean("DB/LED 4", true);
		}
		else
		{
			SmartDashboard::PutBoolean("DB/LED 4", false);
			//DriverStation::ReportError("Outside of safe arm limits!");
		}
		if (armUpButton.Get() == 1 && (armDownButton.Get() == 0))
		{
			if (armAccelButton.Get())
			{
				armMotor.Set(1);
			}
			else
			{
				armMotor.Set(0.6);
			}
		}
		else if (armDownButton.Get() == 1 && (armUpButton.Get() == 0))
		{
			if (armAccelButton.Get())
			{
				armMotor.Set(-1);
			}
			else
			{
				armMotor.Set(-0.6);
			}
		}
		else
		{
			armMotor.Set(0);
		}
	}

	void TestPeriodic()
	{

	}

	double createDeadzone(double amount, double max, double deadzone = DEADZONE)
	{
		amount = (fabs(amount) <= deadzone) ? 0 : (amount = (amount < 0) ? amount += deadzone : amount -= deadzone);

		return ((max - 0) / ((1 - deadzone) - 0) * (amount - 0));
	}

#if PIDMODE
	double shooterPID(int target, int current) {
		//TODO: test values using the ziegler-nichols method. Safety of the bot first!
		//since we want no overshoot, an over-damped algorithm is preferred. This produces a slower, smoother response.
		//overdamp by setting KI to 1/2 value (estimate)
		int pErr = target - current;
		int prevErr = 0;
		static int iErr = 0;
		iErr += pErr;
		int dErr = prevErr - pErr;
		int pOut = 1 * pErr; //arbitrary gain. larger value increases oscillation
		if (pOut < 5) {
			pOut = 5;
		} else if (pOut > 3015) {
			pOut = 1340;
		}
		int iOut = 1500 * iErr; //arbitrary gain. sum of all errors, goal is to nullify steady state error
		int dOut = .2 * dErr; //arbitrary gain. output decrease if  process variable is increasing rapidly
		int output = pOut + iOut + dOut;
		return output;
		prevErr = pErr;

	}
	double armPD(int target, int current) {
		//TODO: test values using the ziegler-nichols method. Safety of the bot first!
		//since we want no overshoot, an over-damped algorithm is preferred. This produces a slower, smoother response.
		//overdamp by setting KI to 1/2 value (estimate)
		//PD takes less bandwidth  and speeds up system, since exact positioning is not as necessary
		int pErr = target - current;
		static int prevErr = 0;
		static int dErr = 0;
		dErr = prevErr - pErr;
		int pervErr;
		int pOut = 1 * pErr; //arbitrary gain. larger value increases oscillation
		if (pOut < 5) {
			pOut = 5;
		} else if (pOut > 1340) {
			pOut = 1340;
		}
		int dOut = .2 * dErr; //arbitrary gain. output decrease if  process variable is increasing rapidly
		int output = pOut + dOut;
		return output;
		prevErr = pErr;

	}

#endif
};

START_ROBOT_CLASS(Robot)
