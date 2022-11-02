#ifndef EASY_PLOT_2_PLOT_CONFIG_HPP_INCLUDED
#define EASY_PLOT_2_PLOT_CONFIG_HPP_INCLUDED

#include "common.hpp"

namespace ep2 {

	/** \brief Класс для хранения конфигурации графика
	 */
	class PlotConfig {
	private:

		inline const struct timespec get_timespec() const noexcept {
			// https://en.cppreference.com/w/c/chrono/timespec_get
			struct timespec ts;
#			if defined(CLOCK_REALTIME)
			clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#			else
			timespec_get(&ts, TIME_UTC);
#			endif
			return ts;
		}

		const uint64_t get_timestamp_ms() const noexcept {
			const struct timespec ts = get_timespec();
			const uint64_t MILLISECONDS_IN_SECOND = 1000;
			const uint64_t NANOSECONDS_IN_SECOND = 1000000;
			return MILLISECONDS_IN_SECOND * ts.tv_sec + ts.tv_nsec / NANOSECONDS_IN_SECOND;
		}

	public:

		std::string file_name;
		std::string name;
		std::string desc;
		uint64_t date	= 0;
		int version		= EASY_PLOT_2_VERSION;

		class Heatmap {
		public:
			std::vector<std::vector<double>> data;

