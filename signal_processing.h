#include <vector>
#include "math.h"
#include "settings.h"
#include <complex>

#pragma once

namespace signalProcessing {
	void centerSignal(std::vector<double>& signal);
	void cutSignal(int LEFT_NULLS, int LEFT_SLOPE, int RIGHT_SLOPE, int RIGHT_NULLS, std::vector<double>& signal);
	Eigen::MatrixXcd HfuncFromBscan(
		std::vector<double> &ts_s, 
		std::vector<double> &xs_mm, 
		std::vector<std::vector<double>> &VoltTicks, 
		std::vector<double>& freqs_Hz, 
		std::vector<double>& alfas_dptr);

	Eigen::MatrixXcd HfuncFromBscanFortran(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& freqs_Hz,
		std::vector<double>& alfas_dptr
	);

	std::vector<std::vector<std::complex<double>>> getMPMwavenumbers(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& freqs_Hz
	);

	std::vector<std::vector<std::complex<double>>> getMPMfreqs(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& alfas_dptr
	);

	void cut_vectorize_shrink_signal(
		std::vector<std::vector<double>>& VoltTicks,
		double & timeStep_s,
		int& x_n,
		std::vector<double>& VoltTicksCut,
		int& t_n,
		double& t0_s, 
		int divider
	);
}
