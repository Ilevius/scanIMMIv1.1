#include <vector>

#pragma once

namespace signalProcessing {
	void centerSignal(std::vector<double>& signal);
	void cutSignal(int LEFT_NULLS, int LEFT_SLOPE, int RIGHT_SLOPE, int RIGHT_NULLS, std::vector<double>& signal);
}
