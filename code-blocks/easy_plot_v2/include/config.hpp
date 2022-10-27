#ifndef EASY_PLOT_2_VIEWER_CONFIG_HPP_INCLUDED
#define EASY_PLOT_2_VIEWER_CONFIG_HPP_INCLUDED

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <string>

namespace ep2 {

	/** \brief Параметры графического меню
	 */
	class Config {
	public:
		const std::string window_name = "EP2 Viewer [" EASY_PLOT_2_VIEWER_VERSION "]";
		const size_t start_window_width		= 800;
		const size_t start_window_height	= 500;
		const size_t framerate				= 60;

		const size_t indent_basement		= 0;

		const size_t decode_settings_width	= 740;
		const size_t decode_settings_height = 280;

		const size_t error_width	= 400;
		const size_t error_height	= 250;

		sf::Color window_background = sf::Color(16,16,16);
	};
}; // ep2

#endif // EASY_PLOT_2_VIEWER_CONFIG_HPP_INCLUDED
