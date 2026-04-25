#include "movable.h"
#include "settings.h"
#include <iostream>
using namespace std;

void movable::MovableDeviceStageStanda8MTL300XY::connect() {
	auto& SETTINGS = Config::instance();
	string TABLE_IP = SETTINGS.getTable_settings().getIpAdress();

	// Opening communication with real controller via Ethernet
	printf("Ожидется открытие соединения с Standa8MTL300XY...\n"); // Wait for opening of communication with Standa8MTL300XY.
	//10.0.0.100- default IP address of the controller

	char IP_ADRESS[256];
	strncpy_s(IP_ADRESS, TABLE_IP.c_str(), sizeof(IP_ADRESS) - 1);
	IP_ADRESS[sizeof(IP_ADRESS) - 1] = '\0';

	hComm = acsc_OpenCommEthernet(IP_ADRESS, ACSC_SOCKET_DGRAM_PORT);
	if (hComm == ACSC_INVALID)
	{
		connection = false;
		throw std::exception("Standa8MTL300XY connection error");
	}
	else
	{
		connection = true;
		printf(" Соединение С РЕАЛЬНЫМ Standa8MTL300XY успешно выполнено! Будьде осторожны, возможны прыжки и резкое движение!!!\n"); //Communication with the REAL Standa8MTL300XY was established successfully! Be aware of stage jumps and moving!!!
	}

}

void movable::MovableDeviceStageStanda8MTL300XY::setup() {
	// тут пожалуй придется выполнять commutation если не удастся настроить его выполнение контроллером автоматически
}

void movable::MovableDeviceStageStanda8MTL300XY::disconnect() {
	if (acsc_CloseComm(hComm)) {
		connection = false;
		cout << "Standa8MTL300XY отсоединён!\n"; //Standa8MTL300XY disconnected!
	}
	else {
		throw "cant disconnect Standa8MTL300XY";
	}

}

void movable::MovableDeviceStageStanda8MTL300XY::setHome() {

}

bool movable::MovableDeviceStageStanda8MTL300XY::is_connected() {
	return connection;
}

void movable::MovableDeviceStageStanda8MTL300XY::moveTo(std::vector<double> position) {
	if (position.size() == 2) {
		position = math::matVecMult(position, getSpecimenTransMatrix());
		position = math::vectorAdd(position, getSpecimenBasePoints()[0]);

		if (XMIN <= position[0] && position[0] <= XMAX && YMIN <= position[1] && position[1] <= YMAX) {
			if (acsc_ToPoint(hComm, 0, ACSC_AXIS_0, position[0], NULL))
			{
				printf("ATTENTION!!!	X AXIS   MOTOR  IS  SET TO MOVE TO POSITION %f\r \n", position[0]);
			}
			else
			{
				throw std::exception("Some error occured while using acsc_ToPoint command! \n");
			}

			if (acsc_ToPoint(hComm, 0, ACSC_AXIS_1, position[1], NULL))
			{
				printf("ATTENTION!!!	Y AXIS   MOTOR  IS  SET TO MOVE TO POSITION %f\r \n", position[1]);
			}
			else
			{
				throw std::exception("Some error occured while using acsc_ToPoint command! \n");
			}

		}
		else {
			throw std::exception("Demanded position is beyond of Standa8MTL300XY limits!");
		}
	}
	else {
		throw std::exception("Incorrect MoveTo position format!");
	}
}

std::vector<double> movable::MovableDeviceStageStanda8MTL300XY::getManualPoint(std::string message) {
	disableMotors();
	cout << message <<  " Setup with hands desired stage position and press enter" << endl;
	cin.get();
	return movable::MovableDeviceStageStanda8MTL300XY::getCoordinates();
}

std::vector<double> movable::MovableDeviceStageStanda8MTL300XY::getCoordinates() {
	vector<double> coords;
	coords.resize(2, 0);

	double XPOS;
	if (acsc_GetFPosition(hComm, ACSC_AXIS_0, &XPOS, NULL))
	{
		coords[0] = XPOS;
	}
	else
	{
		throw std::exception("Getting x position error");
	}

	double YPOS;
	if (acsc_GetFPosition(hComm, ACSC_AXIS_1, &YPOS, NULL))
	{
		coords[1] = YPOS;
	}
	else
	{
		throw std::exception("Getting y position error");
	}

	return coords;
}

void  movable::MovableDeviceStageStanda8MTL300XY::disableMotors() {
	disable_x_motor();
	disable_y_motor();
}

void  movable::MovableDeviceStageStanda8MTL300XY::enableMotors() {
	enable_x_motor();
	enable_y_motor();
}

int movable::MovableDeviceStageStanda8MTL300XY::enable_x_motor()
{
	int status = acsc_Enable(hComm, ACSC_AXIS_0, NULL);
	if (status)
	{
		//setXMotorOn(true);
		printf("ATTENTION!!!	X AXIS   MOTOR  HAS BEEN SWITCHED ON!!! \n");
		return 0;
	}
	else
	{
		cout << "x axis motor enabling error! Status: " << status << endl;
		return -1;
	}
}

int movable::MovableDeviceStageStanda8MTL300XY::enable_y_motor()
{
	int status = acsc_Enable(hComm, ACSC_AXIS_1, NULL);
	if (status)
	{
		//setYMotorOn(true);
		printf("ATTENTION!!!	Y AXIS   MOTOR  HAS BEEN SWITCHED ON!!! \n");
		return 0;
	}
	else
	{
		cout << "y axis motor enabling error! Status: " << status << endl;
		return -1;
	}
}

