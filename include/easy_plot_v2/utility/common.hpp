#ifndef EASY_PLOT_2_COMMON_HPP_INCLUDED
#define EASY_PLOT_2_COMMON_HPP_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <numeric>
#include <stdlib.h>
#include <dirent.h>
#include <dir.h>

#include <nlohmann/json.hpp>

#if defined(_WIN32)
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif
#endif

#include <limits>

namespace ep2 {

	using json = nlohmann::json;

	static inline int EASY_PLOT_2_VERSION	= 4;	/**< Версия библиотеки */

	class Color {
	public:
		double r = 0;
		double g = 0;
		double b = 0;

		Color() {};
		Color(const double _r, const double _g, const double _b) :
			r(_r), g(_g), b(_b) {};

		inline json to_json() const {
			json j;
			j["r"] = r;
			j["g"] = g;
			j["b"] = b;
			return j;
		}

		inline bool from_json(json &j) {
			try {
				r = j["r"];
				g = j["g"];
				b = j["b"];
				return true;
			} catch(...) {};
			return false;
		}
	};

	class Point2D {
	public:
		double x = 0;
		double y = 0;

		Point2D() {};
		Point2D(const double _x, const double _y) :
			x(_x), y(_y) {};

		inline json to_json() const {
			json j;
			j["x"] = x;
			j["y"] = y;
			return j;
		}

		inline bool from_json(json &j) {
			try {
				x = j["x"];
				y = j["y"];
				return true;
			} catch(...) {};
			return false;
		}
	};

	enum HeatMapStyle {
		DEFAULT_HEATMAP = 0,
		COLOR_HEATMAP,
		PLASMA_HEATMAP,
		GRAY_HEATMAP,
		HOT_HEATMAP,
		RD_BU_HEATMAP,
		SPECTRAL_HEATMAP,
	};

	enum PlotType {
		HEATMAP = 0,
		HEATMAP_3D,
	};

	enum TimeMode {
		MILLISECONDS = 0,
		SECONDS = 1,
		MINUTES = 2,
		HOURS = 3,
		TIME = 4,
		DATE = 5,
		DATE_TIME = 6,
		DATE_TIME_MS = 7,
	};

}; // ep2

#endif // EASY_PLOT_2_COMMON_HPP_INCLUDED
