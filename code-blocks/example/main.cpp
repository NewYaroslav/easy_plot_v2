#include <iostream>
#include <array>
#include "easy_plot_v2/ep2.hpp"

int main() {
	std::cout << "Hello world!" << std::endl;
	ep2::PlotConfig save_config;
	{
		ep2::PlotConfig config;
		config.file_name = "test-heatmap.ep2";
		config.name = "test heatmap";
		config.desc = "Your description could be here";

		ep2::PlotConfig::Heatmap heatmap;
		heatmap.init(10,10);
		for(size_t x = 0 ; x < heatmap.w; ++x) {
			for(size_t y = 0; y < heatmap.h; ++y) {
				heatmap.data[x][y] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18));
			}
		}
		heatmap.name = "test heatmap";
		heatmap.desc = "Your description could be here";
		heatmap.text_x = "X description";
		heatmap.text_y = "Y description";

		ep2::PlotConfig::Heatmap heatmap_2;
		heatmap_2.init(32,32);
		for(size_t x = 0 ; x < heatmap_2.w; ++x) {
			for(size_t y = 0; y < heatmap_2.h; ++y) {
				heatmap_2.data[x][y] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18));
			}
		}
		heatmap_2.name = "test heatmap 2";
		heatmap_2.desc = "Your description could be here";
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
		config.file_name = "test-heatmap.ep2";

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
	// 3D
	{
		ep2::PlotConfig config;
		config.file_name = "test-heatmap-3d.ep2";
		config.name = "test heatmap 3d";
		config.desc = "Your description could be here";

		ep2::PlotConfig::Heatmap3D heatmap;
		heatmap.init(120, 100, 20);
		for(size_t x = 0 ; x < heatmap.w; ++x) {
			for(size_t y = 0; y < heatmap.h; ++y) {
				for(size_t z = 0; z < heatmap.l; ++z) {
                    heatmap.data[x][y][z] = 1024 - std::sqrt((x - 18) * (x - 18) + (y - 18) * (y - 18) + (z - 18) * (z - 18));
                }
			}
		}
		heatmap.name = "test heatmap 3d";
		heatmap.desc = "Your description could be here";
		heatmap.text_x = "X description";
		heatmap.text_y = "Y description";
        heatmap.text_z = "Z description";

		config.add_heatmap_3d(heatmap);

		if (!ep2::save_plot(config)) {
			std::cout << "Error: ep2::save_plot" << std::endl;
		}
		save_config = config;
	}
	// Line
	{
        ep2::PlotConfig config;
		config.file_name = "test-line.ep2";
		config.name = "test line";
		config.desc = "Your description could be here";

		ep2::PlotConfig::Line line;
		line.init(100);
		for(size_t x = 0 ; x < line.w; ++x) {
            line.data_x[x] = x * 10;
            line.data_y[x] = 100 + std::sqrt((x - 20) * (x - 20));
		}
		line.name = "test line";
		line.desc = "Your description could be here";
		line.text_x = "X description";
		line.text_y = "Y description";

		config.add_line(line);

		if (!ep2::save_plot(config)) {
			std::cout << "Error: ep2::save_plot" << std::endl;
		}
		save_config = config;
	}
	// Line ??3
	{
        ep2::PlotConfig config;
		config.file_name = "test-line-x3.ep2";
		config.name = "test line 2";
		config.desc = "Your description could be here";

		ep2::PlotConfig::Line line;
		line.init(100);
		for(size_t x = 0 ; x < line.w; ++x) {
            line.data_x[x] = x * 20;
            line.data_y[x] = 100 + std::sqrt((x - 20) * (x - 20));
		}
		line.name = "test line 2";
		line.desc = "Your description could be here";
		line.text_x = "X description";
		line.text_y = "Y description";

		config.add_line(line);
		config.add_line(line);
		config.add_line(line);

		if (!ep2::save_plot(config)) {
			std::cout << "Error: ep2::save_plot" << std::endl;
		}
		save_config = config;
	}
	// Line ??3 with Time
	{
        ep2::PlotConfig config;
		config.file_name = "test-line-x3-time.ep2";
		config.name = "test line time";
		config.desc = "Your description could be here";

		ep2::PlotConfig::Line line;
		line.init(100);
		for(size_t x = 0 ; x < line.w; ++x) {
            line.data_x[x] = x * 3600 * 24 * 365  + 3600 * 24 * 365 * 30;
            line.data_y[x] = 100 + std::sqrt((x - 20) * (x - 20));
		}
		line.name = "test line time";
		line.desc = "Your description could be here";
		line.text_x = "X description";
		line.text_y = "Y description";
		line.use_scale_time_x = true;
		line.time_mode_x = ep2::TimeMode::DATE;

		config.add_line(line);
		config.add_line(line);
		config.add_line(line);

		if (!ep2::save_plot(config)) {
			std::cout << "Error: ep2::save_plot" << std::endl;
		}
		save_config = config;
	}
	return 0;
}
