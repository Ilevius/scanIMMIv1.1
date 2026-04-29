#pragma once
#include <string>
#include "include/json.hpp"
using namespace std;


class Common_settings {
public:
	Common_settings() = default;

	const std::string& getWorkFolder() const { return work_folder_; }
	void setWorkFolder(const std::string& v) { work_folder_ = v; }


private:
	std::string work_folder_;
};

class Table_settings {
public:
	Table_settings() = default;

	const std::string& getIpAdress() const { return ip_adress_; }
	void setIpAdress(const std::string& v) { ip_adress_ = v; }

	bool getSimulator() const { return simulator_; }
	void setSimulator(bool v) { simulator_ = v; }


	int getXMin() const { return x_min_; }
	void setXMin(int v) { x_min_ = v; }

	int getYMin() const { return y_min_; }
	void setYMin(int v) { y_min_ = v; }

	int getXMax() const { return x_max_; }
	void setXMax(int v) { x_max_ = v; }

	int getYMax() const { return y_max_; }
	void setYMax(int v) { y_max_ = v; }


private:
	std::string ip_adress_;
	bool simulator_ = false;
	int  x_min_ = 0;
	int  y_min_ = 0;
	int  x_max_ = 0;
	int  y_max_ = 0;
};

class Ascan_settings {
public:
	Ascan_settings() = default;
	double getX() const { return x_; }
	void setX(double v) { x_ = v; }
	double getY() const { return y_; }
	void setY(double v) { y_ = v; }
private:
	double x_ = 0.0;
	double y_ = 0.0;
};

class Bscan_settings {
public:
	Bscan_settings() = default;
	double x_start() const { return x_start_; }
	double y_start() const { return y_start_; }
	double x_finish() const { return x_finish_; }
	double y_finish() const { return y_finish_; }
	int16_t points_n() const { return points_n_; }

	// Сеттеры
	void set_x_start(double value) { x_start_ = value; }
	void set_y_start(double value) { y_start_ = value; }
	void set_x_finish(double value) { x_finish_ = value; }
	void set_y_finish(double value) { y_finish_ = value; }

	void set_points_n(int16_t value) {
		if (value > 0) {
			points_n_ = value;
		}
	}
private:
	double x_start_ = 0.0;
	double y_start_ = 0.0;
	double x_finish_ = 50.0;
	double y_finish_ = 50.0;
	int16_t points_n_ = 200;
};

class Cscan_settings {
public:
	Cscan_settings() = default;
	double getX0() const { return x0_; }
	void setX0(double v) { x0_ = v; }	
	
	double getY0() const { return y0_; }
	void setY0(double v) { y0_ = v; }

	double getXl() const { return xl_; }
	void setXl(double v) { xl_ = v; }

	double getYl() const { return yl_; }
	void setYl(double v) { yl_ = v; }

	size_t getNx() const { return Nx_; }
	void setNx(size_t v) { Nx_ = v; }	
	
	size_t getNy() const { return Ny_; }
	void setNy(size_t v) { Ny_ = v; }

private:
	double x0_, y0_, xl_, yl_;
	size_t Nx_, Ny_;
};

class Rscan_settings {
public:
	Rscan_settings() = default;
	int16_t points_n() const { return points_n_; }
	void setPoints_n(const int16_t v) { points_n_ = v; }
private:
	int16_t points_n_ = 20;
};

class Oscan_settings {
public:
	// Геттеры
	double r_min() const { return r_min_; }
	double r_max() const { return r_max_; }
	double phi_min_deg() const { return phi_min_deg_; }
	double phi_max_deg() const { return phi_max_deg_; }
	int16_t r_n() const { return r_n_; }
	int16_t phi_n() const { return phi_n_; }

	// Сеттеры
	void set_r_min(double value) { r_min_ = value; }
	void set_r_max(double value) { r_max_ = value; }
	void set_phi_min_deg(double value) { phi_min_deg_ = value; }
	void set_phi_max_deg(double value) { phi_max_deg_ = value; }

	void set_r_n(int16_t value) {
		if (value > 0) {
			r_n_ = value;
		}
	}

	void set_phi_n(int16_t value) {
		if (value > 0) {
			phi_n_ = value;
		}
	}
private:
	double r_min_ = 20;
	double r_max_ = 80;
	double phi_min_deg_ = 0;
	double phi_max_deg_ = 90;
	int16_t r_n_ = 87;
	int16_t phi_n_ = 87;
};

class Oscill_settings {
public:
	Oscill_settings() = default;

	const double getHorScale() const { return horScale_ms_; }
	void setHorScale(const double& v) { horScale_ms_ = v; }	

	const bool getSIMULATOR() const { return SIMULATOR_; }
	void setSIMULATOR(const bool& v) { SIMULATOR_ = v; }	
	
	const int32_t getEmptyTicks() const { return emptyTicks_; }
	void setEmptyTicks(const int32_t& v) { emptyTicks_ = v; }

