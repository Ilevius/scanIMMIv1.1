#include <Windows.h>
#include <iostream>
#include "SignalDeviceOscilloscopeOWON6102A.h"
#include "../../../settings.h"

#pragma comment(lib, "include/visa64.lib")


//OWON6102A

namespace signal {
	// пойдет
	void SignalDeviceOscilloscopeOWON6102A::connect() {
		std::cout << "Инициировано подключение к OWON6102A" << endl; // OWON6102A connection has been started
																// Адрес прибора
		const char* resource = "USB0::0x5345::0x1235::2222099::INSTR";

		DEVICE = VI_NULL;
		RESOURCE_MANAGER = VI_NULL;
		ViStatus status;
		// Открываем Resource Manager
		status = viOpenDefaultRM(&RESOURCE_MANAGER);
		if (status != VI_SUCCESS)
		{
			throw std::exception("Resource manager error!");
		}
		//Открываем прибор
		status = viOpen(RESOURCE_MANAGER, resource, VI_NULL, VI_NULL, &DEVICE);
		if (status != VI_SUCCESS)
		{
			//cout << "Device not found!\n";
			throw "Device not found!\n";
		}
		else {
			printf(" Осцилограф успешно подключен\n"); //Oscilloscope has been connected
			connection = true;

		}
	}

	// пойдет
	void signal::SignalDeviceOscilloscopeOWON6102A::disconnect() {
		std::cout << "Запущено рассоединение с OWON6102A" << std::endl; //OWON6102A disconnection has been started
		viClose(DEVICE);
		viClose(RESOURCE_MANAGER);
		printf(" Осцилограф был успешно отсоединён\n"); // Oscilloscope has been disconnected
		signal::SignalDeviceOscilloscopeOWON6102A::connection = false;
	}

	// должно работать, обновить модуль настроек под числовые парметры depmem, horizontal and vertrical scales
	void signal::SignalDeviceOscilloscopeOWON6102A::setup() {
		cout << "Запущена настройка OWON6102A" << endl; // OWON6102A setup has been started

		auto& SETTINGS = Config::instance();
		auto& osc_sets = SETTINGS.getOscill_settings();

		CURR_DEPMEM = find_depMem(int64_t(osc_sets.getDepmem()));
		CURR_VOLT_SCALE = find_chanScale(int64_t(osc_sets.getDataChScale_V()*1e3));
		CURR_HORSCALE = find_horScale(int64_t(osc_sets.getHorScale()*1e6));

		string setup_commands[] = {
			":HORIzontal:SCALe " + horScale_str(CURR_HORSCALE) + "\n",  // horisontal scale
			":ACQuire:PRECision 14\n",// precision
			":CH1:DISPlay ON\n",			// set chanell to display
			":CH1:OFFSet 1.2\n",
			":CH1:COUP AC\n",
			":TRIG:SING:MODE " + osc_sets.getTrigMode() + "\n",
			":TRIG:SING:EDGE:COUP " + osc_sets.getTrigCoup() + "\n",
			":TRIG:SING:EDGE:SOUR CH2\n",
			":TRIG:SING:EDGE:LEV " + osc_sets.getTrigEdgeLevel() + "\n",
			":ACQ:DEPMEM " + depMem_str(CURR_DEPMEM) + "\n",
			":CH1:SCAL " + chanDivScale_str(CURR_VOLT_SCALE) + "\n"
		};

		//											Начинаем настройку осцилографа
		for (string command : setup_commands) {
			viPrintf(DEVICE, command.c_str());
			Sleep(100);
		}

		set_timebase_ns(8);

		//											Передача стартовых команд
		cout << "Запрос" << "                                 " << '|' << "  " << "Ответ\n";
		cout << "-----------------------------------------------------\n";
		ask_and_print_answer(":HORIzontal:SCALe?\n");
		ask_and_print_answer(":ACQuire:PRECision?\n");
		ask_and_print_answer(":CH1:DISPlay?\n");
		ask_and_print_answer(":CH1:COUP?\n");
		ask_and_print_answer(":CH1:OFFSet?\n");
		ask_and_print_answer(":TRIG:SING:MODE?\n");
		ask_and_print_answer(":TRIG:SING:EDGE:COUP?\n");
		ask_and_print_answer(":TRIG:SING:EDGE:SOUR?\n");
		ask_and_print_answer(":TRIGger:SINGle:EDGE:LEVel?\n");
		ask_and_print_answer(":ACQ:DEPMEM?\n");
		ask_and_print_answer(":CH1:SCAL?\n");
		cout << endl << "OWON timebase, ns is: " << get_timebase_ns() << endl;
	}

