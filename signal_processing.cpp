#include "signal_processing.h"
#include "include/MPM/MPM.h"


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


	Eigen::MatrixXcd HfuncFromBscanFortran(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& freqs_Hz,
		std::vector<double>& alfas_dptr) {
		// Функция, которая принимает векторы отсчетов по времени и по расстоянию, матрицу отсчетов напряжения, 
		// достает параметры преобразования Фурье из файла настроек и обрезает данные 
		// делает двойное преобразование Фурье и возвращает Н-функцию
		
		Eigen::MatrixXcd H(freqs_Hz.size(), alfas_dptr.size());
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();

		int t_n = ts_s.size();
		int x_n = xs_mm.size();
		if (x_n > 1 && t_n > 1 && VoltTicks.size() == x_n && VoltTicks[0].size() == t_n) {

			//											Подготовка вспомогательных данных
			double timeStep_s = ts_s[1] - ts_s[0];
			double xStep_mm = xs_mm[1] - xs_mm[0];

			int Nfreqs = SETTINGS.getFourier_settings().freqs_n();
			double Fmin_Hz = SETTINGS.getFourier_settings().fmin_MHz() * 1e6;
			double Fmax_Hz = SETTINGS.getFourier_settings().fmax_MHz() * 1e6;
			double Fstep_Hz = (Fmax_Hz - Fmin_Hz) / Nfreqs;

			int alfa_n = SETTINGS.getFourier_settings().alfa_n();
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

			std::vector<double> VoltTicksCut(x_n * t_n, 0);
			std::vector<double> H_re(freqs_Hz.size() * alfas_dptr.size(), 0);
			std::vector<double> H_im(freqs_Hz.size() * alfas_dptr.size(), 0);

			int lambda = int(t_n/12*5);
			int mu = 4;
			double delta = 0.1;

			for (size_t tick = 0; tick < t_n; tick++) {
				for (size_t signal = 0; signal < x_n; signal++) {
					VoltTicksCut[signal * t_n + tick] = VoltTicks[signal][tick + Tmin];
				}
			}

			txFourier(
				&t_n,
				&Nfreqs,
				&x_n,
				&alfa_n,
				&t0_s,
				&Fmin_Hz,
				&xs_mm[0],
				&alfaMin,
				&timeStep_s,
				&Fstep_Hz,
				&xStep_mm,
				&alfaStep,
				VoltTicksCut.data(),
				H_re.data(),
				H_im.data()
			);


			//Eigen::MatrixXcd H = math::xtFourier(t_n, Nfreqs, x_n, alfa_n, t0_s, Fmin_Hz, xs_mm[0], alfaMin, timeStep_s, Fstep_Hz, xStep_mm, alfaStep, VoltTicksCut);

			Eigen::MatrixXcd H(Nfreqs, alfa_n);
			for (int i = 0; i < Nfreqs; i++) {
				for (int j = 0; j < alfa_n; j++) {
					H(i, j) = std::complex<double>(H_re[i * alfa_n + j], H_im[i * alfa_n + j]);
			}
			
		}
		return H;
	}
		else {
			throw "Некорректные данные в В-скане";
		}




	}



	std::vector<std::vector<std::complex<double>>> getMPMwavenumbers(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& alfas_dptr
	) {
		std::vector<std::vector<std::complex<double>>> unsortedWavenumbers;
		std::vector<std::complex<double>> wavenums_by_alfa;
		std::complex<double> a_wavenumber;
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();

		int t_n = ts_s.size();
		int x_n = xs_mm.size();
		if (x_n > 1 && t_n > 1 && VoltTicks.size() == x_n && VoltTicks[0].size() == t_n) {

			double timeStep_s = ts_s[1] - ts_s[0];
			double xStep_mm = xs_mm[1] - xs_mm[0];


			int alfa_n = SETTINGS.getFourier_settings().alfa_n();
			double alfaMin = SETTINGS.getFourier_settings().alfa_min_dptr();
			double alfaStep = SETTINGS.getFourier_settings().alfa_step_dptr();

			alfas_dptr.clear();
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

			int lambda = int(t_n / 12 * 5);
			int mu = 4;
			double delta = 0.1;

			std::vector<double> VoltTicksCut(x_n * t_n, 0);
			std::vector<double> H_re(lambda * alfas_dptr.size(), 0);
			std::vector<double> H_im(lambda * alfas_dptr.size(), 0);

			

			for (size_t tick = 0; tick < t_n; tick++) {
				for (size_t signal = 0; signal < x_n; signal++) {
					VoltTicksCut[signal * t_n + tick] = VoltTicks[signal][tick + Tmin];
				}
			}

			MPMxF(
				&x_n,
				&alfa_n,
				&t_n,
				&xs_mm[0],
				&alfaMin,
				&t0_s,
				&xStep_mm,
				&alfaStep,
				&timeStep_s,
				&lambda,
				&delta,
				VoltTicksCut.data(),
				H_re.data(),
				H_im.data(),
				&mu
			);

			//			Переводим в комплексные числа и формируем массив wavenumbers_by_alfa для каждого альфа
			for (size_t i = 0; i < alfas_dptr.size(); i++) {
				wavenums_by_alfa.clear();
				for (size_t j = 0; j < lambda; j++) {
					a_wavenumber = std::complex<double>(H_re[i * lambda + j], H_im[i * lambda + j]);
					if (std::abs(a_wavenumber) > 1e-10) {
						wavenums_by_alfa.push_back(a_wavenumber);
					}	
				}
				unsortedWavenumbers.push_back(wavenums_by_alfa);
			}	

			return unsortedWavenumbers;
		}
	}
}