#include <vector>
#include "mat.h"
#include <filesystem>
#include <complex>
#include "include/Eigen/Eigen"
#pragma once


namespace files {
	void saveSignalToTxt(const std::vector<double>& waveform, const double timebase_s, const std::string& filename);

	void saveAscanToMat(std::vector<double>& point, std::vector<double>& data, double timebase_s, const std::string& filename);

	void createBscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<double>& coord_,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double dist_step_,							// длинна вектора между каждой парой последовательных точек
		const double time_step_,
		const std::vector<std::vector<double>>& points,
		const std::string& filename);


	void createCscanMat(const std::vector<std::vector<double>>& data,      // Nx x Nt (Nx строк замеров)
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const std::vector<double>& time_,                   // время (Nt элементов)
		const double time_step_,
		const std::string& filename);



	void createCscanPointsMat(
		const std::vector<std::vector<double>>& basePoints,                  // координаты X (Nx элементов)  
		const std::vector<std::vector<double>>& scanPoints,                  // координаты X (Nx элементов)  
		const double time_step_,
		const std::string& filename);

	void RscanToMat(
		const std::vector<std::vector<double>>& scanPoints,
		const std::vector<std::vector<double>>& Volt_ticks,
		const double& average_n,
		const std::string& filename
	);

	void spectrumToMatFile(
		const std::vector<double>& fs, 
		const std::vector<std::complex<double>>& spec, 
		const std::vector<double> &sec_ticks,
		const std::vector<double> &Volt_ticks,
		const std::vector<double> &Volt_ticks_cut,
		const std::string &filename
		);

	void spectrumToMatFile(
		const std::vector<double> &fs,
		const std::vector<double> &alfas,
		const Eigen::MatrixXcd& spec,
		const std::string& filename
	);

	void spectrumToMatFile(
		const std::vector<double>& fs, 
		const Eigen::MatrixXcd& spec, 
		const std::vector<double> &sec_ticks,
		const std::vector<double> &Volt_ticks,
		const std::vector<double> &Volt_ticks_cut,
		const std::string &filename
		);

	void numToMatFile(const double &v, std::string name, MATFile* matfp);

	void vectorToMatFile(const std::vector<double>& v, std::string name, MATFile* matfp);

	void matrixToMatFile(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp);

	void matrixToMatFile(const Eigen::MatrixXcd & v, std::string name, MATFile* matfp);

	void matrixToMatFileNorm(const std::vector<std::vector<double>> & v, std::string name, MATFile* matfp);

	void vectorToMatFile(const std::vector<std::complex<double>>& v, std::string name, MATFile* matfp);

	bool ensureDirectoryExists(const std::string& path);

}
