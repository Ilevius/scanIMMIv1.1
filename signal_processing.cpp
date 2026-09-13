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

			double filter_t = SETTINGS.getFourier_settings().filter_t();
			double filter_x = SETTINGS.getFourier_settings().filter_x();

			for (size_t tick = 0; tick < t_n; tick++) {
				for (size_t signal = 0; signal < x_n; signal++) {
					VoltTicksCut[signal * t_n + tick] = VoltTicks[signal][tick + Tmin];
				}
			}

			xtFourier(
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
				H_im.data(),
				&filter_t,
				&filter_x
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
		std::vector<double>& freqs_Hz
	) {
		std::vector<std::vector<std::complex<double>>> unsortedWavenumbers;
		std::vector<std::complex<double>> wavenums_by_freq;
		std::complex<double> a_wavenumber;
		double an_alfa;
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();

		bool debug = false;
		std::vector<double> VoltTicksCut;
		double t0_s;

		int t_n = ts_s.size();
		int x_n = xs_mm.size();
		if (x_n > 1 && t_n > 1 && VoltTicks.size() == x_n && VoltTicks[0].size() == t_n) {

			double timeStep_s = ts_s[1] - ts_s[0];
			double xStep_mm = xs_mm[1] - xs_mm[0];


			int f_n = SETTINGS.getFourier_settings().freqs_n();
			double f_min = SETTINGS.getFourier_settings().fmin_MHz() * 1e6;
			double f_max = SETTINGS.getFourier_settings().fmax_MHz() * 1e6;
			double f_step = (f_max - f_min) / f_n;
			freqs_Hz.clear();
			for (size_t i = 0; i < f_n; i++) {
				freqs_Hz.push_back(f_min + i * f_step);
			}

			

			cut_vectorize_shrink_signal(
				VoltTicks,
				timeStep_s,
				x_n,
				VoltTicksCut,
				t_n,
				t0_s,
				2
			);


			int lambda = int(SETTINGS.getFourier_settings().LAMBDA_K()*x_n);
			int mu = SETTINGS.getFourier_settings().MU();
			double delta = SETTINGS.getFourier_settings().DELTA();
			double filter_t = SETTINGS.getFourier_settings().filter_t();

			std::vector<double> H_re(lambda * f_n, 0);
			std::vector<double> H_im(lambda * f_n, 0);


			MPMtF(
				&x_n,
				&f_n,
				&t_n,
				&xs_mm[0],
				&f_min,
				&t0_s,
				&xStep_mm,
				&f_step,
				&timeStep_s,
				&lambda,
				&delta,
				&filter_t,
				VoltTicksCut.data(),
				H_re.data(),
				H_im.data(),
				&mu,
				&debug
			);


			//			Переводим в комплексные числа и формируем массив wavenumbers_by_alfa для каждого альфа
			for (size_t i = 0; i < f_n; i++) {
				wavenums_by_freq.clear();
				for (size_t j = 0; j < lambda; j++) {
					a_wavenumber = std::complex<double>(H_re[i * lambda + j], H_im[i * lambda + j]);
					if (std::abs(a_wavenumber) > 1e-16) {
						an_alfa = std::arg(a_wavenumber) / xStep_mm;
						wavenums_by_freq.push_back(std::complex<double>(an_alfa, 0.0));
					}	
				}
				unsortedWavenumbers.push_back(wavenums_by_freq);
			}	

			return unsortedWavenumbers;
		}
	}



	std::vector<std::vector<std::complex<double>>> getMPMfreqs(
		std::vector<double>& ts_s,
		std::vector<double>& xs_mm,
		std::vector<std::vector<double>>& VoltTicks,
		std::vector<double>& alfas_dptr
	) {
		std::vector<std::vector<std::complex<double>>> unsortedFreqs;
		std::vector<std::complex<double>> freqs_by_alfa;
		std::complex<double> a_frequency;
		double freq;
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		bool debug = false;
		std::vector<double> VoltTicksCut;
		double t0_s;

		int t_n = ts_s.size();
		int x_n = xs_mm.size();
		if (x_n > 1 && t_n > 1 && VoltTicks.size() == x_n && VoltTicks[0].size() == t_n) {

			// Alfas array
			int alfa_n = SETTINGS.getFourier_settings().alfa_n();
			double alfaMin = SETTINGS.getFourier_settings().alfa_min_dptr();
			double alfaStep = SETTINGS.getFourier_settings().alfa_step_dptr();
			alfas_dptr.clear();
			for (size_t i = 0; i < alfa_n; i++) {
				alfas_dptr.push_back(alfaMin + i * alfaStep);
			}

			double timeStep_s = ts_s[1] - ts_s[0];
			double xStep_mm = xs_mm[1] - xs_mm[0];

			double max_delata_t_s = 1.0/(2.0*SETTINGS.getFourier_settings().fmax_MHz() * 1e6);
			int divider = std::max(1, int(0.8 * max_delata_t_s/timeStep_s));


			cut_vectorize_shrink_signal(
				VoltTicks,
				timeStep_s,
				x_n,
				VoltTicksCut,
				t_n,
				t0_s,
				divider
			);


			int lambda = int(SETTINGS.getFourier_settings().LAMBDA_K() * t_n);
			int mu = SETTINGS.getFourier_settings().MU();
			double delta = SETTINGS.getFourier_settings().DELTA();
			double filter_x = SETTINGS.getFourier_settings().filter_x();
			
			std::vector<double> H_re(lambda * alfa_n, 0);
			std::vector<double> H_im(lambda * alfa_n, 0);
			
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
				&filter_x,
				VoltTicksCut.data(),
				H_re.data(),
				H_im.data(),
				&mu,
				&debug
			);

			//			Переводим в комплексные числа и формируем массив wavenumbers_by_alfa для каждого альфа
			for (size_t i = 0; i < alfa_n; i++) {
				freqs_by_alfa.clear();
				for (size_t j = 0; j < lambda; j++) {
					a_frequency = std::complex<double>(H_re[i * lambda + j], H_im[i * lambda + j]);
					if (std::abs(a_frequency) > 1e-10) {
						freq = std::arg(a_frequency) / (2.0 * std::numbers::pi * timeStep_s);
						freqs_by_alfa.push_back(std::complex<double>(freq, 0.0));
					}
				}
				unsortedFreqs.push_back(freqs_by_alfa);
			}

			return unsortedFreqs;
		}
	}



	void cut_vectorize_shrink_signal(
		std::vector<std::vector<double>>& VoltTicks,
		double & timeStep_s,
		int & x_n,
		std::vector<double> & VoltTicksCut,
		int & t_n,
		double & t0_s,
		int divider
	) {
		auto& SETTINGS = Config::instance();
		SETTINGS.loadFromFile();
		int t_n_orig = VoltTicks[0].size();
		double ave;

		//										Обрезка данных
		size_t Tmin = size_t(SETTINGS.getFourier_settings().head_ms() * 1e-3 / timeStep_s);
		size_t Tmax = size_t(SETTINGS.getFourier_settings().tail_ms() * 1e-3 / timeStep_s);

		if (Tmin > t_n || Tmax > t_n) {
			Tmin = 0; Tmax = t_n;
		}


		t_n_orig = Tmax - Tmin;
		timeStep_s = timeStep_s * divider;
		t0_s = Tmin * timeStep_s;

		t_n = floor(t_n_orig / divider);


		
		
		VoltTicksCut.clear();
		VoltTicksCut.resize(x_n * t_n);

		for (size_t tick = 0; tick < t_n; tick++) {
			for (size_t signal = 0; signal < x_n; signal++) {
				ave = 0;
				for (size_t k = 0; k < divider; k++) {
					ave += VoltTicks[signal][tick * divider + k + Tmin];
				}
				VoltTicksCut[signal * t_n + tick] = ave / divider;
			}
		}
	}
}