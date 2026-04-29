#include <Windows.h>
#include <iostream>
#include "SignalDeviceOscilloscopePicoScope5000s.h"
#include "../../../settings.h"

//																	PicoScope5000s

// ready
void signal::SignalDeviceOscilloscopePicoScope5000s::connect() {
	int16_t count;
	int8_t serials[100] = { 0 };
	int16_t serialLth = 100;
	PICO_STATUS CONNECTION_STATUS;
	PICO_STATUS POWER_MODE = PICO_POWER_SUPPLY_NOT_CONNECTED;

	ps5000aEnumerateUnits(&count, serials, &serialLth);
	if (count < 1) {
		std::cout << "The is no any PicoScope Device, check commutations and try later!" << std::endl;
		connection = false;
	}
	else {
		CONNECTION_STATUS = ps5000aOpenUnit(&HANDLE, serials, PS5000A_DR_8BIT);
		if (CONNECTION_STATUS == PICO_OK) {
			std::cout << "Device, serial: " << serials << "connected succesfully" << std::endl;
			connection = true;
		}
		else {
			if (CONNECTION_STATUS == 282) {
				std::cout << "Connection ended with status: " << CONNECTION_STATUS << ". Changing power supply mode to USB" << std::endl;
				Sleep(1000);
				ps5000aChangePowerSource(HANDLE, POWER_MODE);
				Sleep(1000);
				if (ps5000aPingUnit(HANDLE) == PICO_OK) {
					connection = true;
					std::cout << "PicoScope5000s connected!" << std::endl;
					ps5000aFlashLed(HANDLE, 3);
				}
				else {
					connection = false;
					std::cout << "Cant change power supply mode!" << std::endl;
				}
			}
			else {
				std::cout << "Connection ended with status: " << CONNECTION_STATUS << std::endl;
				connection = false;
			}
		}
	}
}
// ready
void signal::SignalDeviceOscilloscopePicoScope5000s::disconnect() {
	if (ps5000aCloseUnit(HANDLE) == PICO_OK) {
		std::cout << "PicoScope5000s disconnected successfully!" << std::endl;
		connection = false;
	}
	else throw "Cant disconnect!";
}

void signal::SignalDeviceOscilloscopePicoScope5000s::setup() {
	PICO_STATUS SETUP_STATUS;
	auto& SETTINGS = Config::instance();
	
	uint32_t wanted_ticks;
	int32_t max_samples;
	uint32_t segmIndex = 0;
	float sample_interval_ns;

	VoltScale = SETTINGS.getOscill_settings().getDataChScale_V();
	wanted_ticks = SETTINGS.getOscill_settings().getWantedTicks();
	setTimebase_code(SETTINGS.getOscill_settings().getTimebaseCode());

	//														Тут нужно привязать диапазон к значению в настройках
	// 1. Активируем канал A (DC coupling, 1V диапазон)
	SETUP_STATUS = ps5000aSetChannel(HANDLE, PS5000A_CHANNEL_A, 1,
		PS5000A_AC, picoChanRange(VoltScale), 0.0f);
	std::cout << "Канал A: " << SETUP_STATUS << std::endl;

	// 2. Деактивируем прочие каналы для максимальной битовой глубины
	SETUP_STATUS = ps5000aSetChannel(HANDLE, PS5000A_CHANNEL_B, 0, PS5000A_AC, PS5000A_1V, 0.0f);
	SETUP_STATUS = ps5000aSetChannel(HANDLE, PS5000A_CHANNEL_C, 0, PS5000A_AC, PS5000A_1V, 0.0f);
	SETUP_STATUS = ps5000aSetChannel(HANDLE, PS5000A_CHANNEL_D, 0, PS5000A_AC, PS5000A_1V, 0.0f);


	// 3. Устанавливаем разрешение 16 бит (требует 1 активный канал)
	SETUP_STATUS = ps5000aSetDeviceResolution(HANDLE, PS5000A_DR_16BIT);
	if (SETUP_STATUS == PICO_OK) {
		std::cout << "Разрешение: " << (size_t)PS5000A_DR_16BIT << " номером в списке" << std::endl;
	}
	else {
		throw "Resolution setup error!";
	}
	Sleep(2000);
	// 4. Выбираем timebase (масштаб времени), ищем подходящий
	SETUP_STATUS = ps5000aGetTimebase2(HANDLE, getTimebase_code(), wanted_ticks, &sample_interval_ns, &max_samples, segmIndex);
	if (SETUP_STATUS == PICO_OK && wanted_ticks <= max_samples) {
		set_timebase_ns(sample_interval_ns);
		std::cout << "Timebase, ns " << sample_interval_ns << std::endl;
	}
	else {
		throw "Timebase getting error! Error code: " + to_string(SETUP_STATUS);
	}
	// 5. Настраиваем триггер 
	SETUP_STATUS = ps5000aSetSimpleTrigger(HANDLE, 1, PS5000A_EXTERNAL, 1, PS5000A_RISING, 0, 0);
	if (SETUP_STATUS != PICO_OK) {
		throw "Trigger setup error!";
	}
	// 6. Настраиваем буфер для передачи сырых отсчетов
	SETUP_STATUS = ps5000aSetDataBuffer(HANDLE, PS5000A_CHANNEL_A, data_buffer, data_buffer_len, segmIndex, PS5000A_RATIO_MODE_NONE);
	if (SETUP_STATUS != PICO_OK) {
		throw "Data buffer setup error!";
	}
}

std::vector<uint16_t> signal::SignalDeviceOscilloscopePicoScope5000s::getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) {
	vector<uint16_t> result(TICKS, 0);
	PICO_STATUS	PicoStatus;
	int32_t preTicks = int32_t(EMPTY_TICKS);
	int32_t postTicks = int32_t(TICKS - EMPTY_TICKS);
	int32_t timeIndisposedMsl;
	uint32_t segmentIndex = 0;
	void *pParameter;
	int16_t ready = 0;

	uint32_t startIndex = 0;
	uint32_t noOfSamplesFact = TICKS;
	int16_t overFlow = 0;

	PicoStatus = ps5000aRunBlock(HANDLE, preTicks, postTicks, getTimebase_code(), &timeIndisposedMsl, segmentIndex, NULL, &pParameter);
	do {
		//Sleep(SLEEP_MS);
		ps5000aIsReady(HANDLE, &ready);
	} while (!ready);

	ps5000aGetValues(HANDLE, startIndex, &noOfSamplesFact, 0, PS5000A_RATIO_MODE_NONE, segmentIndex, &overFlow);

	for (size_t i = 0; i < TICKS; i += 1) {
		result[i] = data_buffer[i] + 32767;
	}
	return result;
}

double signal::SignalDeviceOscilloscopePicoScope5000s::rawTickToVolts(double signal_tick) {
	double result = (signal_tick - 32767) / 32767*VoltScale;
	return result;
}

PS5000A_RANGE signal::SignalDeviceOscilloscopePicoScope5000s::picoChanRange(double v) {
	if (v <= 0.01) return PS5000A_10MV;
	if (v <= 0.02) return PS5000A_20MV;
	if (v <= 0.05) return PS5000A_50MV;
	if (v <= 0.1)  return PS5000A_100MV;
	if (v <= 0.2)  return PS5000A_200MV;
	if (v <= 0.5)  return PS5000A_500MV;
	if (v <= 1.0)  return PS5000A_1V;
	if (v <= 2.0)  return PS5000A_2V;
	if (v <= 5.0)  return PS5000A_5V;
	if (v <= 10.0) return PS5000A_10V;
	if (v <= 20.0) return PS5000A_20V;
	return PS5000A_50V;
}




