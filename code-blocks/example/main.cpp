#include <iostream>
#include <array>
#include "easy_plot_v2/ep2.hpp"

int main() {
	std::cout << "Hello world!" << std::endl;
	ep2::PlotConfig save_config;
	{
		ep2::PlotConfig config;
		config.file_name = "test.ep2";
		config.name = "test";
		config.note = "Your description could be here";

		ep2::PlotConfig::Heatmap heatmap;
		heatmap.init(10,10);
		for(size_t x = 0 ; x < heatmap.w; ++x) {
			for(size_t y = 0; y < heatmap.h; ++y) {
				heatmap.data[x][y] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18));
			}
		}
		heatmap.name = "test heatmap";
		heatmap.note = "Your description could be here";
		heatmap.text_x = "X description";
		heatmap.text_y = "Y description";

		ep2::PlotConfig::Heatmap heatmap_2;
		heatmap_2.init(32,32);
		for(size_t x = 0 ; x < heatmap_2.w; ++x) {
			for(size_t y = 0; y < heatmap_2.h; ++y) {
				heatmap_2.data[x][y] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18));
			}
		}
		heatmap_2.name = "test heatmap_2";
		heatmap_2.note = "Your description could be here";
		heatmap_2.text_x = "X description";
		heatmap_2.text_y = "Y description";

		config.add_heatmap(heatmap);
		config.add_heatmap(heatmap_2);

		if (!ep2::save_plot(config)) {
			std::cout << "Error: ep2::save_plot" << std::endl;
		}
		save_config = config;
	}
	{
		ep2::PlotConfig config;
		config.file_name = "test.ep2";

		if (!ep2::open_plot(config)) {
			std::cout << "Error: ep2::open_plot" << std::endl;
		}

		if (config.name != save_config.name) std::cout << "Error: config != save_config (1)" << std::endl;
		if (config.heatmap.size() != save_config.heatmap.size()) std::cout << "Error: config != save_config (2)" << std::endl;

		for (size_t i = 0; i < config.heatmap.size(); ++i) {
			ep2::PlotConfig::Heatmap heatmap = config.get_heatmap(i);
			ep2::PlotConfig::Heatmap save_heatmap = save_config.get_heatmap(i);
			if (heatmap.w != save_heatmap.w) std::cout << "Error: config != save_config (3) " << heatmap.w << " | " << save_heatmap.w << std::endl;
			if (heatmap.h != save_heatmap.h) std::cout << "Error: config != save_config (4) " << heatmap.h << " | " << save_heatmap.h << std::endl;
			for(size_t x = 0 ; x < heatmap.w; ++x) {
				for(size_t y = 0; y < heatmap.h; ++y) {
					if (std::abs(heatmap.data[x][y] - save_heatmap.data[x][y]) > 0.01) std::cout << "Error: config != save_config (5)" << std::endl;
				}
			}

		}
	}
	//
	return 0;
}
