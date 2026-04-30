#include "scan.h"
#include <numbers>

namespace scan {
	std::vector<double> Scan::getMeasure() {
		std::cout << " Началось осреднение сигналов с осцилографа" << std::endl; // Saving average signal from oscill to file has been started!
		auto& SETTINGS = Config::instance();
		int EMP_TICKS = SETTINGS.getOscill_settings().getEmptyTicks();
		int TICKS = SETTINGS.getOscill_settings().getWantedTicks();
		int SLEEP_MS = 1;
		int AVE_NUM = SETTINGS.getOscill_settings().getAveN();
		std::vector<double> signal = oscill_->getAveragedVolts(EMP_TICKS, TICKS, SLEEP_MS, AVE_NUM);
		signalProcessing::centerSignal(signal);
		return signal;
	}

	void Scan::start() {
		if (points.size() > 0) {
			// Чтение актуальных настроек
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();

			// Основные пареметры скана
			std::string filename;
			double timebase_s = oscill_->get_timebase_ns() * 1e-9;
			uint16_t Nticks = SETTINGS.getOscill_settings().getWantedTicks();
			double dist_step;
			uint16_t aveN = SETTINGS.getOscill_settings().getAveN();

			//  Вычисление шага по расстоянию
			if (points.size() > 1) {
				dist_step = math::euclideanDistance(points[0], points[1]);
			}
			else {
				dist_step = 0;
			}

			// Некоторые векторные величины
			std::vector<std::vector<double>> ScanData;
			std::vector<double> dist_ticks;
			std::vector<double> time_ticks;
			std::vector<double> SignalAtPoint;
			
			// Запрос на ввод имени файла в который будут сохранены данные
			std::cout << " Введите название файла: ";
			std::cin >> filename;
			std::string fullFilename = SETTINGS.getCommon_settings().getWorkFolder() + filename;

			// Заплонение вектора отсчетами времени
			for (size_t i = 0; i < Nticks; i++) {
				time_ticks.push_back(i * timebase_s);
			}

			// Заполнение вектора отсчетами расстояния   (имеет смысл только для В-скана)
			for (size_t i = 0; i < points.size(); i++) {
				dist_ticks.push_back(i * dist_step);
			}


			/*
			здесь надо бы создать если ее нет временную папку на случай сбоя сохранения основного файла
			*/

			// Файл с основными параметрами замера для временного каталога			
			//				здесь надо добавить сохранение числа осреднений
			std::string tempMat = SETTINGS.getCommon_settings().getWorkFolder() + "\\temp\\scanParameters.mat";
			files::createCscanPointsMat(basePoints, points, time_ticks, timebase_s, tempMat);

			// Включаем моторы для старта сканирования
			stage_->enableMotors();
			Sleep(1000); //				Нужно добавить проверку, что моторы успели включиться!!!

			// Обход базовых точек скана ( в основном у сканов они получаются крайними ) на предмет проверки вылета стола
			std::cout << std::endl << "Внимание! Автоматический проход столом базовых точек. Проверка доступности точек!!!" << std::endl;
			// Содержимое цикла обработать на исключения и если вылет вывести причину и вернуть в меню скана
			for (size_t i = 0; i < basePoints.size(); ++i) {
				stage_->moveTo(basePoints[i]);
				while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
					Sleep(100);
				}
				Sleep(1000);
				std::cout << "Базовая точка " << i << " из " << basePoints.size() << " успешно достигнута"  << std::endl;
			}

			std::cout << std::endl << "Базовые точки пройдены успешно, начинается сканирование!!!" << std::endl;

			for (size_t i = 0; i < points.size(); ++i) {
				stage_->moveTo(points[i]);
				while (stage_->is_moving()) {//			Тут добавить проверку, что стол приехал в нужную точку с некоторой точностью!!!
					Sleep(100);
				}
				Sleep(1000);
				SignalAtPoint = getMeasure();
				ScanData.push_back(SignalAtPoint);
				// сохранение файла ( хотелось бы, чтобы дозаписывался)

				/*сохранение файла с замером в текущей точке во временном каталоге*/
				std::string aPointInTemp = SETTINGS.getCommon_settings().getWorkFolder() + "\\temp\\" + to_string(i) + ".txt";
				files::saveSignalToTxt(SignalAtPoint, timebase_s, aPointInTemp);

				// Подтверждение снятия и сохранение точки, сколько еще осталось точек     надо добавить расчет оставшегося времени!!!
				std::cout << " Сохранение точки " << i << " из " << points.size() << "  скана успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}
			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
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

	void Ascan::manualSetBasePoints(){
		basePoints.clear();
		std::vector<double> basePoint = stage_->getManualPoint("Setting of the A-scan point!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
	}
	void Ascan::setPoints() {
		points.clear();
		if (basePoints.size()) {
			points = basePoints;
		}
		else {
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();
			points.push_back({SETTINGS.getAscan_settings().getX(), SETTINGS.getAscan_settings().getY() });
		}
		
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
		//files::saveSignalToTxt(SignalAtPoint, oscill_->get_timebase_ns()*1e-9, fullFileName);
		files::saveAscanToMat(points[0], SignalAtPoint, oscill_->get_timebase_ns() * 1e-9, fullFileName);
		std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
	}

	void Bscan::manualSetBasePoints() {
		basePoints.clear();
		std::vector<double> basePoint = stage_->getManualPoint("Переместите стол руками в НАЧАЛЬНУЮ точку В-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
		basePoint = stage_->getManualPoint("Переместите стол руками в КОНЕЧНУЮ точку В-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
	};
	void Bscan::setPoints(){
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		if (!basePoints.size()) {
			auto& SETTINGS = Config::instance();
			SETTINGS.loadFromFile();
			basePoints.push_back({ SETTINGS.getBscan_settings().x_start(), SETTINGS.getBscan_settings().y_start() });
			basePoints.push_back({ SETTINGS.getBscan_settings().x_finish(), SETTINGS.getBscan_settings().y_finish() });
		}

		auto pair = math::getInterpolatedPoints(basePoints[0], basePoints[1], SETTINGS.getBscan_settings().points_n());
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
				files::createBscanMat(BscanData, dists, times, DIST_STEP, timebase_s, points, filename);
				//files::saveSignalToTxt(SignalAtPoint, oscill_->get_timebase_ns()*1e-9, testFilename);
				std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}
			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};

	void Cscan::manualSetBasePoints() {
		// нет большого смысла в задании точек С-скана вручную
	};
	void Cscan::setPoints() {
		double x0, y0, xl, yl;
		size_t Nx, Ny;
		points.clear();
		auto& SETTINGS = Config::instance();
		x0 = SETTINGS.getCscan_settings().getX0();
		y0 = SETTINGS.getCscan_settings().getY0();
		Nx = SETTINGS.getCscan_settings().getNx();
		Ny = SETTINGS.getCscan_settings().getNy();
		xl = SETTINGS.getCscan_settings().getXl();
		yl = SETTINGS.getCscan_settings().getYl();
		basePoints.clear();
		basePoints.push_back({ x0, y0 });
		basePoints.push_back({ x0+xl, y0 });
		basePoints.push_back({ x0, y0+yl });
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

	void Rscan::manualSetBasePoints() {
		basePoints.clear();
		std::vector<double> basePoint = stage_->getManualPoint("Переместите стол руками в ПЕРВУЮ точку R-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
		basePoint = stage_->getManualPoint("Переместите стол руками во ВТОРУЮ точку R-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
		basePoint = stage_->getManualPoint("Переместите стол руками в ТРЕТЬЮ точку R-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
		basePoint = stage_->getManualPoint("Переместите стол руками в ЧЕТВЕРТУЮ точку R-скана и нажмите enter!\n");
		basePoints.push_back(stage_->toPlateCoords(basePoint));
	};
	void Rscan::setPoints() {
		points.clear();
		auto& SETTINGS = Config::instance();
		points = math::getRandomQuadrogonPoints(basePoints[0], basePoints[1], basePoints[2], basePoints[3], SETTINGS.getRscan_settings().points_n());
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
				files::createBscanMat(RscanData, dists, times, 1, timebase_s, points, filename);
				std::cout << "  Сохранение файла успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}
			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};

	void Oscan::setPoints() {
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		basePoints.push_back({ 0, 0 });
		size_t Nr, Nphi;
		double r = SETTINGS.getOscan_settings().r_max();
		double r0 = SETTINGS.getOscan_settings().r_min();
		double phi_min = SETTINGS.getOscan_settings().phi_min_deg();
		double phi_max = SETTINGS.getOscan_settings().phi_max_deg();
		double anR, aPhi;
		std::vector<double> aPoint;
		Nr = SETTINGS.getOscan_settings().r_n();
		Nphi = SETTINGS.getOscan_settings().phi_n();
		points.clear();
		aPoint = { 0, 0 };
		points.push_back(aPoint);


		for (size_t i = 1; i < Nr; i++) {
			anR = r0 + i * r / Nr;
			for (size_t j = 0; j <= Nphi; j++) {
				if (i % 2 == 0) {
					aPhi = (Nphi - j) * 0.5 * std::numbers::pi / Nphi;
				}
				else {
					aPhi = j * 0.5 * std::numbers::pi / Nphi;
				}
				
				aPoint = { anR * cos(aPhi), anR * sin(aPhi) };
				points.push_back(aPoint);
			}
		}
	}
	void Oscan::start() {
		if (points.size() > 0) {
			std::vector<std::vector<double>> table_points;

			// Уточняем у осциллографа какой шаг по времени у его отсчетов напряжения
			double timebase_s = oscill_->get_timebase_ns() * 1e-9;
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
			std::string CscanPointsFileName = SETTINGS.getCommon_settings().getWorkFolder() + "\\Oscan\\OscanPoints.mat";
			files::createCscanPointsMat(basePoints, points, times, timebase_s, CscanPointsFileName);

			std::cout << endl << "Сохранение mat-файла c точками O-скана прошло успешно!" << endl;

			// Включаем моторы для старта сканирования
			stage_->enableMotors();
			Sleep(1000); //				Нужно добавить проверку, что моторы успели включиться!!!
			for (size_t i = 0; i < points.size(); ++i) {
				std::string CscanOnePointFileName = SETTINGS.getCommon_settings().getWorkFolder() + "\\Oscan\\" + to_string(i) + ".txt";
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
				std::cout << " Сохранение точки " << i << " из " << points.size() << "  O - скана успешно выполнено" << std::endl << std::endl; //File saved succesfully!
			}

			std::string CscanMatFileName = SETTINGS.getCommon_settings().getWorkFolder() + "Oscan.mat";
			files::createCscanMat(CscanData, basePoints, points, table_points, times, timebase_s, CscanMatFileName);
			std::cout << endl << "Сохранение mat-файла O-скана прошло успешно!" << endl;

			stage_->disableMotors();
		}
		else throw "There is no points to scan at!";
	};


}