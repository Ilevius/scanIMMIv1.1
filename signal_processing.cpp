#include "signal_processing.h"


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

	Eigen::MatrixXcd HfuncFromBscan(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& freqs_Hz,
		std::vector<double>& alfas_dptr) {
		// Функция, которая принимает векторы отсчетов по времени и по расстоянию, матрицу отсчетов напряжения, 
		// достает параметры преобразования Фурье из файла настроек и обрезает данные 
		// делает двойное преобразование Фурье и возвращает Н-функцию
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();

		size_t t_n = ts_s.size();
		size_t x_n = xs_mm.size();
		if (x_n > 1 && t_n > 1 && VoltTicks.size() == x_n && VoltTicks[0].size() == t_n) {

			//											Подготовка вспомогательных данных
			double timeStep_s = ts_s[1] - ts_s[0];
			double xStep_mm = xs_mm[1] - xs_mm[0];

			size_t Nfreqs = SETTINGS.getFourier_settings().freqs_n();
			double Fmin_Hz = SETTINGS.getFourier_settings().fmin_MHz() * 1e6;
			double Fmax_Hz = SETTINGS.getFourier_settings().fmax_MHz() * 1e6;
			double Fstep_Hz = (Fmax_Hz - Fmin_Hz) / Nfreqs;

			size_t alfa_n = SETTINGS.getFourier_settings().alfa_n();
			double alfaMin = SETTINGS.getFourier_settings().alfa_min_dptr();
			double alfaStep = SETTINGS.getFourier_settings().alfa_step_dptr();


			freqs_Hz.clear(); alfas_dptr.clear();
			for (size_t i = 0; i < Nfreqs; i++) {
				freqs_Hz.push_back(Fmin_Hz + i * Fstep_Hz);
			}
			for (size_t i = 0; i < alfa_n; i++) {
				alfas_dptr.push_back(alfaMin + i * alfaStep);
			}

			//										Обрезка данных
			size_t Tmin = size_t(SETTINGS.getFourier_settings().head_ms() * 1e-3 / timeStep_s);
			size_t Tmax = size_t(SETTINGS.getFourier_settings().tail_ms() * 1e-3 / timeStep_s);
			if (Tmin > t_n || Tmax > t_n) {
				Tmin = 0; Tmax = t_n;
			}
			t_n = Tmax - Tmin;
			double t0_s = Tmin * timeStep_s;

			std::vector<std::vector<double>> VoltTicksCut(x_n, std::vector<double>(t_n, 0));

			for (size_t tick = 0; tick < t_n; tick++) {
				for (size_t signal = 0; signal < x_n; signal++) {
					VoltTicksCut[signal][tick] = VoltTicks[signal][tick+Tmin];
				}
			}


			Eigen::MatrixXcd H = math::xtFourier(t_n, Nfreqs, x_n, alfa_n, t0_s, Fmin_Hz, xs_mm[0], alfaMin, timeStep_s, Fstep_Hz, xStep_mm, alfaStep, VoltTicksCut);

			return H;
		}
		else {
			throw "Некорректные данные в В-скане";
		}




	}

}