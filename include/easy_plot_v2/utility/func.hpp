#ifndef EASY_PLOT_2_FUNC_HPP_INCLUDED
#define EASY_PLOT_2_FUNC_HPP_INCLUDED

#include "common.hpp"
#include "files.hpp"
#include "plot_config.hpp"

namespace ep2 {

	bool save_plot(const PlotConfig &config) {
		std::vector<std::uint8_t> v_msgpack = config.to_msgpack();
		create_directory(config.file_name, true);
		return write_file(config.file_name, v_msgpack);
	}

	bool open_plot(PlotConfig &config) {
		std::vector<std::uint8_t> v_msgpack;
		if (!load_file(config.file_name, v_msgpack)) return false;
		return config.from_msgpack(v_msgpack);
	}

	bool draw_plot(const PlotConfig &config) {
		save_plot(config);
		const std::string cmd_text = config.file_name;
		std::system(cmd_text.c_str());
	}

}; // ep2

#endif // EASY_PLOT_2_FUNC_HPP_INCLUDED
