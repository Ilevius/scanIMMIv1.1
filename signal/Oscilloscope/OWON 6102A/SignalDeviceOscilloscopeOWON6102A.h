#include "../SignalDeviceOscilloscope.h"
#include "../../../include/visa.h"

namespace signal {
	class SignalDeviceOscilloscopeOWON6102A : public SignalDeviceOscilloscope {
	public:
		~SignalDeviceOscilloscopeOWON6102A() = default;
		void connect() override;
		void disconnect()  override;
		void setup()  override;
		bool trigger();
		std::vector<uint16_t> getRaw16BitSignal(const uint16_t& SLEEP_MS, const uint16_t& EMPTY_TICKS, const uint32_t& TICKS) override;
		double rawTickToVolts(double signal_tick) override;

	private:
		ViSession DEVICE;
		ViSession RESOURCE_MANAGER;
		int ask_and_print_answer(ViConstString inquiry);

		enum class horScales_ns : uint64_t
		{
			ns1 = 1, ns2 = 2, ns5 = 5, ns10 = 10, ns20 = 20, ns50 = 50, ns100 = 100, ns200 = 200, ns500 = 500, us1 = 1000, us2 = 2000,
			us5 = 5000, us10 = 10000, us20 = 20000, us50 = 50000, us100 = 100000, us200 = 200000, us500 = 500000, ms1 = 1000000,
			ms2 = 2000000, ms5 = 5000000, ms10 = 10000000, ms20 = 20000000, ms50 = 50000000, ms100 = 100000000, ms200 = 200000000,
			ms500 = 500000000, s1 = 1000000000
		};

		enum class depMem :uint64_t
		{
			k1 = 1000, k10 = 10000, k100 = 100000, m1 = 1000000, m10 = 10000000
		};

		enum class chanDivScale_mv :uint64_t {
			mv2 = 2, mv5 = 5, mv10 = 10, mv20 = 20, mv50 = 50, mv100 = 100, mv200 = 200, mv500 = 500, v1 = 1000, v2 = 2000, v5 = 5000
		};

		std::string horScale_str(horScales_ns horScale);

		std::string depMem_str(depMem dpm);

		std::string chanDivScale_str(chanDivScale_mv chanScale);

		horScales_ns find_horScale(uint64_t ns_value);

		depMem find_depMem(uint64_t points);

		chanDivScale_mv find_chanScale(uint64_t mv_value);

		depMem CURR_DEPMEM = depMem::k100;
		chanDivScale_mv CURR_VOLT_SCALE = chanDivScale_mv::v5;
		horScales_ns CURR_HORSCALE = horScales_ns::us200;
	};
}