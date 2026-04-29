#include "../SignalDeviceOscilloscope.h"
#include "../../../include/PicoScope/ps5000aApi.h"
#pragma comment(lib, "include/PicoScope/ps5000a.lib")
namespace signal {
	class SignalDeviceOscilloscopePicoScope5000s : public SignalDeviceOscilloscope {
	public:
		~SignalDeviceOscilloscopePicoScope5000s() = default;
		void connect() override;
		void disconnect()  override;
		void setup()  override;
		std::vector<uint16_t> getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) override;
		double rawTickToVolts(double signal_tick) override;
		double VoltScale=1;
		PS5000A_RANGE picoChanRange(double v);
		void setTimebase_code(const uint32_t& v) { timebase_code = v; }
		uint32_t getTimebase_code() { return timebase_code; }

	private:
		int16_t HANDLE = 0;
		int16_t data_buffer[150000];
		int32_t data_buffer_len = 150000;	
		uint32_t timebase_code = 4;
	};
}