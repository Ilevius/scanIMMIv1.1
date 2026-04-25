#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <sstream>
#include "include/ACSC.h"
#include"math.h"
#pragma once
#pragma comment(lib, "include/ACSCL_x64.LIB")

//using namespace std;

namespace movable {

	// Базовый класс (абстрактный)
	class MovableDevice {
	public:
		virtual ~MovableDevice() = default;  // Виртуальный деструктор
		virtual void connect() = 0;
		virtual void disconnect() = 0;
		virtual void setup() = 0;
		virtual bool is_connected() = 0;
		std::vector<std::vector<double>> getSpecimenTransMatrix() { return specimenTransMatrix; };
		std::vector<std::vector<double>> getSpecTransMatInverse();
		void setSpecimenTransMatrix(std::vector<std::vector<double>> &v) { specimenTransMatrix = v; };
		std::vector<std::vector<double>> getSpecimenBasePoints() { return specimenBasePoints; };
		void setSpecimenBasePoints(std::vector<std::vector<double>> &v) { specimenBasePoints = v; };

	private:
		std::vector<std::vector<double>> specimenTransMatrix = { {1, 0}, {0, 1} };
		std::vector<std::vector<double>> specimenBasePoints = { {0, 0}, {1, 0}, {0, 1} };

	};

	class MovableDeviceStage : public MovableDevice {  // virtual для множественного наследования
	public:
		virtual ~MovableDeviceStage() = default;

		// Считывание всех координат (динамическое число)
		virtual std::vector<double> getCoordinates() = 0;

		// Количество осей
		virtual size_t getAxisCount() const = 0;

		virtual void setHome() = 0;

		virtual std::vector<double> getManualPoint(std::string message) = 0;

		virtual void setupSpecimenCoordSys() = 0;

		virtual void moveTo(std::vector<double> position) = 0;

		virtual bool is_moving() = 0;
		virtual void disableMotors() = 0;
		virtual void enableMotors() = 0;
	};

	// Конкретная модель
	class MovableDeviceStageStanda8MTL300XY : public MovableDeviceStage {

	public:
		void connect() override;
		void disconnect() override;
		void setup() override;
		bool is_connected() override;
		std::vector<double> getCoordinates() override;
		size_t getAxisCount() const override {return 2;}
		void moveTo(std::vector<double> position) override;
		void setHome() override;
		std::vector<double> getManualPoint(std::string message) override;
		bool is_moving() override;
		void disableMotors() override;
		void enableMotors() override;
		void setupSpecimenCoordSys() override;
	private:
		HANDLE hComm;
		const double XMIN = -149;
		const double XMAX = 149;
		const double YMIN = -149;
		const double YMAX = 149;
		bool connection = false;

		int enable_x_motor();
		int disable_x_motor();
		int enable_y_motor();
		int disable_y_motor();
		bool x_is_moving();
		bool y_is_moving();
	};

	// Симулятор стола

	class MovableDeviceStageSIMULATORstunda : public MovableDeviceStage {

	public:
		void connect() override;
		void disconnect() override;
		void setup() override;
		bool is_connected() override;
		std::vector<double> getCoordinates() override;
		size_t getAxisCount() const override { return 2; }
		void moveTo(std::vector<double> position) override;
		void setHome() override;
		std::vector<double> getManualPoint(std::string message) override;
		bool is_moving() override;
		void disableMotors() override;
		void enableMotors() override;
		void setupSpecimenCoordSys() override;
	private:
		HANDLE hComm;
		const double XMIN = -149;
		const double XMAX = 149;
		const double YMIN = -149;
		const double YMAX = 149;
		bool connection = false;
		double x, y;

		int enable_x_motor();
		int disable_x_motor();
		int enable_y_motor();
		int disable_y_motor();
		bool x_is_moving();
		bool y_is_moving();
	};

	/*Поскольку у однокоординатника нет энкодеров, придется при введении начальной точки и конечной делать запрос на ввод с клавиатуры*/



};