			template<class T>
			std::vector<T> get_data() {
				std::vector<T> temp(w * h);
				if (data.empty() || data[0].empty()) return std::move(temp);
				std::vector<std::vector<double>> r_data(h, std::vector<double>(w, 0));
				for (int x = 0; x < w; ++x) {
					for (int y = 0; y < h; ++y) {
						r_data[y][x] = data[x][y];
					}
				}

				size_t i = temp.size() - 1;
				for (int x = 0; x < h; ++x) {
					for (int y = w-1; y >= 0; --y) {
						temp[i--] = r_data[x][y];
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
			std::string desc;
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
				j["desc"] = desc;
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
					if (j.contains("note")) {
                        desc = j["note"];
					} else
					if (j.contains("desc")){
                        desc = j["desc"];
					}
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

        /** \brief Класс для работы с 3D тепловой картой
         */
		class Heatmap3D {
		private:

            template<class T>
			std::vector<T> to_data(
                    const std::vector<std::vector<double>> &src,
                    const size_t _w,
                    const size_t _h) {
				std::vector<T> temp(_w * _h);
				if (src.empty() || src[0].empty()) return std::move(temp);
				std::vector<std::vector<double>> r_data(_h, std::vector<double>(_w, 0));
				for (int x = 0; x < _w; ++x) {
					for (int y = 0; y < _h; ++y) {
						r_data[y][x] = src[x][y];
					}
				}

				size_t i = temp.size() - 1;
				for (int x = 0; x < _h; ++x) {
					for (int y = _w-1; y >= 0; --y) {
						temp[i--] = r_data[x][y];
					}
				}
				return std::move(temp);
			}

		public:
			std::vector<std::vector<std::vector<double>>> data;

			enum ViewType {
                XY,
                XZ,
                YZ
			};

			template<class T>
			std::vector<T> get_data(const int type, size_t v) {
                std::vector<std::vector<double>> temp;
                switch (type) {
                case ViewType::XY:
                    if (v >= l) v = l - 1;
                    temp.resize(w, std::vector<double>(h, 0));
                    for (size_t x = 0; x < w; ++x) {
                        for (size_t y = 0; y < h; ++y) {
                            temp[x][y] = data[x][y][v];
                        }
                    }
                    return to_data<T>(temp, w, h);
                case ViewType::XZ:
                    if (v >= h) v = h - 1;
                    temp.resize(w, std::vector<double>(l, 0));
                    for (size_t x = 0; x < w; ++x) {
                        for (size_t z = 0; z < l; ++z) {
                            temp[x][z] = data[x][v][z];
                        }
                    }
                    return to_data<T>(temp, w, l);
                case ViewType::YZ:
                    if (v >= w) v = w - 1;
                    temp.resize(l, std::vector<double>(h, 0));
                    for (size_t z = 0; z < l; ++z) {
                        for (size_t y = 0; y < h; ++y) {
                            temp[z][y] = data[v][y][z];
                        }
                    }
                    return to_data<T>(temp, l, h);
                }

				return to_data<T>(temp, 0, 0);
			}

			int w = 0;
			int h = 0;
			int l = 0;

			double min = 0;
			double max = 0;

			inline void init(const size_t _w, const size_t _h, const size_t _l) noexcept {
				w = _w;
				h = _h;
				l = _l;
				data.resize(w);
				for (size_t x = 0; x < w; ++x) {
					data[x].resize(h, std::vector<double>(l, 0));
				}
			}

			inline void calc_min_max() {
				if (data.empty() || data[0].empty() || data[0][0].empty()) return;
				max = min = data[0][0][0];
				for (size_t x = 0; x < w; ++x) {
					for (size_t y = 0; y < h; ++y) {
                        for (size_t z = 0; z < l; ++z) {
                            if (data[x][y][z] < min) min = data[x][y][z];
                            if (data[x][y][z] > max) max = data[x][y][z];
						}
					}
				}
			}

			inline void calc_min_max(double &scale_min, double &scale_max) const {
				if (data.empty() || data[0].empty() || data[0][0].empty()) return;
				scale_max = scale_min = data[0][0][0];
				for (size_t x = 0; x < w; ++x) {
					for (size_t y = 0; y < h; ++y) {
                        for (size_t z = 0; z < l; ++z) {
                            if (data[x][y][z] < scale_min) scale_min = data[x][y][z];
                            if (data[x][y][z] > scale_max) scale_max = data[x][y][z];
						}
					}
				}
			}

			std::string name;
			std::string desc;
			std::string text_x;
			std::string text_y;
			std::string text_z;

			int style	= HeatMapStyle::DEFAULT_HEATMAP;

			Point2D start   = Point2D(0,0);
			Point2D step	= Point2D(1,1);

			Color text      = Color(1.0,1.0,1.0);	/**< цвет текста */
			Color mouse     = Color(0.5,0.5,0.5);	/**< цвет указателя */

            int view_type = ViewType::XY;
            int layer = 0;

            inline void check_layer() {
                if (layer < 0) layer = 0;
                switch(view_type) {
                case ViewType::XY:
                    if (layer > l) layer = l - 1;
                    break;
				case ViewType::XZ:
                    if (layer > h) layer = h - 1;
                    break;
				case ViewType::YZ:
                    if (layer > w) layer = w - 1;
                    break;
				};
			}

            inline int get_2d_w() {
                switch(view_type) {
                case ViewType::XY:
                    return w;
				case ViewType::XZ:
                    return w;
				case ViewType::YZ:
                    return l;
				};
				return 0;
			}

			inline int get_2d_h() {
                switch(view_type) {
                case ViewType::XY:
                    return h;
				case ViewType::XZ:
                    return l;
				case ViewType::YZ:
                    return h;
				};
				return 0;
			}

			inline int get_2d_l() {
                switch(view_type) {
                case ViewType::XY:
                    return l;
				case ViewType::XZ:
                    return h;
				case ViewType::YZ:
                    return w;
				};
				return 0;
			}

			inline json to_json() const {
				json j;
				j["name"] = name;
				j["desc"] = desc;
				j["text_x"] = text_x;
				j["text_y"] = text_y;
				j["text_z"] = text_z;
				j["style"] = style;
				j["start"] = start.to_json();
				j["step"] = step.to_json();
				j["color"]["text"] = text.to_json();
				j["color"]["mouse"] = mouse.to_json();
				j["w"] = w;
				j["h"] = h;
				j["l"] = l;
				double scale_min = 0, scale_max = 0;
				calc_min_max(scale_min, scale_max);
				j["max"] = scale_max;
				j["min"] = scale_min;
				json j_data;
				for (size_t x = 0; x < data.size(); ++x) {
					j_data[x] = json::array();
					for (size_t y = 0; y < data[x].size(); ++y) {
						j_data[x][y] = json::array();
						for (size_t z = 0; z < data[x][y].size(); ++z) {
                            j_data[x][y][z] = data[x][y][z];
                        }
					}
				}
				j["data"] = j_data;
				return j;
			}

			inline bool from_json(json &j) {
				try {
					name = j["name"];
					if (j.contains("note")) {
                        desc = j["note"];
					} else
					if (j.contains("desc")){
                        desc = j["desc"];
					}
					text_x = j["text_x"];
					text_y = j["text_y"];
					text_z = j["text_z"];
					style = j["style"];
					start.from_json(j["start"]);
					step.from_json(j["step"]);
					text.from_json(j["color"]["text"]);
					mouse.from_json(j["color"]["mouse"]);
					w = j["w"];
					h = j["h"];
					l = j["l"];
					min = j["min"];
					max = j["max"];
					init(w, h, l);
					json &j_data = j["data"];
					for (size_t x = 0; x < w; ++x) {
						for (size_t y = 0; y < h; ++y) {
							 for (size_t z = 0; z < l; ++z) {
                                data[x][y][z] = j_data[x][y][z];
							 }
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
			std::string desc;
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
				j["desc"] = desc;
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
					if (j.contains("note")) {
                        desc = j["note"];
					} else
					if (j.contains("desc")){
                        desc = j["desc"];
					}
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
		std::vector<Heatmap3D> heatmap_3d;

		inline void add_heatmap(const Heatmap &data) noexcept {
			heatmap.push_back(data);
		}

		inline Heatmap get_heatmap(const size_t x) noexcept {
			if (x >= heatmap.size()) return Heatmap();
			return heatmap[x];
		}

		inline void add_heatmap_3d(const Heatmap3D &data) noexcept {
			heatmap_3d.push_back(data);
		}

		inline Heatmap3D get_heatmap_3d(const size_t x) noexcept {
			if (x >= heatmap_3d.size()) return Heatmap3D();
			return heatmap_3d[x];
		}

		inline void add_line(const Line &data) noexcept {
			line.push_back(data);
		}

		inline Line get_line(const size_t x) noexcept {
			if (x >= line.size()) return Line();
			return line[x];
		}

		inline void init_date() noexcept {
			date = get_timestamp_ms();
		}

		inline json to_json() const {
			json j;
			j["name"] = name;
			j["desc"] = desc;
			if (date == 0) {
				j["date"] = get_timestamp_ms();
			} else {
				j["date"] = date;
			}
			j["version"] = version;

			j["line"] = json::array();
			for (size_t i = 0; i < line.size(); ++i) {
				j["line"][i] = line[i].to_json();
			}

			j["heatmap"] = json::array();
			for (size_t i = 0; i < heatmap.size(); ++i) {
				j["heatmap"][i] = heatmap[i].to_json();
			}

			j["heatmap_3d"] = json::array();
			for (size_t i = 0; i < heatmap_3d.size(); ++i) {
				j["heatmap_3d"][i] = heatmap_3d[i].to_json();
			}
			return j;
		}

		inline bool from_json(json &j) {
			try {
				name = j["name"];
				if (j.contains("note")) {
                    desc = j["note"];
                } else
                if (j.contains("desc")){
                    desc = j["desc"];
                }
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

				heatmap_3d.resize(j["heatmap_3d"].size());
				for (size_t i = 0; i < heatmap_3d.size(); ++i) {
					if (!heatmap_3d[i].from_json(j["heatmap_3d"][i])) return false;
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
