#include <iostream>
#include "application.h"
#include "settings.h"
#include "files.h"
#include "signal_processing.h"
#include "scan.h"
#include "signal/Oscilloscope/OWON 6102A/SignalDeviceOscilloscopeOWON6102A.h"
#include "signal/Oscilloscope/PicoScope 5000 series/SignalDeviceOscilloscopePicoScope5000s.h"
#include "signal/Oscilloscope/Simulator/Simulator.h"


namespace app {

void equipMenu(State& AppState) {
	//								Здесь будет автопоиск и добавление экземпляров классов подсоединенных осциллографов и столов
	//								Надо создавать экземпляр с передачей ему юсб адреса
	cout << endl << "Вы находитесь в меню подключения оборудования" << endl;
	int option;
	auto& SETTINGS = Config::instance();

	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Выйти в главное меню" << endl;
		if (!AppState.stage) {
			cout << "1: Подключиться к столу STUNDA" << endl;
		}
		else {
			cout << "1: Отключиться от стола STUNDA" << endl;
		}
		cout << "2: Подключиться к однокоординатному столу (будет доступно позже)" << endl;
		if (!AppState.osc) {
			cout << "3: Подключиться осциллографу OWON" << endl;
			cout << "4: Подключиться осциллографу PicoScope" << endl;
		}
		else {
			cout << "3: Отключиться от осциллографа OWON" << endl;
			cout << "4: Отключиться от осциллографа PicoScope" << endl;
		}
		if (AppState.stage) {
			cout << "5: Настроить систему координат пластины" << endl;
		}
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			if (SETTINGS.getTable_settings().getSimulator()) {
				AppState.stage = std::make_shared<movable::MovableDeviceStageSIMULATORstunda>();
			}
			else {
				AppState.stage = std::make_shared<movable::MovableDeviceStageStanda8MTL300XY>();
			}
			AppState.stage->connect();
			break;
		case 2:
			cout << "Подключение к однокоординатному столу будет доступно позже" << endl;
			break;
		case 3:
			
			if (SETTINGS.getOscill_settings().getSIMULATOR()) {
				AppState.osc = std::make_shared<signal::SignalDeviceOscilloscopeSIMULATORowon>();
			}
			else {
				AppState.osc = std::make_shared<signal::SignalDeviceOscilloscopeOWON6102A>();
			}
			
			try {
				AppState.osc->connect();
				AppState.osc->setup();
			}
			catch (const char* error_msg) {
				cout << error_msg;
				break;
			}
			//
			break;
		case 4:
			AppState.osc = std::make_shared<signal::SignalDeviceOscilloscopePicoScope5000s>();
			try {
				AppState.osc->connect();
				AppState.osc->setup();
			}
			catch (const char* error_msg) {
				cout << error_msg;
				break;
			}
			//
			break;
		case 5:
			try {
				AppState.stage->setupSpecimenCoordSys();
			}
			catch (const char* error_msg) {
				cout << error_msg;
				break;
			}

		}

	}
}

