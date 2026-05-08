#include <vector>
#include <random>
#include <numbers>
#include "include/Eigen/Eigen"
namespace math {


	double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b);

	//			Получение шага dist_step и координат points npoints точек лежащих на отрезке с концими start, finish
	std::pair<std::vector<std::vector<double>>, double> getInterpolatedPoints(
		const std::vector<double>& start,
		const std::vector<double>& finish,
		int npoints);

	double vectorMean(const std::vector<double>& v);

	void subtractFromVector(std::vector<double>& vec, const double value);


	double triangleArea2D(const std::vector<double>& a,
		const std::vector<double>& b,
		const std::vector<double>& c);

	// равномерная точка внутри треугольника ABC (2D)
	std::vector<double> randomPointInTriangle(
		const std::vector<double>& A,
		const std::vector<double>& B,
		const std::vector<double>& C,
		std::mt19937& gen,
		std::uniform_real_distribution<double>& dist);

	std::vector<std::vector<double>> getRandomQuadrogonPoints(
		const std::vector<double>& first,
		const std::vector<double>& second,
		const std::vector<double>& third,
		const std::vector<double>& fourth,
		int npoints);


	void normalizeVector(std::vector<double>& vec);

	void scaleVector(std::vector<double>& vec, double scalar);

	double dotProduct(const std::vector<double>& a, const std::vector<double>& b);

	std::vector<std::vector<double>> ThreePointsToTransMatrix(std::vector<double>& origin, std::vector<double>& xPoint, std::vector<double>& yPoint);

	std::vector<double> matVecMult(const std::vector<double>& v, const std::vector<std::vector<double>>& m);

	std::vector<std::vector<double>> rectSnake(double& x0, double& y0, double& xl, double& yl, size_t& Nx, size_t& Ny);

	std::vector<double> vectorAdd(std::vector<double> &a, std::vector<double> &b);
	std::vector<double> vectorSubstraction(std::vector<double> &a, std::vector<double> &b);

	Eigen::MatrixXcd splineSpectrum(
		std::vector<double>& SecondTicks, std::vector<std::vector<double>>& VoltTicks, std::vector<double>& HerzTicks
	);

	std::vector<std::complex<double>> splineSpectum(
		std::vector<double>& SecondTicks, std::vector<double>& VoltTicks, std::vector<double>& HerzTicks
	);
};
