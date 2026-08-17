
#include <fstream>
#include <iomanip>
#include <vector>
#include "files.h"
#include <algorithm>
  // MATLAB Engine MAT API // Нужно помимо настроек проекта добавить еще в переменные среды Path C:\Program Files\MATLAB\R2018a\bin\win64
//													и, возможно, перезагрузить среду разработки


	void files::saveSignalToTxt(const std::vector<double>& waveform, const double timebase_s, const std::string& filename) {
		std::ofstream file(filename);
		file << std::fixed << std::setprecision(6);  // 6 знаков после запятой

		for (double v : waveform) {
			file << v << '\n';
		}
		file.close();
	}


	void files::writeWaveNumbersToTxt(
		const std::vector<double>& alfas,
		const std::vector<std::vector<std::complex<double>>>& waveNumbers,
		const std::string& filename)
	{
		// Проверка на соответствие размеров
		if (alfas.size() != waveNumbers.size()) {
			throw std::runtime_error("Размеры alfas и waveNumbers не совпадают!");
		}

		std::ofstream outFile(filename);
		if (!outFile.is_open()) {
			throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
		}

		// Настройка формата вывода
		outFile << std::scientific << std::setprecision(15);

		// Заголовок (опционально)
		outFile << "% alpha\t\tRe(waveNumber)\tIm(waveNumber)\n";
		outFile << "% ------------------------------------------------\n";

		// Проход по всем элементам alfas
		for (size_t i = 0; i < alfas.size(); ++i) {
			outFile << "% alpha[" << i << "] = " << alfas[i] << "\n";

			const auto& wnVec = waveNumbers[i];

			// Проход по всем элементам вектора waveNumbers[i]
			for (size_t j = 0; j < wnVec.size(); ++j) {
				outFile << alfas[i] << "\t"
					<< wnVec[j].real() << "\t"
					<< wnVec[j].imag() << "\n";
			}

			// Пустая строка между разными alfas (опционально)
			outFile << "\n";
		}

		outFile.close();
	}


	std::vector<double> files::loadSignalFromTxt(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Can't open file: " + filename);
		}

		std::vector<double> waveform;
		double value;

		while (file >> value) {
			waveform.push_back(value);
		}

		if (waveform.empty()) {
			throw std::runtime_error("File is empty or contains no valid doubles: " + filename);
		}

		return waveform;
	}

	void files::saveAscanToMat(std::vector<double>& point, std::vector<double>& data, double timebase_s, const std::string& filename) {
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		if (!point.size() || !data.size() || !timebase_s) {
			throw "Data are incorrect";
		}
		vectorToMatFile(point, "point", matfp);
		vectorToMatFile(data, "data", matfp);
		numToMatFile(timebase_s, "timebase_s", matfp);
	}

	void files:: createBscanMat(
		const std::vector<std::vector<double>>& data,
		const std::vector<double>& coord_,
		const std::vector<double>& time_,
		const double dist_step_,
		const double time_step_,
		const std::vector<std::vector<double>>& points,
		const std::string& filename) {

		std::vector<double> coord_cut;
	
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		size_t Nx = data.size();
		
		if (coord_.size() != Nx) {
			for (size_t i = 0; i < Nx; i++) {
				coord_cut.push_back(coord_[i]);
			}
		}
		else coord_cut = coord_;
	
		// 1. data (Nx x Nt)
		matrixToMatFile(data, "data", matfp);
	
		// 2. data_norm (Nx x Nt) — нормализованные данные
		matrixToMatFile(data, "data_norm", matfp);

		// 3. coord_ (1 x Nx)
		vectorToMatFile(coord_cut, "coord_", matfp);
	
		// 4. time_ (1 x Nt)
		vectorToMatFile(time_, "time_", matfp);
	
		// 5. добавляем dist_step_
		numToMatFile(dist_step_, "dist_step_", matfp);

		// 6. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matrixToMatFile(points, "points", matfp);
	
		matClose(matfp);
	}

	files::ScanHeader files::ScanHeaderFromMatFile(const std::string filename) {
		ScanHeader theScanHeader;

		MATFile* matfp = matOpen(filename.c_str(), "r");
		if (!matfp) {
			throw "Error! Can't open mat file for reading: " + filename;
		}

		try {
			theScanHeader.basePoints = matrixFromMatFile("basePoints", matfp);
			theScanHeader.scanPoints = matrixFromMatFile("scanPoints", matfp);
			theScanHeader.time_step_ = numFromMatFile("time_step_", matfp);
		}
		catch (...) {
			throw "Error! Can't find some important data in mat file: " + filename;
		}
		return theScanHeader;
	}

	files::BscanCoreData files::BscanFromFile(const std::string filename) {

		std::vector<double> xs, ts;
		std::vector<std::vector<double>> data, data_norm;
		files::BscanCoreData result;

		MATFile* matfp = matOpen(filename.c_str(), "r");
		if (!matfp) {
			throw "Can't open mat file for reading: " + filename;
		}

		try {
			// hotfix hardcode for old bad B-scans
			//for (size_t i = 0; i < 15000; i++) {
			//	ts.push_back(i * 16 * 0.000000001);
			//}
			// end hardcode

			vectorFromMatFile(xs, "coord_", matfp);
			vectorFromMatFile(ts, "time_", matfp);
			data = matrixFromMatFile("data", matfp);
			data_norm = matrixFromMatFile("data_norm", matfp);
			result.ts = ts;
			result.xs = xs;
			result.data = data;
			result.data_norm = data_norm;
			return result;
		}
		catch (...) {
			matClose(matfp);
			throw "Cant save data to mat file";
		}

		matClose(matfp);

	}

	void files::createCscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double time_step_,
		const std::string& filename) {

		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		size_t Nx = data.size();
		size_t Nt = (Nx > 0) ? data[0].size() : 0;

		// 1. data (Nx x Nt)
		matrixToMatFile(data, "data", matfp);

		// 2. data_norm (Nx x Nt) — нормализованные данные
		matrixToMatFile(data, "data_norm", matfp);

		// 3. coord_ (1 x Nx)
		matrixToMatFile(basePoints, "basePoints", matfp);

		// 4. coord_ (1 x Nx)
		matrixToMatFile(scanPoints, "scanPoints", matfp);

		// 5. time_ (1 x Nt)
		vectorToMatFile(time_, "time_", matfp);


		// 6. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matClose(matfp);
	}



	void files::createCscanPointsMat(
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const double time_step_,
		const std::string& filename) {

		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		
		
		// 1. coord_ (1 x Nx)
		matrixToMatFile(basePoints, "basePoints", matfp);

		// 2. coord_ (1 x Nx)
		matrixToMatFile(scanPoints, "scanPoints", matfp);

		// 3. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matClose(matfp);
	}


	void files::RscanToMat(
		const std::vector<std::vector<double>>& scanPoints,
		const std::vector<std::vector<double>>& Volt_ticks,
		const double& average_n,
		const std::string& filename
	) {
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}

		// 1. coord_ (1 x Nx)
		matrixToMatFile(scanPoints, "points", matfp);

		// 2. coord_ (1 x Nx)
		matrixToMatFile(Volt_ticks, "data", matfp);

		// 3. добавляем time_step_
		numToMatFile(average_n, "average_n", matfp);

		matClose(matfp);
	}


	void files::spectrumToMatFile(
		const std::vector<double>& fs,
		const std::vector<std::complex<double>>& spec,
		const std::vector<double>& sec_ticks,
		const std::vector<double>& Volt_ticks,
		const std::vector<double>& Volt_ticks_cut,
		const std::string& filename
	) {
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		
		vectorToMatFile(fs, "freqs", matfp);
		vectorToMatFile(spec, "spectrum", matfp);
		vectorToMatFile(sec_ticks, "sec_ticks", matfp);
		vectorToMatFile(Volt_ticks, "Volt_ticks", matfp);
		vectorToMatFile(Volt_ticks_cut, "Volt_ticks_cut", matfp);
	}

	void files::spectrumToMatFile(
		const std::vector<double>& fs,
		const Eigen::MatrixXcd& spec,
		const std::vector<double>& sec_ticks,
		const std::vector<double>& Volt_ticks,
		const std::vector<double>& Volt_ticks_cut,
		const std::string& filename
	) {
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		
		vectorToMatFile(fs, "freqs", matfp);
		matrixToMatFile(spec, "spectrum", matfp);
		vectorToMatFile(sec_ticks, "sec_ticks", matfp);
		vectorToMatFile(Volt_ticks, "Volt_ticks", matfp);
		vectorToMatFile(Volt_ticks_cut, "Volt_ticks_cut", matfp);
	}


	void files::spectrumToMatFile(
		const std::vector<double>& fs,
		const std::vector<double>& alfas,
		const Eigen::MatrixXcd& spec,
		const std::string& filename
	) {
		MATFile* matfp = matOpen(filename.c_str(), "w");
		if (!matfp) {
			throw "Can't create mat file!";
		}
		vectorToMatFile(fs, "freqs", matfp);
		matrixToMatFile(spec, "spectrum", matfp);
		vectorToMatFile(alfas, "sec_ticks", matfp);
	}




	//																							Data types to or from mat file

	void files::numToMatFile(const double &v, std::string name, MATFile* matfp) {
		mxArray* mx_v = mxCreateDoubleScalar(v);
		matPutVariable(matfp, name.c_str(), mx_v);
		mxDestroyArray(mx_v);
	}

	double files::numFromMatFile(const std::string& name, MATFile* matfp)
	{
		mxArray* mx_v = matGetVariable(matfp, name.c_str());
		if (!mx_v || !mxIsDouble(mx_v) || mxIsComplex(mx_v) || mxGetNumberOfElements(mx_v) != 1) {
			if (mx_v) mxDestroyArray(mx_v);
			throw std::runtime_error("Can't load double scalar " + name + " from mat file");
		}

		double value = mxGetScalar(mx_v);
		mxDestroyArray(mx_v);
		return value;
	}

	void files::vectorToMatFile(const std::vector<double>& v, std::string name, MATFile* matfp) {
		size_t len = v.size();
		if (len < 1) throw "incorrect vector for saving to mat file";

		mxArray* mx_v = mxCreateDoubleMatrix(1, len, mxREAL);
		double* v_ptr = mxGetPr(mx_v);
		for (size_t i = 0; i < v.size(); ++i) {
			v_ptr[i] = v[i];
		}
		matPutVariable(matfp, name.c_str(), mx_v);
		mxDestroyArray(mx_v);
	}

	void files::vectorFromMatFile(
		std::vector<double>& v,
		const std::string& name,
		MATFile* matfp
	) {
		mxArray* mx_v = matGetVariable(matfp, name.c_str());
		if (!mx_v || !mxIsDouble(mx_v) || mxIsComplex(mx_v) || mxGetNumberOfDimensions(mx_v) != 2) {
			throw std::runtime_error("Can't load vector " + name + " from mat file");
		}

		mwSize rows = mxGetM(mx_v);
		mwSize cols = mxGetN(mx_v);
		if (rows > 1 && cols > 1) {
			throw std::runtime_error("Vector " + name + " must be a row or column vector");
		}

		size_t len = rows * cols;
		v.resize(len);

		const double* data = mxGetPr(mx_v);
		for (size_t i = 0; i < len; ++i) {
			v[i] = data[i];
		}

		mxDestroyArray(mx_v);
	}

	void files::matrixToMatFile(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp) {
		size_t rows = v.size();
		size_t colls = v[0].size();
		if (rows < 1 || colls < 1) throw "incorrect matrix for saving to matfile";

		mxArray* mx_v = mxCreateDoubleMatrix(rows, colls, mxREAL);
		double* v_ptr = mxGetPr(mx_v);
		for (int i = 0; i < colls; ++i) {        // строка
			for (int j = 0; j < rows; ++j) {    // столбец
				v_ptr[i*rows + j ] = v[j][i];  //column-major для MATLAB
			}
		}
		matPutVariable(matfp, name.c_str(), mx_v);
		mxDestroyArray(mx_v);
	}


	std::vector<std::vector<double>> files::matrixFromMatFile(
		const std::string& name,
		MATFile* matfp
	) {
		mxArray* mx_m = matGetVariable(matfp, name.c_str());
		if (!mx_m || !mxIsDouble(mx_m) || mxIsComplex(mx_m)) {
			throw std::runtime_error("Can't load real matrix " + name + " from mat file");
		}

		mwSize rows = mxGetM(mx_m);
		mwSize cols = mxGetN(mx_m);

		std::vector<std::vector<double>> m(rows, std::vector<double>(cols));
		const double* data = mxGetPr(mx_m);  // 

		for (mwIndex j = 0; j < cols; ++j) {
			for (mwIndex i = 0; i < rows; ++i) {
				m[i][j] = data[j * rows + i];  // 
			}
		}

		mxDestroyArray(mx_m);
		return m;
	}

	void files::matrixToMatFile(const Eigen::MatrixXcd& v, std::string name, MATFile* matfp) {
		size_t rows = v.rows();
		size_t colls = v.cols();
		if (rows < 1 || colls < 1) throw "incorrect matrix for saving to matfile";

		mxArray* mx_v_r = mxCreateDoubleMatrix(rows, colls, mxREAL);
		double* v_r_ptr = mxGetPr(mx_v_r);
		for (int i = 0; i < colls; ++i) {        // строка
			for (int j = 0; j < rows; ++j) {    // столбец
				v_r_ptr[i * rows + j] = v(j,i).real();  //column-major для MATLAB
			}
		}

		mxArray* mx_v_i = mxCreateDoubleMatrix(rows, colls, mxREAL);
		double* v_i_ptr = mxGetPr(mx_v_i);
		for (int i = 0; i < colls; ++i) {        // строка
			for (int j = 0; j < rows; ++j) {    // столбец
				v_i_ptr[i * rows + j] = v(j,i).imag();  //column-major для MATLAB
			}
		}

		auto real_name = name + "Real";
		auto imag_name = name + "Imag";

		matPutVariable(matfp, real_name.c_str(), mx_v_r);
		matPutVariable(matfp, imag_name.c_str(), mx_v_i);

		mxDestroyArray(mx_v_r);
		mxDestroyArray(mx_v_i);

	}

	void files::matrixToMatFileNorm(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp) {
		size_t rows = v.size();
		size_t colls = v[0].size();
		if (rows < 1 || colls < 1) throw "incorrect matrix for saving to matfile";

		mxArray* mx_v_norm = mxCreateDoubleMatrix(rows, colls, mxREAL);
		double* norm_ptr = mxGetPr(mx_v_norm);
		for (int i = 0; i < rows; ++i) {
			double max_val = v[i][0];
			double min_val = v[i][0];

			// Поиск max/min
			for (int j = 0; j < colls; ++j) {
				if (v[i][j] > max_val) max_val = v[i][j];
				if (v[i][j] < min_val) min_val = v[i][j];
			}

			double amp = (max_val - min_val) / 2.0;
			if (amp == 0.0) amp = 1.0;  //  Защита от деления на ноль!

			for (int j = 0; j < colls; ++j) {
				norm_ptr[i*colls + j] = v[i][j] / amp;  // правильный индекс
			}
		}
		matPutVariable(matfp, name.c_str(), mx_v_norm);
		mxDestroyArray(mx_v_norm);
	}

	void files::vectorToMatFile(const std::vector<std::complex<double>>& v, std::string name, MATFile* matfp) {
		size_t len = v.size();
		if (len < 1) throw std::runtime_error("incorrect vector for saving to mat file");

		// реальный вектор
		mxArray* mx_real = mxCreateDoubleMatrix(1, len, mxREAL);
		double* v_real = mxGetPr(mx_real);
		for (size_t i = 0; i < len; ++i) v_real[i] = v[i].real();

		// мнимый вектор
		mxArray* mx_imag = mxCreateDoubleMatrix(1, len, mxREAL);
		double* v_imag = mxGetPr(mx_imag);
		for (size_t i = 0; i < len; ++i) v_imag[i] = v[i].imag();

		// сохраняем два вектора, например, как name + "Real" и name + "Imag"
		auto real_name = name + "Real";
		auto imag_name = name + "Imag";

		matPutVariable(matfp, real_name.c_str(), mx_real);
		matPutVariable(matfp, imag_name.c_str(), mx_imag);

		mxDestroyArray(mx_real);
		mxDestroyArray(mx_imag);
	}

	bool files::ensureDirectoryExists(const std::string& path) {
		namespace fs = std::filesystem;

		std::error_code ec;
		if (fs::exists(path, ec)) {
			return fs::is_directory(path, ec);
		}

		fs::create_directories(path, ec);
		return bool(ec);
	}

