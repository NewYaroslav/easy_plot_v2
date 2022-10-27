#ifndef EASY_PLOT_2_VIEWER_FONTS_HPP_INCLUDED
#define EASY_PLOT_2_VIEWER_FONTS_HPP_INCLUDED

/** \brief Инициализировать шрифты
 */
static void init_fonts(const std::string &path) {
	/* настраиваем язык */
	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = 1;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};

	ImGuiIO& io = ImGui::GetIO();
	std::string file_name = path + "fonts/Roboto-Medium.ttf";
	io.Fonts->AddFontFromFileTTF(file_name.c_str(), 18.0f, NULL, ranges);
	//io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 24.0f, NULL, ranges);

	ImGui::SFML::UpdateFontTexture();
}

#endif // EASY_PLOT_2_VIEWER_FONTS_HPP_INCLUDED
