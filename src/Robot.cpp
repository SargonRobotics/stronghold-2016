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
		LIGHTS = 4
	};

	enum axis {
		SHOOTBALL = 3, ARMDIRECTION = 5, MOVE = 1, ROTATE = 0
	};

#endif

	enum motors {
		LEFTDRIVE = 0, RIGHTDRIVE = 1,
		LEFTARM = 2, RIGHTARM = 3,
		LEFTROLLERS = 4, RIGHTROLLERS = 5,
	};

	enum inputs {
		LEFTPOTCHANNEL = 0, RIGHTPOTCHANNEL = 1
	};

	enum relays {
		LIGHTSWITCH = 3
	};

//	enum limit_switches {
//		MINARM = 0, LEFTARM = 1, RIGHTARM = 2
//	};

//IMAQdx Image Processing
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;
	Relay lightSwitch;
	Joystick controller; // only joystick
	JoystickButton lightsButton;

public:
	Robot() :
		lightSwitch(LIGHTSWITCH),
		controller(MAINJOY),
		lightsButton(&controller, LIGHTS)

	{

	}

private:
	LiveWindow *lw = LiveWindow::GetInstance();

	void RobotInit() {
//		CameraServer::GetInstance()->SetQuality(50);
//		std::shared_ptr<USBCamera> camera(new USBCamera ("cam0" , true));
//		camera->SetExposureManual(30);
//		CameraServer::GetInstance()->StartAutomaticCapture("cam0");

		 //IMAQdx Imaging
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		//the camera name (ex "cam0") can be found through the roborio web interface
		imaqError = IMAQdxOpenCamera("cam0", IMAQdxCameraControlModeController, &session);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError) + "\n");
		}
		imaqError = IMAQdxConfigureGrab(session);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxConfigureGrab error: " + std::to_string((long)imaqError) + "\n");
		}

	}

	void AutonomousInit() {

	}

	void AutonomousPeriodic() {

	}

	void TeleopInit() {
	}

	void TeleopPeriodic() {
		//IMAQdx Image Processing
		IMAQdxStartAcquisition(session);
		while(IsOperatorControl() && IsEnabled()) {
			if (lightsButton.Get() == 1) {
				lightSwitch.Set(Relay::kOn);
			} else {
				lightSwitch.Set(Relay::kOff);
			}

			IMAQdxGrab(session, frame, true, NULL);
			if(imaqError != IMAQdxErrorSuccess) {
				DriverStation::ReportError("IMAQdxGrab error: " + std::to_string((long)imaqError) + "\n");
			} else {
				imaqDrawShapeOnImage(frame, frame, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);
				frcReadImage(frame,"/var/volatile/tmp/opencv-frame.png");
				CameraServer::GetInstance()->SetImage(frame);
			}
			Wait(0.005);				// wait for a motor update time
			}
		     // stop image acquisition
			IMAQdxStopAcquisition(session);
	}

	void TestPeriodic() {
		lw->Run();
	}

};

START_ROBOT_CLASS(Robot)
