#include "signal_processing.h"
#include "math.h"

namespace signalProcessing{
	void centerSignal(std::vector<double>& signal) {
		const double signal_mean = math::vectorMean(signal);
		math::subtractFromVector(signal, signal_mean);
	}

	void cutSignal(int LEFT_NULLS, int LEFT_SLOPE, int RIGHT_SLOPE, int RIGHT_NULLS, std::vector<double>& signal) {
		if (LEFT_NULLS + LEFT_SLOPE + RIGHT_SLOPE + RIGHT_NULLS >= signal.size()) {
			//throw exception
		}
		else {
			// signal cut
		}
	}
}