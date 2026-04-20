#include "Simulator.h"
#include <Windows.h>
#include <iostream>
#include "../../../settings.h"


//SignalDeviceOscilloscopeSIMULATORowon

namespace signal {

	void SignalDeviceOscilloscopeSIMULATORowon::connect() {
		std::cout << "Инициировано подключение к SIMULATORowon" << std::endl; // OWON6102A connection has been started
		connection = true;
	}

	void SignalDeviceOscilloscopeSIMULATORowon::disconnect() {
		std::cout << "Запущено рассоединение с SIMULATORowon" << std::endl; //OWON6102A disconnection has been started
		connection = false;
	}

	void SignalDeviceOscilloscopeSIMULATORowon::setup() {
		std::cout << "Запущена настрокйка SIMULATORowon" << std::endl; // OWON6102A setup has been started
		auto& SETTINGS = Config::instance();
		auto& osc_sets = SETTINGS.getOscill_settings();
	}

	
	double SignalDeviceOscilloscopeSIMULATORowon::rawTickToVolts(double signal_tick) {
		return (signal_tick - 8192) / 6400;
	}

	std::vector<uint16_t> SignalDeviceOscilloscopeSIMULATORowon::getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) {
		std::vector<uint16_t> result(TICKS, 1);
		
		Sleep(SLEEP_MS);		// Типа ожидание триггера
		return result;
	}

}