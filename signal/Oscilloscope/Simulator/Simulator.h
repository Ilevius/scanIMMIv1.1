#include "../SignalDeviceOscilloscope.h"

namespace signal {
	class SignalDeviceOscilloscopeSIMULATORowon : public SignalDeviceOscilloscope {
	public:
		~SignalDeviceOscilloscopeSIMULATORowon() = default;
		void connect() override;
		void disconnect()  override;
		void setup()  override;
		std::vector<uint16_t> getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) override;
		double rawTickToVolts(double signal_tick) override;
	};
}
