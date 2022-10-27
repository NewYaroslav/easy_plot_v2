#ifndef EASY_PLOT_2_PLOT_CONFIG_HPP_INCLUDED
#define EASY_PLOT_2_PLOT_CONFIG_HPP_INCLUDED

#include "common.hpp"

namespace ep2 {

	/** \brief Класс для хранения конфигурации графика
	 */
	class PlotConfig {
	public:

		std::string file_name;
		std::string name;
		std::string note;
		uint64_t date	= 0;
		int version		= EASY_PLOT_2_VERSION;

		class Heatmap {
		public:
			std::vector<std::vector<double>> data;

			template<class T>
			std::vector<T> get_data() {
				std::vector<T> temp(w * h);
				if (data.empty() || data[0].empty()) return std::move(temp);
				size_t i = 0;
				for (size_t x = 0; x < w; ++x) {
					for (size_t y = 0; y < h; ++y) {
						temp[i++] = data[x][y];
					}
				}
				return std::move(temp);
			}

			int w = 0;
			int h = 0;

			double min = 0;
			double max = 0;

			inline void init(const size_t _w, const size_t _h) noexcept {
				w = _w;
				h = _h;
				data.resize(w);
				for (size_t x = 0; x < w; ++x) {
					data[x].resize(h, 0);
				}
			}

			inline void calc_min_max() {
				if (data.empty() || data[0].empty()) return;
				max = min = data[0][0];
				for (size_t x = 0; x < w; ++x) {
					for (size_t y = 0; y < h; ++y) {
						if (data[x][y] < min) min = data[x][y];
						if (data[x][y] > max) max = data[x][y];
					}
				}
			}

			inline void calc_min_max(double &scale_min, double &scale_max) const {
				if (data.empty() || data[0].empty()) return;
				scale_max = scale_min = data[0][0];
				for (size_t x = 0; x < w; ++x) {
					for (size_t y = 0; y < h; ++y) {
						if (data[x][y] < scale_min) scale_min = data[x][y];
						if (data[x][y] > scale_max) scale_max = data[x][y];
					}
				}
			}

			std::string name;
			std::string note;
			std::string text_x;
			std::string text_y;

			int style	= HeatMapStyle::DEFAULT_HEATMAP;

			Point2D start	= Point2D(0,0);
			Point2D step	= Point2D(1,1);

			Color text			= Color(1.0,1.0,1.0);	/**< цвет текста */
			Color mouse			= Color(0.5,0.5,0.5);	/**< цвет указателя */

			inline json to_json() const {
				json j;
				j["name"] = name;
				j["note"] = note;
				j["text_x"] = text_x;
				j["text_y"] = text_y;
				j["style"] = style;
				j["start"] = start.to_json();
				j["step"] = step.to_json();
				j["color"]["text"] = text.to_json();
				j["color"]["mouse"] = mouse.to_json();
				j["w"] = w;
				j["h"] = h;
				double scale_min = 0, scale_max = 0;
				calc_min_max(scale_min, scale_max);
				j["max"] = scale_max;
				j["min"] = scale_min;
				json j_data;
				for (size_t x = 0; x < data.size(); ++x) {
					j_data[x] = json::array();
					for (size_t y = 0; y < data[x].size(); ++y) {
						j_data[x][y] = data[x][y];
					}
				}
				j["data"] = j_data;
				return j;
			}

			inline bool from_json(json &j) {
				try {
					name = j["name"];
					note = j["note"];
					text_x = j["text_x"];
					text_y = j["text_y"];
					style = j["style"];
					start.from_json(j["start"]);
					step.from_json(j["step"]);
					text.from_json(j["color"]["text"]);
					mouse.from_json(j["color"]["mouse"]);
					w = j["w"];
					h = j["h"];
					min = j["min"];
					max = j["max"];
					init(w, h);
					json &j_data = j["data"];
					for (size_t x = 0; x < w; ++x) {
						for (size_t y = 0; y < h; ++y) {
							 data[x][y] = j_data[x][y];
						}
					}
					return true;
				} catch(...) {};
				return false;
			}
		};

		class Line {
		public:
			std::vector<double> data_x;
			std::vector<double> data_y;