	const string getDataCoup() const { return dataCoup_; }
	void setDataCoup(const std::string& v) { dataCoup_ = v; }

	const string getTrigMode() const { return trigMode_; }
	void setTrigMode(const std::string& v) { trigMode_ = v; }

	const string getTrigCoup() const { return trigCoup_; }
	void setTrigCoup(const std::string& v) { trigCoup_ = v; }

	const string getTrigEdgeLevel() const { return trigEdgeLevel_; }
	void setTrigEdgeLevel(const std::string& v) { trigEdgeLevel_ = v; }

	const double getDataChScale_V() const { return dataChScale_V_; }
	void setDataChScale_V(const double& v) { dataChScale_V_ = v; }

	const int32_t getDepmem() const { return depmem_; }
	void setDepmem(const int32_t& v) { depmem_ = v; }

	const int getWantedTicks() const { return wantedTicks_; }
	void setWantedTicks(const int v) { wantedTicks_ = v; }

	const int16_t getAveN() const { return average_n_; }
	void setAveN(const int16_t& v) { average_n_ = v; }

	const int8_t getTimebaseCode() const { return timabase_code_; }
	void setTimebaseCode(const int8_t& v) { timabase_code_ = v; }

private:
	bool SIMULATOR_ = true;
	double horScale_ms_ = 0.2;
	string dataCoup_ = "";
	string trigMode_ = "";	
	string trigCoup_ = "";
	string trigEdgeLevel_ = "";	
	double dataChScale_V_ = 5;
	int32_t depmem_ = 100000;
	int wantedTicks_ = 20000;
	int emptyTicks_ = 3000;
	int16_t average_n_ = 100;
	int8_t timabase_code_ = 7;
};

class Config {
public:
	static Config& instance();

	bool loadFromFile(const std::string& filename = "settings.json");
	bool saveToFile(const std::string& filename = "settings.json");

	const Common_settings& getCommon_settings() const { return Common_settings_; }
	void setCommon(const Common_settings& c) { Common_settings_ = c; }

	const Table_settings& getTable_settings() const { return Table_settings_; }
	void setTable_settings(const Table_settings& t) { Table_settings_ = t; }

	const Ascan_settings& getAscan_settings() const { return Ascan_settings_; }
	void setAscan_settings(const Ascan_settings& s) { Ascan_settings_ = s; }

	const Bscan_settings& getBscan_settings() const { return Bscan_settings_; }
	void setBscan_settings(const Bscan_settings& s) { Bscan_settings_ = s; }

	const Cscan_settings& getCscan_settings() const { return Cscan_settings_; }
	void setCscan_settings(const Cscan_settings& s) { Cscan_settings_ = s; }

	const Rscan_settings& getRscan_settings() const { return Rscan_settings_; }
	void setRscan_settings(const Rscan_settings& s) { Rscan_settings_ = s; }

	const Oscan_settings& getOscan_settings() const { return Oscan_settings_; }
	void setOscan_settings(const Oscan_settings& s) { Oscan_settings_ = s; }

	const Oscill_settings& getOscill_settings() const { return Oscill_settings_; }
	void setOscill_settings(const Oscill_settings& o) { Oscill_settings_ = o; }

private:
	Config() = default;
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	Common_settings Common_settings_;
	Table_settings Table_settings_;
	Ascan_settings Ascan_settings_;
	Bscan_settings Bscan_settings_;
	Cscan_settings Cscan_settings_;
	Rscan_settings Rscan_settings_;
	Oscan_settings Oscan_settings_;
	Oscill_settings Oscill_settings_;
};


// только объявления функций конверсии
	void to_json(nlohmann::json& j, const Config& c);
	void from_json(const nlohmann::json& j, Config& c);

	void to_json(nlohmann::json& j, const Table_settings& t);
	void from_json(const nlohmann::json& j, Table_settings& t);

	void to_json(nlohmann::json& j, const Oscill_settings& o);
	void from_json(const nlohmann::json& j, Oscill_settings& o);

	void to_json(nlohmann::json& j, const Common_settings& c);
	void from_json(const nlohmann::json& j, Common_settings& c);

	void to_json(nlohmann::json& j, const Ascan_settings& s);
	void from_json(const nlohmann::json& j, Ascan_settings& s);

	void to_json(nlohmann::json& j, const Bscan_settings& s);
	void from_json(const nlohmann::json& j, Bscan_settings& s);

	void to_json(nlohmann::json& j, const Cscan_settings& c);
	void from_json(const nlohmann::json& j, Cscan_settings& c);

	void to_json(nlohmann::json& j, const Rscan_settings& c);
	void from_json(const nlohmann::json& j, Rscan_settings& c);

	void to_json(nlohmann::json& j, const Oscan_settings& c);
	void from_json(const nlohmann::json& j, Oscan_settings& c);