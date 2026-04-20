#include "settings.h"
#include "include/json.hpp"
#include <fstream>

using json = nlohmann::json;

// Common
void to_json(json& j, const Common_settings& c) {
	j = json{
		{ "WORK_FOLDER", c.getWorkFolder() }
	};
}

void from_json(const json& j, Common_settings& c) {
	c.setWorkFolder(j.at("WORK_FOLDER").get<std::string>());
}

// Table
void to_json(json& j, const Table_settings& t) {
	j = json{
		{ "IP_ADRESS", t.getIpAdress() },
	{ "SIMULATOR", t.getSimulator() },
	{ "X_MIN",     t.getXMin() },
	{ "Y_MIN",     t.getYMin() },
	{ "X_MAX",     t.getXMax() },
	{ "Y_MAX",     t.getYMax() }
	};
}

void from_json(const json& j, Table_settings& t) {
	t.setIpAdress(j.at("IP_ADRESS").get<std::string>());
	t.setSimulator(j.at("SIMULATOR").get<bool>());
	t.setXMin(j.at("X_MIN").get<int>());
	t.setYMin(j.at("Y_MIN").get<int>());
	t.setXMax(j.at("X_MAX").get<int>());
	t.setYMax(j.at("Y_MAX").get<int>());
}

// Scan
void to_json(json& j, const Scan_settings& s) {
	j = json{
		{ 
			"LENGTH", s.getLength(),
			"NPOINTS", s.getNpoints(),
			"AVE_NUM", s.getNave()
		}
	};
}

void from_json(const json& j, Scan_settings& s) {
	s.setLength(j.at("LENGTH").get<double>());
	s.setNpoints(j.at("NPOINTS").get<int>());
	s.setNave(j.at("AVE_NUM").get<int>());
}


// Cscan
void to_json(json& j, const Cscan_settings& s) {
	j = json{
		{
			"x0", s.getX0(),
			"y0", s.getY0(),
			"Nx", s.getNx(),
			"Ny", s.getNy(),
			"Lx", s.getXl(),
			"Ly", s.getYl()
		}
	};
}

void from_json(const json& j, Cscan_settings& s) {
	s.setX0(j.at("x0").get<double>());
	s.setY0(j.at("y0").get<double>());
	s.setNx(j.at("Nx").get<size_t>());
	s.setNy(j.at("Ny").get<size_t>());
	s.setXl(j.at("Lx").get<double>());
	s.setYl(j.at("Ly").get<double>());
}


// Oscilloscope
void to_json(json& j, const Oscill_settings& o) {
	j = json{
		{
			"SIMULATOR", o.getSIMULATOR(),
			"HOR_SCALE_ms", o.getHorScale(),
			"DATA_COUP", o.getDataCoup(),
			"TRIG_MODE", o.getTrigMode(),
			"TRIG_COUP", o.getTrigCoup(),
			"TRIG_EDGE_LEVEL", o.getTrigEdgeLevel(),
			"DATA_CH_SCALE_V", o.getDataChScale_V(),
			"DEPMEM", o.getDepmem(),
			"WANTED_TICKS", o.getWantedTicks(),
			"EMPTY_TICKS", o.getEmptyTicks()
		}
	};
}

void from_json(const json& j, Oscill_settings& o) {
	o.setSIMULATOR(j.at("SIMULATOR").get<bool>());
	o.setHorScale(j.at("HOR_SCALE_ms").get<double>());
	o.setDataCoup(j.at("DATA_COUP").get<string>());
	o.setTrigMode(j.at("TRIG_MODE").get<string>());
	o.setTrigCoup(j.at("TRIG_COUP").get<string>());
	o.setTrigEdgeLevel(j.at("TRIG_EDGE_LEVEL").get<string>());
	o.setDataChScale_V(j.at("DATA_CH_SCALE_V").get<double>());
	o.setDepmem(j.at("DEPMEM").get<int32_t>());
	o.setWantedTicks(j.at("WANTED_TICKS").get<int>());
	o.setEmptyTicks(j.at("EMPTY_TICKS").get<int>());
}

// Config (вложенные классы)
void to_json(json& j, const Config& c) {
	j = json{
		{ "COMMON", c.getCommon_settings() },
	{ "TABLE",  c.getTable_settings() },
	{ "SCAN",   c.getScan_settings() },
	{"CSCAN", c.getCscan_settings()},
	{"OSCILLOSCOPE", c.getOscill_settings() }
	};
}

void from_json(const json& j, Config& c) {
	c.setCommon(j.at("COMMON").get<Common_settings>());
	c.setTable_settings(j.at("TABLE").get<Table_settings>());
	c.setScan_settings(j.at("SCAN").get<Scan_settings>());
	c.setCscan_settings(j.at("CSCAN").get<Cscan_settings>());
	c.setOscill_settings(j.at("OSCILLOSCOPE").get<Oscill_settings>());
}


// Singleton
Config& Config::instance() {
	static Config cfg;
	static bool loaded = cfg.loadFromFile();  // автозагрузка
	return cfg;
}

// Загрузка
bool Config::loadFromFile(const std::string& filename) {
	std::ifstream in(filename);
	if (!in.is_open()) {
		return false;  // файл не найден — дефолты
	}

	try {
		json j;
		in >> j;
		from_json(j, *this);
		return true;
	}
	catch (...) {
		return false;
	}
}

// Сохранение
bool Config::saveToFile(const std::string& filename) {
	try {
		json j = *this;
		std::ofstream out(filename);
		out << j.dump(4);
		return true;
	}
	catch (...) {
		return false;
	}
}