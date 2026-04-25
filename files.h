#include <vector>
#include "mat.h"
#pragma once


namespace files {
	void saveSignalToTxt(const std::vector<double>& waveform, const double timebase_s, const std::string& filename);

	void saveAscanToMat(std::vector<double>& point, std::vector<double>& data, double timebase_s, const std::string& filename);

	void createBscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<double>& coord_,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double dist_step_,							// длинна вектора между каждой парой последовательных точек
		const double time_step_,
		const std::string& filename);


	void createCscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& table_points,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double time_step_,
		const std::string& filename);



	void createCscanPointsMat(
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double time_step_,
		const std::string& filename);


	void numToMatFile(const double &v, std::string name, MATFile* matfp);

	void vectorToMatFile(const std::vector<double>& v, std::string name, MATFile* matfp);

	void matrixToMatFile(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp);

	void matrixToMatFileNorm(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp);

}
