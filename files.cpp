
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
		vectorToMatFile(coord_, "coord_", matfp);
	
		// 4. time_ (1 x Nt)
		vectorToMatFile(time_, "time_", matfp);
	
		// 5. добавляем dist_step_
		numToMatFile(dist_step_, "dist_step_", matfp);

		// 6. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matrixToMatFile(points, "points", matfp);
	
		matClose(matfp);
	}


	void files::createCscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& table_points,                  // координаты X (Nx элементов)  
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

		// 5. coord_ (1 x Nx)
		matrixToMatFile(table_points, "table_points", matfp);

		// 6. time_ (1 x Nt)
		vectorToMatFile(time_, "time_", matfp);


		// 7. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matClose(matfp);
	}



	void files::createCscanPointsMat(
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
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

		// 3. time_ (1 x Nt)
		vectorToMatFile(time_, "time_", matfp);

		// 4. добавляем time_step_
		numToMatFile(time_step_, "time_step_", matfp);

		matClose(matfp);
	}




	void files::numToMatFile(const double &v, std::string name, MATFile* matfp) {
		mxArray* mx_v = mxCreateDoubleScalar(v);
		matPutVariable(matfp, name.c_str(), mx_v);
		mxDestroyArray(mx_v);
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


