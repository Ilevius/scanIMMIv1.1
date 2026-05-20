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
	//								«десь будет автопоиск и добавление экземпл€ров классов подсоединенных осциллографов и столов
	//								Ќадо создавать экземпл€р с передачей ему юсб адреса
	cout << endl << "¬ы находитесь в меню подключени€ оборудовани€" << endl;
	int option;
	auto& SETTINGS = Config::instance();

	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ¬ыйти в главное меню" << endl;
		if (!AppState.stage) {
			cout << "1: ѕодключитьс€ к столу STUNDA" << endl;
		}
		else {
			cout << "1: ќтключитьс€ от стола STUNDA" << endl;
		}
		cout << "2: ѕодключитьс€ к однокоординатному столу (будет доступно позже)" << endl;
		if (!AppState.osc) {
			cout << "3: ѕодключитьс€ осциллографу OWON" << endl;
			cout << "4: ѕодключитьс€ осциллографу PicoScope" << endl;
		}
		else {
			cout << "3: ќтключитьс€ от осциллографа OWON" << endl;
			cout << "4: ќтключитьс€ от осциллографа PicoScope" << endl;
		}
		if (AppState.stage) {
			cout << "5: Ќастроить систему координат пластины" << endl;
		}
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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
			cout << "ѕодключение к однокоординатному столу будет доступно позже" << endl;
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
	cout << endl << "¬ы находитесь в меню обработки сигналов" << endl;
	int choice;
	auto& SETTINGS = Config::instance();

	while (true) {
		cout << endl<<"0: ¬ыйти в главное меню" << endl;
		cout << "1: ¬вести им€ скана и получить Ќ-функцию" << endl;


		std::cout << "-> ";

		if (!(std::cin >> choice)) {
			//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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
			cout << "¬ведите название мат файла ¬-скана, лежащего в рабочей папке и нажмите enter, параметры будут вз€ты из файла настроек"<< endl;
			cout << "->";
			cin >> filename;
			try{
				files::BscanCoreData Bscan = files::BscanFromFile(SETTINGS.getCommon_settings().getWorkFolder()+filename);
				Eigen::MatrixXcd H = signalProcessing::HfuncFromBscan(Bscan.ts, Bscan.xs, Bscan.data, freqs, alfas);
				filename.pop_back(); filename.pop_back(); filename.pop_back(); filename.pop_back();
				files::spectrumToMatFile(freqs, alfas, H, SETTINGS.getCommon_settings().getWorkFolder() + filename + "-H.mat");
			}
			catch(...){
				cout << "Ќе удалось открыть файл скана";
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
	cout << endl << "¬ы находитесь в меню ј-сканировани€" << endl; // You are in the Bscan menu. 
	scan::Ascan CURR_ASCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ¬ыйти из меню ј-сканировани€" << endl;
		cout << "1: ”казать точку сканировани€ вручную" << endl;
		cout << "2: Ќачать сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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
	cout << endl << "¬ы находитесь в меню Bscan" << endl; // You are in the Bscan menu. 
	scan::Bscan CURR_BSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ¬ыйти из меню ¬-сканировани€" << endl;
		cout << "1: ”казать начальную и конечную точки скана вручную" << endl;
		cout << "2: Ќачать ¬-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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
	cout << endl << "¬ы находитесь в меню Cscan" << endl; // You are in the Bscan menu. 
	scan::Cscan CURR_CSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ѕокинуть меню —-сканировани€" << endl;
		cout << "1: Ќачать —-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
									//ќбработка неверного ввода (буквы и т.п.)
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
	cout << endl << "¬ы находитесь в меню Rscan (пакет ј-сканов в рандомных точках)" << endl; // You are in the Bscan menu. 
	scan::Rscan CURR_RSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ѕокинуть меню R-сканировани€" << endl;
		cout << "1: ¬вести координаты базовых точек скана" << endl;
		cout << "2: Start Rscan" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {
			//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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
	cout << endl << "¬ы находитесь в меню Oscan" << endl; // You are in the Bscan menu. 
	scan::Oscan CURR_CSCAN(AppState.osc, AppState.stage);
	int option;
	while (true) {
		cout << "" << endl; // Choose an option and input its number
		cout << "0: ѕокинуть меню O-сканировани€" << endl;
		cout << "1: Ќачать O-сканирование" << endl;
		std::cout << "-> ";

		if (!(std::cin >> option)) {//ќЅя«ј“≈Ћ№Ќќ ќЅ–јЅќ“ј“№ ¬¬ќƒ нечисловых символов (буквы и т.п.) «ј÷» Ћ»¬ј≈“—я!!!
			//ќбработка неверного ввода (буквы и т.п.)
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