void FourierMenu(State& AppState) {
	cout << endl << "Вы находитесь в меню обработки сигналов" << endl;
	int choice;
	auto& SETTINGS = Config::instance();

	while (true) {
		cout << endl<<"0: Выйти в главное меню" << endl;
		cout << "1: Ввести имя скана и получить Н-функцию" << endl;


		std::cout << "-> ";

		if (!(std::cin >> choice)) {
			//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}

		std::cin.ignore(1000, '\n');
		switch (choice) {
		case 0: return;

		case 1:
			SETTINGS.loadFromFile();
			std::string filename;
			std::vector<double> times, xs, alfas, freqs;
			std::vector<std::vector<double>> data, data_norm;
			cout << "Введите название мат файла В-скана, лежащего в рабочей папке и нажмите enter, параметры будут взяты из файла настроек"<< endl;
			cout << "->";
			cin >> filename;
			try{
				files::BscanCoreData Bscan = files::BscanFromFile(SETTINGS.getCommon_settings().getWorkFolder()+filename);
				times = Bscan.ts;
				xs = Bscan.xs;
				data = Bscan.data;
				data_norm = Bscan.data_norm;
			}
			catch(...){
				cout << "Не удалось открыть файл скана";
			}

			size_t t_n = times.size();
			size_t x_n = xs.size();
			if (x_n > 1 && t_n > 1 && data_norm.size() == x_n && data_norm[0].size() == t_n) {
				double timeStep = times[1] - times[0];
				double xStep = xs[1] - xs[0];

				size_t Nfreqs = SETTINGS.getFourier_settings().freqs_n();
				double Fmin_Hz = SETTINGS.getFourier_settings().fmin_MHz() * 1e6;
				double Fmax_Hz = SETTINGS.getFourier_settings().fmax_MHz() * 1e6;
				double Fstep_Hz = (Fmax_Hz - Fmin_Hz) / Nfreqs;

				size_t alfa_n = SETTINGS.getFourier_settings().alfa_n();
				double alfaMin = SETTINGS.getFourier_settings().alfa_min_dptr();
				double alfaStep = SETTINGS.getFourier_settings().alfa_step_dptr();

				size_t Tmin = size_t(SETTINGS.getFourier_settings().head_ms() * 1e-3 / timeStep);
				size_t Tmax = size_t(SETTINGS.getFourier_settings().tail_ms() * 1e-3 / timeStep);
				if (Tmin > t_n || Tmax > t_n) {
					Tmin = 0; Tmax = t_n;
				}

				t_n = Tmax - Tmin;

				for (size_t i = 0; i < Nfreqs; i++) {
					freqs.push_back(Fmin_Hz + i * Fstep_Hz);
				}
				for (size_t i = 0; i < alfa_n; i++) {
					alfas.push_back(alfaMin + i * alfaStep);
				}

				Eigen::MatrixXcd H = math::xtFourier(t_n, Nfreqs, x_n, alfa_n, times[0], Fmin_Hz, xs[0], alfaMin, timeStep, Fstep_Hz, xStep, alfaStep, data);
				files::spectrumToMatFile(freqs, alfas, H, SETTINGS.getCommon_settings().getWorkFolder()+"Hfunction.mat");
			}
			else {
				cout << "Некорректные данные в В-скане";
			}
			

			



			return;

		}
	}
}

void MeasureVoltage(State& AppState) {
	scan::MeasureVoltage CURR_MEAS_VOLT(AppState.osc);
	CURR_MEAS_VOLT.start();
	CURR_MEAS_VOLT.getGeneratorSignal(AppState.genSignal);
}

void Ascan(State AppState) {
	cout << endl << "Вы находитесь в меню А-сканирования" << endl; // You are in the Bscan menu. 
	scan::Ascan CURR_ASCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Выйти из меню А-сканирования" << endl;
		cout << "1: Указать точку сканирования вручную" << endl;
		cout << "2: Начать сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			CURR_ASCAN.manualSetBasePoints();
			break;
		case 2:
			CURR_ASCAN.setPoints();
			CURR_ASCAN.start();
			break;
		}
	}
}

void Bscan(State AppState) {
	cout << endl << "Вы находитесь в меню Bscan" << endl; // You are in the Bscan menu. 
	scan::Bscan CURR_BSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Выйти из меню В-сканирования" << endl;
		cout << "1: Указать начальную и конечную точки скана вручную" << endl;
		cout << "2: Начать В-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			CURR_BSCAN.manualSetBasePoints();
			break;
		case 2:
			CURR_BSCAN.setPoints();
			CURR_BSCAN.start();
			break;
		}
	}
}

void Cscan(State AppState) {
	cout << endl << "Вы находитесь в меню Cscan" << endl; // You are in the Bscan menu. 
	scan::Cscan CURR_CSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Покинуть меню С-сканирования" << endl;
		cout << "1: Начать С-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
									//Обработка неверного ввода (буквы и т.п.)
									//std::cin.clear();
									//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
									//std::cout << "Invalid input, try again.\n\n";
									//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			try {
				CURR_CSCAN.setPoints();				
			}
			catch (...) {
				std::cout << "Some trouble while scan points generation!";
				return;
			}
			
			CURR_CSCAN.start();
			break;
		}
	}
}

void Rscan(State AppState) {
	cout << endl << "Вы находитесь в меню Rscan (пакет А-сканов в рандомных точках)" << endl; // You are in the Bscan menu. 
	scan::Rscan CURR_RSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Покинуть меню R-сканирования" << endl;
		cout << "1: Ввести координаты базовых точек скана" << endl;
		cout << "2: Start Rscan" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			CURR_RSCAN.manualSetBasePoints();
			break;
		case 2:
			CURR_RSCAN.setPoints();
			CURR_RSCAN.start();
			break;
		}
	}
}

void Oscan(State AppState) {
	cout << endl << "Вы находитесь в меню Oscan" << endl; // You are in the Bscan menu. 
	scan::Oscan CURR_CSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: Покинуть меню O-сканирования" << endl;
		cout << "1: Начать O-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ОБЯЗАТЕЛЬНО ОБРАБОТАТЬ ВВОД нечисловых символов (буквы и т.п.) ЗАЦИКЛИВАЕТСЯ!!!
			//Обработка неверного ввода (буквы и т.п.)
			//std::cin.clear();
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Invalid input, try again.\n\n";
			//continue;
		}
		std::cin.ignore(1000, '\n');
		switch (option) {
		case 0: return;
		case 1:
			CURR_CSCAN.setPoints();
			CURR_CSCAN.start();
			break;
		}
	}
}

}