int movable::MovableDeviceStageStanda8MTL300XY::disable_x_motor()
{
	int status = acsc_Disable(hComm, ACSC_AXIS_0, NULL);
	if (status)
	{
		//setXMotorOn(false);
		printf("X AXIS   MOTOR  disabled \n");
		return 0;
	}
	else
	{
		cout << "ATTENTION!!! DISABLING ERROR!!!	X AXIS   MOTOR  still WORKS!! " << status << endl;
		return -1;
	}
}

int movable::MovableDeviceStageStanda8MTL300XY::disable_y_motor()
{
	int status = acsc_Disable(hComm, ACSC_AXIS_1, NULL);
	if (status)
	{
		//setYMotorOn(false);
		printf("Y AXIS   MOTOR  disabled \n");
		return 0;
	}
	else
	{
		cout << "ATTENTION!!! DISABLING ERROR!!!	Y AXIS   MOTOR  still WORKS!! " << status << endl;
		return -1;
	}
}

bool movable::MovableDeviceStageStanda8MTL300XY::is_moving() {
	return x_is_moving()||y_is_moving();
}

bool movable::MovableDeviceStageStanda8MTL300XY::x_is_moving()
{
	int State;
	if (!acsc_GetMotorState(hComm, ACSC_AXIS_0, &State, NULL))
	{
		throw std::exception("Motor x state getting error!");
		return false;
	}
	return State & ACSC_MST_MOVE;
}

bool movable::MovableDeviceStageStanda8MTL300XY::y_is_moving()
{
	int State;
	if (!acsc_GetMotorState(hComm, ACSC_AXIS_1, &State, NULL))
	{
		throw std::exception("Motor y state getting error!");
		return false;
	}
	return State & ACSC_MST_MOVE;
}


void movable::MovableDeviceStageStanda8MTL300XY::setupSpecimenCoordSys() {
	std::vector<std::vector<double>> basePlatePoints;
	std::vector<std::vector<double>> transforMatrix;

	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system ORIGIN!\n"));
	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system X AXIS POINT!\n"));
	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system Y AXIS POINT!\n"));
	setSpecimenBasePoints(basePlatePoints);
	transforMatrix = math::ThreePointsToTransMatrix(basePlatePoints[0], basePlatePoints[1], basePlatePoints[2]);
	setSpecimenTransMatrix(transforMatrix);

}


//												S I M U L A T O R

void movable::MovableDeviceStageSIMULATORstunda::connect() {
	auto& SETTINGS = Config::instance();
	bool SIMULATOR = SETTINGS.getTable_settings().getSimulator();
	string TABLE_IP = SETTINGS.getTable_settings().getIpAdress();
	std::cout << "Communication with the SIMULATOR stunda was established successfully!\n" << TABLE_IP << std::endl;
	connection = true;
}

void movable::MovableDeviceStageSIMULATORstunda::setup() {
	// тут пожалуй придется выполнять commutation если не удастся настроить его выполнение контроллером автоматически
}

void movable::MovableDeviceStageSIMULATORstunda::disconnect() {
	cout << endl << "SIMULATOR stunda disconnected" << endl;
	connection = false;
}

bool movable::MovableDeviceStageSIMULATORstunda::is_connected() {
	return connection;
}

void movable::MovableDeviceStageSIMULATORstunda::moveTo(std::vector<double> position) {
	if (position.size() == 2) {
		position = math::matVecMult(position, getSpecimenTransMatrix());
		position = math::vectorAdd(position, getSpecimenBasePoints()[0]);
		if (XMIN <= position[0] && position[0] <= XMAX && YMIN <= position[1] && position[1] <= YMAX) {
			printf("stunda simulator:	X AXIS   MOTOR  IS  SET TO MOVE TO POSITION %f\r \n", position[0]);
			printf("stunda simulator:	Y AXIS   MOTOR  IS  SET TO MOVE TO POSITION %f\r \n", position[1]);
			x = position[0]; y = position[1];

		}
		else {
			throw std::exception("Demanded position is beyond of stunda simulator limits!");
		}
	}
	else {
		throw std::exception("Incorrect MoveTo position format!");
	}
}

std::vector<double> movable::MovableDeviceStageSIMULATORstunda::getManualPoint(std::string message) {
	double x_, y_;
	std::cout << message << " Setup with hands desired stage position and press enter (simulator)" << std::endl;
	std::cout << "Enter X coordinate: ";
	std::cin >> x_;
	std::cout << "Enter Y coordinate: ";
	std::cin >> y_;
	x = x_; y = y_;
	return { x_, y_ };
}

void  movable::MovableDeviceStageSIMULATORstunda::disableMotors() {
	cout << endl << "simulator stunda motors disabled" << endl;
}

void  movable::MovableDeviceStageSIMULATORstunda::enableMotors() {
	cout << endl << "simulator stunda motors enabled" << endl;
}

bool movable::MovableDeviceStageSIMULATORstunda::is_moving() {
	return false;
}

std::vector<double> movable::MovableDeviceStageSIMULATORstunda::getCoordinates() {
	return { x, y };
}

void movable::MovableDeviceStageSIMULATORstunda::setHome() {

}

void movable::MovableDeviceStageSIMULATORstunda::setupSpecimenCoordSys() {
	std::vector<std::vector<double>> basePlatePoints;
	std::vector<std::vector<double>> transforMatrix;

	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system ORIGIN!\n"));
	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system X AXIS POINT!\n"));
	basePlatePoints.push_back(getManualPoint("Setting of the plate coord system Y AXIS POINT!\n"));
	setSpecimenBasePoints(basePlatePoints);
	transforMatrix = math::ThreePointsToTransMatrix(basePlatePoints[0], basePlatePoints[1], basePlatePoints[2]);
	setSpecimenTransMatrix(transforMatrix);

}