#include "scan.h"


namespace scan {

	std::vector<double> Scan::getMeasure() {
		std::cout << " Началось осреднение сигналов с осцилографа" << std::endl; // Saving average signal from oscill to file has been started!
		auto& SETTINGS = Config::instance();
		int EMP_TICKS = SETTINGS.getOscill_settings().getEmptyTicks();
		int TICKS = SETTINGS.getOscill_settings().getWantedTicks();
		int SLEEP_MS = 1;
		int AVE_NUM = SETTINGS.getScan_settings().getNave();
		std::vector<double> signal = oscill_->getAveragedVolts(EMP_TICKS, TICKS, SLEEP_MS, AVE_NUM);
		signalProcessing::centerSignal(signal);
		return signal;
	}

	void MeasureVoltage::start(){
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		std::string fileName = "";
		std::cout << " Введите название файла: ";
		std::cin >> fileName;
		MeasuredSignal = getMeasure();
		std::string fullFileName = SETTINGS.getCommon_settings().getWorkFolder() + fileName;
		files::saveSignalToTxt(MeasuredSignal, oscill_->get_timebase_ns()*1e-9, fullFileName);
		std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
	};

	void Ascan::setBasePoints(){
		basePoints.clear();
		basePoints.push_back(stage_->getManualPoint("Setting of the A-scan point!\n"));
	}
	void Ascan::setPoints() {
		points.clear();
		setBasePoints();
		points = basePoints;
	}
	void Ascan::start() {
		std::vector<double> SignalAtPoint;
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		std::string fileName;
		std::cout << " Введите название файла: ";
		std::cin >> fileName;
		stage_->enableMotors();
		Sleep(1000);							// МОТОРЫ ДОЛЖНЫ УСПЕТЬ ВКЛЧИТЬСЯ ИНАЧЕ ПРОГА ВЫЛЕТАЕТ!!
		stage_->moveTo(points[0]);
		while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
			Sleep(100);
		}
		SignalAtPoint = getMeasure();
		stage_->disableMotors();
		std::string fullFileName = SETTINGS.getCommon_settings().getWorkFolder() + fileName;
		files::saveSignalToTxt(SignalAtPoint, oscill_->get_timebase_ns()*1e-9, fullFileName);
		std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
	}

	void Bscan::setBasePoints() {
		basePoints.clear();
		double x_, y_;
		std::cout  << "Введите координаты точки СТАРТА В-скана в системе координат пластины" << std::endl;
		std::cout << "Enter X coordinate: ";
		std::cin >> x_;
		std::cout << "Enter Y coordinate: ";
		std::cin >> y_;
		basePoints.push_back({ x_,y_ });

		std::cout  << "Введите координаты точки ФИНИША В-скана в системе координат пластины" << std::endl;
		std::cout << "Enter X coordinate: ";
		std::cin >> x_;
		std::cout << "Enter Y coordinate: ";
		std::cin >> y_;
		basePoints.push_back({ x_,y_ });
	};

	void Bscan::setPoints(){
		auto& SETTINGS = Config::instance();
		auto pair = math::getInterpolatedPoints(basePoints[0], basePoints[1], SETTINGS.getScan_settings().getNpoints());
		points.clear();
		points = pair.first;
		DIST_STEP = pair.second;
	}
	void Bscan::start() {
		if (points.size() > 0) {

			// Уточняем у осциллографа какой шаг по времени у его отсчетов напряжения
			double timebase_s = oscill_->get_timebase_ns()*1e-9;
			// массив отсчетов времени для мат файлов
			std::vector<double> times;
			//	заполняем массив отсчетов по времени!
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();
			times.resize(SETTINGS.getOscill_settings().getWantedTicks(), 0);
			for (size_t j = 0; j < SETTINGS.getOscill_settings().getWantedTicks(); j++) {
				times[j] = timebase_s * j;
			}

			// Сам скан и шаг по расстоянию
			std::vector<std::vector<double>> BscanData;
			std::vector<double> dists;
			// Замер в текущей точки для тестов
			std::vector<double> SignalAtPoint;

			// Включаем моторы для старта сканирования
			stage_->enableMotors();
			Sleep(1000); //				Нужно добавить проверку, что моторы успели включиться!!!
			for (size_t i = 0; i < points.size(); ++i) {
				std::vector<double> newPoint;
				newPoint = math::matVecMult(points[i], stage_->getSpecimenTransMatrix());
				newPoint = math::vectorAdd(newPoint, stage_->getSpecimenBasePoints()[0]);
				dists.push_back(DIST_STEP*i);
				stage_->moveTo(points[i]);
				while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
					Sleep(100);
				}
				SignalAtPoint = getMeasure();
				Sleep(1000);
				BscanData.push_back(SignalAtPoint);
				std::string filename = SETTINGS.getCommon_settings().getWorkFolder() + "Bscan.mat";
				std::string testFilename = SETTINGS.getCommon_settings().getWorkFolder() + "px" + std::to_string(i) + "py0.txt";
				files::createBscanMat(BscanData, dists, times, DIST_STEP, timebase_s, filename);
				//files::saveSignalToTxt(SignalAtPoint, oscill_->get_timebase_ns()*1e-9, testFilename);
				std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}
			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};

	void Cscan::setBasePoints() {
		basePoints.clear();
		basePoints.push_back(stage_->getManualPoint("Setting of the plate coord system ORIGIN!\n"));
		basePoints.push_back(stage_->getManualPoint("Setting of the plate coord system X AXIS POINT!\n"));
		basePoints.push_back(stage_->getManualPoint("Setting of the plate coord system Y AXIS POINT!\n"));
	};
	void Cscan::setPoints() {
		double x0, y0, xl, yl;
		size_t Nx, Ny;
		points.clear();
		setBasePoints();
		auto& SETTINGS = Config::instance();
		x0 = SETTINGS.getCscan_settings().getX0();
		y0 = SETTINGS.getCscan_settings().getY0();
		Nx = SETTINGS.getCscan_settings().getNx();
		Ny = SETTINGS.getCscan_settings().getNy();
		xl = SETTINGS.getCscan_settings().getXl();
		yl = SETTINGS.getCscan_settings().getYl();
		points = math::rectSnake(x0, y0, xl, yl, Nx, Ny);
	}
	void Cscan::start() {
		if (points.size() > 0) {
			std::vector<std::vector<double>> table_points;

			// Уточняем у осциллографа какой шаг по времени у его отсчетов напряжения
			double timebase_s = oscill_->get_timebase_ns()*1e-9;
			// массив отсчетов времени для мат файлов
			std::vector<double> times;
			//	заполняем массив отсчетов по времени!
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();
			times.resize(SETTINGS.getOscill_settings().getWantedTicks(), 0);
			for (size_t j = 0; j < SETTINGS.getOscill_settings().getWantedTicks(); j++) {
				times[j] = timebase_s * j;
			}

			// Сам скан и шаг по расстоянию
			std::vector<std::vector<double>> CscanData;
			// Замер в текущей точки для тестов
			std::vector<double> SignalAtPoint;
			std::string CscanPointsFileName = SETTINGS.getCommon_settings().getWorkFolder() + "\\Cscan\\CscanPoints.mat";
			files::createCscanPointsMat(basePoints, points, times, timebase_s, CscanPointsFileName);

			std::cout << endl << "Сохранение mat-файла c точками С-скана прошло успешно!" << endl;

			// Включаем моторы для старта сканирования
			stage_->enableMotors();
			Sleep(1000); //				Нужно добавить проверку, что моторы успели включиться!!!
			for (size_t i = 0; i < points.size(); ++i) {
				std::string CscanOnePointFileName = SETTINGS.getCommon_settings().getWorkFolder() + "\\Cscan\\" + to_string(i) + ".txt";
				std::vector<double> newPoint;
				newPoint = math::matVecMult(points[i], stage_->getSpecimenTransMatrix());
				newPoint = math::vectorAdd(newPoint, stage_->getSpecimenBasePoints()[0]);
				table_points.push_back(newPoint);
				stage_->moveTo(points[i]);
				while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
					Sleep(100);
				}
				SignalAtPoint = getMeasure();

				CscanData.push_back(SignalAtPoint);
				files::saveSignalToTxt(SignalAtPoint, timebase_s, CscanOnePointFileName);
				std::cout << " Сохранение точки "<< i << " из " << points.size()  << "  С - скана успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}

			std::string CscanMatFileName = SETTINGS.getCommon_settings().getWorkFolder() + "Cscan.mat";
			files::createCscanMat(CscanData, basePoints, points, table_points, times, timebase_s, CscanMatFileName);
			std::cout << endl << "Сохранение mat-файла С-скана прошло успешно!" << endl;

			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};


	//1 Выбор 4 базовых точек
	void Rscan::setBasePoints() {
		basePoints.clear();
		basePoints.push_back(stage_->getManualPoint("Setting of the 1-st point!\n"));
		basePoints.push_back(stage_->getManualPoint("Setting of the 2-nd point!\n"));
		basePoints.push_back(stage_->getManualPoint("Setting of the 3-rd point!\n"));
		basePoints.push_back(stage_->getManualPoint("Setting of the 4-th point!\n"));
	};
	//2 Получение точек 
	void Rscan::setPoints() {
		points.clear();
		setBasePoints();
		auto& SETTINGS = Config::instance();
		points = math::getRandomQuadrogonPoints(basePoints[0], basePoints[1], basePoints[2], basePoints[3], SETTINGS.getScan_settings().getNpoints());
	}

	void Rscan::start() {
		if (points.size() > 0) {
			// Уточняем у осциллографа какой шаг по времени у его отсчетов напряжения
			double timebase_s = oscill_->get_timebase_ns()*1e-9;
			// массив отсчетов времени для мат файлов
			std::vector<double> times;
			//	заполняем массив отсчетов по времени!
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();
			times.resize(SETTINGS.getOscill_settings().getWantedTicks(), 0);
			for (size_t j = 0; j < SETTINGS.getOscill_settings().getWantedTicks(); j++) {
				times[j] = timebase_s * j;
			}

			// Сам скан и шаг по расстоянию
			std::vector<std::vector<double>> RscanData;
			std::vector<double> dists;




			// Замер в текущей точки для тестов
			std::vector<double> SignalAtPoint;

			// Включаем моторы для старта сканирования
			stage_->enableMotors();
			Sleep(1000); //				Нужно добавить проверку, что моторы успели включиться!!!
			for (size_t i = 0; i < points.size(); ++i) {
				stage_->moveTo(points[i]);
				while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
					Sleep(100);
				}
				SignalAtPoint = getMeasure();
				dists.push_back(double(i));
				RscanData.push_back(SignalAtPoint);
				std::string filename = SETTINGS.getCommon_settings().getWorkFolder() + "Rscan.mat";
				files::createBscanMat(RscanData, dists, times, 1, timebase_s, filename);
				std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}
			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};
}