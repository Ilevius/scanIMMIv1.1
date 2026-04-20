#include "../signal.h"
#include <vector>
#pragma once


namespace signal {
	class SignalDeviceOscilloscope : public SignalDevice {
		/*  у любого осциллографа есть возможность использовать тригер, есть возможность получить напряжение в Вольтах*/
	protected:
		//												Тригеры будут разные, придется переопределять
		double timebase_ns = 0;

		virtual std::vector<uint16_t> getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) = 0;

		virtual double rawTickToVolts(double signal_tick) = 0;

	public:
		virtual ~SignalDeviceOscilloscope() = default;  // Виртуальный деструктор
														// после сложения целые данные надо поделить на число осреднений и перевести в вольты согласно настройкам осциллографа
		double get_timebase_ns() { return timebase_ns; }
		void set_timebase_ns(double v) { timebase_ns = v; }

		//					Перплексити говорит так быстрее в 100 раз, надо проверить
		std::vector<double> getAveragedVolts(const uint16_t EMPTY_TICKS, const uint32_t TICKS, const uint16_t SLEEP_MS, const uint16_t NAVE);

	};
}