			inline void init(const size_t l) noexcept {
				data_x.resize(l,0);
				data_y.resize(l,0);
			}

			int id = 0;

			std::string name;
			std::string note;
			std::string text_x;
			std::string text_y;

			Point2D grid_period = Point2D(0,0);

			Color line			= Color(1.0,1.0,1.0);	/**< цвет изображения */
			Color grid			= Color(0.1,0.1,0.1);	/**< цвет сетки */
			Color frame			= Color(1.0,1.0,1.0);	/**< цвет сетки */
			Color text			= Color(1.0,1.0,1.0);	/**< цвет текста */
			Color mouse			= Color(0.5,0.5,0.5);	/**< цвет указателя */

			inline json to_json() const {
				json j;
				j["name"] = name;
				j["note"] = note;
				j["text_x"] = text_x;
				j["text_y"] = text_y;
				j["id"] = id;
				j["grid_period"] = grid_period.to_json();
				j["color"]["line"] = line.to_json();
				j["color"]["grid"] = grid.to_json();
				j["color"]["frame"] = frame.to_json();
				j["color"]["text"] = text.to_json();
				j["color"]["mouse"] = mouse.to_json();
				j["data"]["x"] = data_x;
				j["data"]["y"] = data_y;
				return j;
			}

			inline bool from_json(json &j) {
				try {
					name = j["name"];
					note = j["note"];
					text_x = j["text_x"];
					text_y = j["text_y"];
					id = j["id"];
					grid_period.from_json(j["grid_period"]);
					line.from_json(j["color"]["line"]);
					grid.from_json(j["color"]["grid"]);
					frame.from_json(j["color"]["frame"]);
					text.from_json(j["color"]["text"]);
					mouse.from_json(j["color"]["mouse"]);
					data_x = j["data"]["x"].get<std::vector<double>>();
					data_y = j["data"]["y"].get<std::vector<double>>();
					return true;
				} catch(...) {};
				return false;
			}

			Line() {};
			~Line() {};
		};

		std::vector<Line> line;
		std::vector<Heatmap> heatmap;

		inline void add_heatmap(const Heatmap &data) noexcept {
			heatmap.push_back(data);
		}

		inline Heatmap get_heatmap(const size_t x) noexcept {
			if (x >= heatmap.size()) return Heatmap();
			return heatmap[x];
		}

		inline void add_line(const Line &data) noexcept {
			line.push_back(data);
		}

		inline Line get_line(const size_t x) noexcept {
			if (x >= line.size()) return Line();
			return line[x];
		}

		inline json to_json() const {
			json j;
			j["name"] = name;
			j["note"] = note;
			j["date"] = date;
			j["version"] = version;

			j["line"] = json::array();
			for (size_t i = 0; i < line.size(); ++i) {
				j["line"][i] = line[i].to_json();
			}

			j["heatmap"] = json::array();
			for (size_t i = 0; i < heatmap.size(); ++i) {
				j["heatmap"][i] = heatmap[i].to_json();
			}
			return j;
		}

		inline bool from_json(json &j) {
			try {
				name = j["name"];
				note = j["note"];
				date = j["date"];
				version = j["version"];
				line.resize(j["line"].size());
				for (size_t i = 0; i < line.size(); ++i) {
					if (!line[i].from_json(j["line"][i])) return false;
				}
				heatmap.resize(j["heatmap"].size());
				for (size_t i = 0; i < heatmap.size(); ++i) {
					if (!heatmap[i].from_json(j["heatmap"][i])) return false;
				}
				return true;
			} catch(...) {};
			return false;
		}

		inline std::vector<std::uint8_t> to_msgpack() const {
			std::vector<std::uint8_t> v_msgpack = json::to_msgpack(to_json());
			return std::move(v_msgpack);
		}

		inline bool from_msgpack(const std::vector<std::uint8_t> &v_msgpack) {
			try {
				json j_from_msgpack = json::from_msgpack(v_msgpack);
				return from_json(j_from_msgpack);
			} catch(...) {};
			return false;
		}

		PlotConfig() {};
	};

}; // ep2

#endif // EASY_PLOT_2_PLOT_CONFIG_HPP_INCLUDED