	// пойдет
	int signal::SignalDeviceOscilloscopeOWON6102A::ask_and_print_answer(ViConstString inquire)
	{
		char buffer[256];
		ViUInt32 bytes_read;
		ViStatus status;

		viPrintf(DEVICE, inquire);
		status = viRead(DEVICE, (ViBuf)buffer, 255, &bytes_read);

		if (status < VI_SUCCESS) {
			printf("Reading error: 0x%08X\n", status);
			buffer[bytes_read] = 0;
			cout << inquire << setw(40) << '|' << "  " << "Reading error: 0x%08X\n" << ' ' << status;
			return -1;
		}
		else {
			buffer[bytes_read] = 0;
			cout << inquire << setw(40) << '|' << "  " << buffer;
			// cout << "The ask is: " << inquire;
			// printf(" The answer is: %s\n", buffer);
			return 0;
		}
	}

	// нормально
	bool signal::SignalDeviceOscilloscopeOWON6102A::trigger() {
		char status[16];
		viQueryf(DEVICE, ":TRIGger:STATus?\n", "%s", status);
		if (strstr(status, "TRIG")) {
			return true;
		}
		else {
			return false;
		}
	}


	vector<uint16_t> signal::SignalDeviceOscilloscopeOWON6102A::getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) {
		vector<uint16_t> result(TICKS, 0);

		uint32_t startRead = uint32_t(uint32_t(CURR_DEPMEM) / 2) - uint32_t(EMPTY_TICKS);
		const string WAV_RANGE = ":WAV:RANG " + to_string(startRead) + "," + to_string(TICKS) + "\n";

		ViUInt32 bytes_read;
		unsigned char read_buf[301000];

		while (!trigger()) {		// На эту проверку уходит за 3000 осреднений 39 секунд, но она всегда + если только тригер включен
			Sleep(SLEEP_MS);
		}


		// 1. Начать чтение
		viPrintf(DEVICE, ":WAV:BEG CH1\n");
		// 2. Задать offset и size  
		viPrintf(DEVICE, WAV_RANGE.c_str());
		// 3. Читать данные
		viPrintf(DEVICE, ":WAV:FETC?\n");

		viRead(DEVICE, read_buf, sizeof(read_buf) - 1, &bytes_read);
		read_buf[bytes_read] = '\0';

		viPrintf(DEVICE, ":WAV:END\n");


		// 4. Парсинг TMC + int16... (без изменений)

		if (bytes_read < 2 || read_buf[0] != '#') throw "Wrong format data packet from oscill!"; // if answer contains less then 2 bytes or has no header
		int n_digits = read_buf[1] - '0';						// amount of digits in data bytes number
		if (bytes_read < 2 + n_digits) throw "Empty data packet from oscill!";			// if there is no data after header

		std::string len_str((char*)read_buf + 2, n_digits);		// the string of data bytes number
		size_t data_len = std::stoul(len_str);					// integer data bytes number

																//						Проверка длины пакета данных и ее четности
		if (data_len & 1) {
			throw "Wrong format data packet from oscill!";
		}
		else {
			if (uint32_t(data_len / 2) > TICKS)
				throw "Data packet longer than demanded ticks";
		}
		// Извлечение int16 из байтов (big-endian)
		const unsigned char* data_ptr = read_buf + 2 + n_digits;

		for (size_t i = 0; i < data_len; i += 2) {
			uint16_t raw16 = (data_ptr[i + 1] << 8) | (data_ptr[i]);  // 16-бит слово
			result[i / 2] = (uint16_t)(raw16 & 0x3FFF);
		}
		

		return result;
	}

	double signal::SignalDeviceOscilloscopeOWON6102A::rawTickToVolts(double signal_tick) {
		return (signal_tick - 8192)/6400;
	}

	std::string signal::SignalDeviceOscilloscopeOWON6102A::horScale_str(horScales_ns horScale) {
		switch (horScale)
		{
		case horScales_ns::ns1: return "1.0ns";
		case horScales_ns::ns2:  return "2.0ns";
		case horScales_ns::ns5:  return "5.0ns";
		case horScales_ns::ns10: return "10ns";
		case horScales_ns::ns20: return "20ns";
		case horScales_ns::ns50: return "50ns";
		case horScales_ns::ns100: return "100ns";
		case horScales_ns::ns200: return "200ns";
		case horScales_ns::ns500: return "500ns";
		case horScales_ns::us1:  return "1.0us";
		case horScales_ns::us2:  return "2.0us";
		case horScales_ns::us5:  return "5.0us";
		case horScales_ns::us10: return "10us";
		case horScales_ns::us20: return "20us";
		case horScales_ns::us50: return "50us";
		case horScales_ns::us100: return "100us";
		case horScales_ns::us200: return "200us";
		case horScales_ns::us500: return "500us";
		case horScales_ns::ms1:  return "1.0ms";
		case horScales_ns::ms2:  return "2.0ms";
		case horScales_ns::ms5:  return "5.0ms";
		case horScales_ns::ms10: return "10ms";
		case horScales_ns::ms20: return "20ms";
		case horScales_ns::ms50: return "50ms";
		case horScales_ns::ms100: return "100ms";
		case horScales_ns::ms200: return "200ms";
		case horScales_ns::ms500: return "500ms";
		case horScales_ns::s1:   return "1.0s";
		default:
			return "unknown";
			break;
		}
	}


	std::string signal::SignalDeviceOscilloscopeOWON6102A::depMem_str(depMem dpm) {
		switch (dpm)
		{
		case depMem::k1: return "1k";
			break;
		case depMem::k10: return "10k";
			break;
		case depMem::k100: return "100k";
			break;
		case depMem::m1: return "1M";
			break;
		case depMem::m10: return "10M";
			break;
		default: throw "Invalid DEPMEM value!";
			break;
		}
	}

	std::string signal::SignalDeviceOscilloscopeOWON6102A::chanDivScale_str(chanDivScale_mv chanScale) {
		switch (chanScale) {
		case chanDivScale_mv::mv2:   return "2mV";
		case chanDivScale_mv::mv5:   return "5mV";
		case chanDivScale_mv::mv10:  return "10mV";
		case chanDivScale_mv::mv20:  return "20mV";
		case chanDivScale_mv::mv50:  return "50mV";
		case chanDivScale_mv::mv100: return "100mV";
		case chanDivScale_mv::mv200: return "200mV";
		case chanDivScale_mv::mv500: return "500mV";
		case chanDivScale_mv::v1:    return "1V";
		case chanDivScale_mv::v2:    return "2V";
		case chanDivScale_mv::v5:    return "5V";
		default: return "unknown";
		}
	}

	signal::SignalDeviceOscilloscopeOWON6102A::horScales_ns signal::SignalDeviceOscilloscopeOWON6102A::find_horScale(uint64_t ns_value) {
		switch (ns_value) {
		case 1:         return horScales_ns::ns1;
		case 2:         return horScales_ns::ns2;
		case 5:         return horScales_ns::ns5;
		case 10:        return horScales_ns::ns10;
		case 20:        return horScales_ns::ns20;
		case 50:        return horScales_ns::ns50;
		case 100:       return horScales_ns::ns100;
		case 200:       return horScales_ns::ns200;
		case 500:       return horScales_ns::ns500;
		case 1000:      return horScales_ns::us1;
		case 2000:      return horScales_ns::us2;
		case 5000:      return horScales_ns::us5;
		case 10000:     return horScales_ns::us10;
		case 20000:     return horScales_ns::us20;
		case 50000:     return horScales_ns::us50;
		case 100000:    return horScales_ns::us100;
		case 200000:    return horScales_ns::us200;
		case 500000:    return horScales_ns::us500;
		case 1000000:   return horScales_ns::ms1;
		case 2000000:   return horScales_ns::ms2;
		case 5000000:   return horScales_ns::ms5;
		case 10000000:  return horScales_ns::ms10;
		case 20000000:  return horScales_ns::ms20;
		case 50000000:  return horScales_ns::ms50;
		case 100000000: return horScales_ns::ms100;
		case 200000000: return horScales_ns::ms200;
		case 500000000: return horScales_ns::ms500;
		case 1000000000ULL: return horScales_ns::s1;
		default:        return horScales_ns::us200;  // 10 нс — разумный default
		}
	}

	signal::SignalDeviceOscilloscopeOWON6102A::depMem signal::SignalDeviceOscilloscopeOWON6102A::find_depMem(uint64_t points) {
		switch (points) {
		case 1000:      return depMem::k1;
		case 10000:     return depMem::k10;
		case 100000:    return depMem::k100;
		case 1000000:   return depMem::m1;
		case 10000000:  return depMem::m10;
		default:        throw "incorrect DEPMEM VALUE!";  // 10k — разумный default
		}
	}

	signal::SignalDeviceOscilloscopeOWON6102A::chanDivScale_mv signal::SignalDeviceOscilloscopeOWON6102A::find_chanScale(uint64_t mv_value) {
		switch (mv_value) {
		case 2:     return chanDivScale_mv::mv2;
		case 5:     return chanDivScale_mv::mv5;
		case 10:    return chanDivScale_mv::mv10;
		case 20:    return chanDivScale_mv::mv20;
		case 50:    return chanDivScale_mv::mv50;
		case 100:   return chanDivScale_mv::mv100;
		case 200:   return chanDivScale_mv::mv200;
		case 500:   return chanDivScale_mv::mv500;
		case 1000:  return chanDivScale_mv::v1;
		case 2000:  return chanDivScale_mv::v2;
		case 5000:  return chanDivScale_mv::v5;
		default:    return chanDivScale_mv::mv10;  // 10mV — разумный default
		}
	}
}