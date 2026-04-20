#include <Windows.h>
#include "SignalDeviceOscilloscope.h"

namespace signal {
	//																	SignalDeviceOscilloscope

	std::vector<double> SignalDeviceOscilloscope::getAveragedVolts(const uint16_t EMPTY_TICKS, const uint32_t TICKS,
		const uint16_t SLEEP_MS, const uint16_t NAVE) {


		//MMRESULT res = timeBeginPeriod(1);   // запросить 1 мс 
		//if (res != TIMERR_NOERROR) {
		//	// обработка ошибки
		//}


		std::vector<uint64_t> signals_sum(TICKS, 0);
		for (size_t i = 0; i < NAVE; i++) {
			auto signal = getRaw16BitSignal(SLEEP_MS, EMPTY_TICKS, TICKS);
			for (size_t j = 0; j < TICKS; j++) {
				signals_sum[j] += signal[j];
			}
		}

		//timeEndPeriod(1);

		std::vector<double> result(TICKS);
		for (size_t j = 0; j < TICKS; j++) {
			result[j] = rawTickToVolts(static_cast<double>(signals_sum[j]) / NAVE);
		}
		return result;
	}

}