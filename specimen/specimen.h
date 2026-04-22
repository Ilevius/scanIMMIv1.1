#include <string>
#pragma once

namespace specimen {
	class Specimen {
	public:
		std::string name = "unnamed plate";
		// border_demph
		// surface : polished/tape
	};

	class Plate : public Specimen {
	public:
		double width_mm = 0;
		double length_mm = 0;
		double height_mm = 0;
		double rho_kg_m3 = 0;
	};

}
