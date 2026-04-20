#include <vector>
#include <numeric>
#include <random>
#include <cmath>
#include <stdexcept>
#include "math.h"

namespace math {


	double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
		double dist = 0.0;
		for (size_t i = 0; i < a.size(); ++i) {
			double diff = b[i] - a[i];
			dist += diff * diff;
		}
		return std::sqrt(dist);
	}

	//			ѕолучение шага dist_step и координат points npoints точек лежащих на отрезке с концими start, finish
	std::pair<std::vector<std::vector<double>>, double> getInterpolatedPoints(
		const std::vector<double>& start,
		const std::vector<double>& finish,
		int npoints) {

		if (npoints < 1) {
			return { {}, 0.0 };
		}

		std::vector<double> direction(start.size());
		for (size_t i = 0; i < start.size(); ++i) {
			direction[i] = finish[i] - start[i];
		}

		double dist_total = euclideanDistance(start, finish);
		double dist_step = dist_total / npoints;

		std::vector<std::vector<double>> points;
		points.reserve(npoints + 1);  // NPOINTS отрезков = NPOINTS+1 точек

		for (int k = 0; k <= npoints; ++k) {
			std::vector<double> point(start.size());
			for (size_t i = 0; i < start.size(); ++i) {
				point[i] = start[i] + (k * 1.0 / npoints) * direction[i];
			}
			points.push_back(point);
		}

		return { points, dist_step };
	}

	double vectorMean(const std::vector<double>& v)
	{
		if (v.empty())
			return 0.0;

		double sum = std::accumulate(v.begin(), v.end(), 0.0);
		return sum / v.size();
	}

	void subtractFromVector(std::vector<double>& vec, const double value) {
		for (size_t i = 0; i < vec.size(); ++i) {
			vec[i] -= value;
		}
	}

	double triangleArea2D(const std::vector<double>& a,
		const std::vector<double>& b,
		const std::vector<double>& c)
	{
		// ”двоенна€ площадь по векторному произведению
		return std::abs((b[0] - a[0]) * (c[1] - a[1]) -
			(b[1] - a[1]) * (c[0] - a[0])) * 0.5;
	}

	// равномерна€ точка внутри треугольника ABC (2D)
	std::vector<double> randomPointInTriangle(
		const std::vector<double>& A,
		const std::vector<double>& B,
		const std::vector<double>& C,
		std::mt19937& gen,
		std::uniform_real_distribution<double>& dist)
	{
		double r1 = dist(gen);
		double r2 = dist(gen);

		// трюк с корнем дл€ равномерного распределени€ по площади[web:18]
		double s1 = std::sqrt(r1);
		double u = 1.0 - s1;
		double v = s1 * (1.0 - r2);
		double w = s1 * r2;

		std::vector<double> P(2);
		P[0] = u * A[0] + v * B[0] + w * C[0];
		P[1] = u * A[1] + v * B[1] + w * C[1];
		return P;
	}

	std::vector<std::vector<double>> getRandomQuadrogonPoints(
		const std::vector<double>& first,
		const std::vector<double>& second,
		const std::vector<double>& third,
		const std::vector<double>& fourth,
		int npoints)
	{
		if (first.size() < 2 || second.size() < 2 ||
			third.size() < 2 || fourth.size() < 2)
		{
			throw std::invalid_argument("All points must be at least 2D");
		}
		if (npoints <= 0) {
			return {};
		}

		// –азбиение выпуклого четырехугольника на два треугольника:
		// T1 = (first, second, third), T2 = (first, third, fourth)[web:14][web:19]
		double area1 = triangleArea2D(first, second, third);
		double area2 = triangleArea2D(first, third, fourth);
		double totalArea = area1 + area2;
		if (totalArea == 0.0) {
			throw std::invalid_argument("Degenerate quadrilateral");
		}

		double p1 = area1 / totalArea;  // веро€тность выбирать первый треугольник

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> dist01(0.0, 1.0);

		std::vector<std::vector<double>> result;
		result.reserve(static_cast<size_t>(npoints));

		for (int i = 0; i < npoints; ++i) {
			double r = dist01(gen);
			if (r < p1) {
				// точка в треугольнике (first, second, third)
				result.push_back(randomPointInTriangle(first, second, third, gen, dist01));
			}
			else {
				// точка в треугольнике (first, third, fourth)
				result.push_back(randomPointInTriangle(first, third, fourth, gen, dist01));
			}
		}

		return result;
	}


	void normalizeVector(std::vector<double>& vec) {
		if (vec.empty()) {
			return;
		}

		double norm = 0.0;
		for (double x : vec) {
			norm += x * x;
		}
		norm = std::sqrt(norm);

		if (norm == 0.0) {
			return;
		}

		for (double& x : vec) {
			x /= norm;
		}
	}


	void scaleVector(std::vector<double>& vec, double scalar) {
		for (double& x : vec) {
			x *= scalar;
		}
	}


	double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
		if (a.size() != b.size() || a.empty()) {
			return 0.0;  // ќшибка или пустые векторы
		}

		double result = 0.0;
		for (size_t i = 0; i < a.size(); ++i) {
			result += a[i] * b[i];
		}

		return result;
	}


	std::vector<std::vector<double>> makeCscanPoints(int16_t Np, double Len_y,
		const std::vector<double>& p1, const std::vector<double>& p2,
		const std::vector<double>& p3) {

		if (p1.size() < 2 || p1.size() != p2.size() || p1.size() != p3.size() || Np <= 0) {
			throw std::invalid_argument("Incorrect input for makeCscanPoints");
		}

		size_t dim = p1.size();
		std::vector<double> x(dim, 0), y(dim, 0), dir(dim,0);

		// Ўаг по X (от p1 к p2)
		for (size_t d = 0; d < dim; d++) {
			x[d] = (p2[d] - p1[d]) / (Np - 1);  
			dir[d] = (p3[d] - p1[d]); // вектор направлени€ от первой точки до третьей
		}

		// ќртогональный Y (поворот X на 90 градусов в проскости)
		y[0] = -x[1];
		y[1] = x[0];
		if (dim > 2) std::fill(y.begin() + 2, y.end(), 0.0);

		if (dotProduct(y, dir) < 0) scaleVector(y, -1); // провер€ем смотрит ли у в сторону третьей точки,
		// если да то скал€рное произведение больше нул€

		normalizeVector(y);
		scaleVector(y, Len_y / (Np - 1));

		std::vector<std::vector<double>> points;

		//  проход по точкам сетки змейкой
		for (size_t i = 0; i < Np; i++) {
			for (size_t j = 0; j < Np; j++) {
				std::vector<double> newPoint(dim, 0);
				for (size_t d = 0; d < dim; d++) {
					if (i % 2 == 0) {
						newPoint[d] = p1[d] + x[d] * i + y[d] * j;
					}
					else {
						newPoint[d] = p1[d] + x[d] * i + y[d] * (Np-1-j);
					}
				}
				points.push_back(newPoint);
			}
		}

		return points;
	}


	std::vector<std::vector<double>> ThreePointsToTransMatrix(std::vector<double>& origin, std::vector<double>& xPoint, 
		std::vector<double>& yPoint){
		if (origin.size() < 2 || origin.size() != xPoint.size() || origin.size() != yPoint.size() ) {
			throw std::invalid_argument("Incorrect input for makeCscanPoints");
		}

		size_t dim = origin.size();
		std::vector<double> x(dim, 0), y(dim, 0), dir(dim, 0);
		std::vector<std::vector<double>> result;

		for (size_t d = 0; d < dim; d++) {
			x[d] = xPoint[d] - origin[d];
			dir[d] = yPoint[d] - origin[d]; // вектор направлени€ от первой точки до третьей
		}

		// ќртогональный Y (поворот X на 90 градусов в проскости)
		y[0] = -x[1];
		y[1] = x[0];
		if (dim > 2) std::fill(y.begin() + 2, y.end(), 0.0);

		if (dotProduct(y, dir) < 0) scaleVector(y, -1); // провер€ем смотрит ли у в сторону третьей точки,
														// если да то скал€рное произведение больше нул€
		normalizeVector(x); normalizeVector(y);
		result.push_back(x); result.push_back(y);
		return result;
	}

	std::vector<double> matVecMult(const std::vector<double>& v, const std::vector<std::vector<double>>& m) 
	{
		// ѕровер€ем размеры: v.size() == m[i].size()
		if (v.empty() || m.empty() || v.size() != m[0].size()) {
			return std::vector<double>();
		}

		const std::size_t rows = m.size();
		const std::size_t cols = v.size();

		std::vector<double> result(rows, 0.0);

		for (std::size_t i = 0; i < rows; ++i) {
			double sum = 0.0;
			for (std::size_t j = 0; j < cols; ++j) {
				sum += m[j][i] * v[j];
			}
			result[i] = sum;
		}

		return result;
	}

	std::vector<std::vector<double>> rectSnake(double& x0, double& y0, double& xl, double& yl, size_t& Nx, size_t& Ny) {
		const double dx = double(xl / Nx);
		const double dy = double(yl / Nx);
		std::vector<std::vector<double>> result;

		//  проход по точкам сетки змейкой
		for (size_t i = 0; i <= Ny; i++) {
			for (size_t j = 0; j <= Nx; j++) {
				std::vector<double> newPoint(2, 0);
					if (i % 2 == 0) {
						newPoint[0] = x0 + dx * j;
					}
					else {
						newPoint[0] = x0 + dx * (Nx - j);
					}
					newPoint[1] = y0 + dy * i;
				result.push_back(newPoint);
			}
		}
		return result;
	}

	std::vector<double> vectorAdd(std::vector<double> &a, std::vector<double> &b) {
		if (a.size() == b.size()) {
			std::vector<double> result(a.size(), 0);
			for (size_t i = 0; i < a.size(); i++) {
				result[i] = a[i] + b[i];
			}
			return result;
		}
		else {
			throw "you are trying to add differnt len vectors!";
		}
	}
};
