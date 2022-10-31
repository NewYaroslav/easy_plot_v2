#define EASY_PLOT_2_VIEWER_VERSION "0.1 DEMO"

#if defined(_WIN32)
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif
#endif
#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "implot.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "include/config.hpp"
#include "include/fonts.hpp"
#include "include/style.hpp"
#include "include/imgui-addons.hpp"

#include "easy_plot_v2/ep2.hpp"

// Конфигурация программы
ep2::Config config;
// Конфигурация графиков
ep2::PlotConfig plot_config;
bool is_open_file = false;
bool is_open_file_error = false;
bool is_auto_update_data = false;

int main(int argc, char* argv[]) {
	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_HIDE);

	const std::string main_path = ep2::get_clear_path(argv[0]);
	const std::string init_file_name = main_path + "imgui.ini";

	if (argc >= 2) {
		plot_config.file_name = std::string(argv[1]);
		if (!ep2::open_plot(plot_config)) {
			is_open_file_error = true;
		} else {
			is_open_file = true;
		}
	}

	/* инициализируем окно */
	sf::RenderWindow window(sf::VideoMode(config.start_window_width, config.start_window_height), config.window_name.c_str());
	window.setFramerateLimit(config.framerate);

	/* настраиваем внешний вид программы */
	sf::Image icon;
	if (icon.loadFromFile(main_path + "icon/icon.png")) {
		window.setIcon(32, 32, icon.getPixelsPtr());
	}

	ImGui::SFML::Init(window, false);
	ImGui::StyleColorsDark();
	ImPlot::CreateContext();
	init_fonts(main_path);
	init_style();

	/* создаем браузер файловой системы и редактор текста */
	imgui_addons::ImGuiFileBrowser file_dialog;
	ep2::ErrorPopup error_popup("##ErrorPopup");

	sf::Clock delta_clock;
	sf::Clock reopen_file_clock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			} else
			if (event.type == sf::Event::Resized) {
				sf::Vector2u window_size = window.getSize();
				if (window_size.x < config.start_window_width ||
					window_size.y < config.start_window_height) {
					window_size.x = std::max(window_size.x, (uint32_t)config.start_window_width);
					window_size.y = std::max(window_size.y, (uint32_t)config.start_window_height);
					window.setSize(window_size);
				}
			}
		}

		//{ Обновляем данные файла
		const uint32_t reopen_file_delay = 100;
		if (is_auto_update_data && is_open_file && reopen_file_clock.getElapsedTime().asMilliseconds() > reopen_file_delay) {
			reopen_file_clock.restart();
			ep2::PlotConfig reopen_plot_config = plot_config;
			if (ep2::open_plot(reopen_plot_config)) {
				plot_config = reopen_plot_config;
			}
		}
		//} Обновляем данные файла

		ImGui::SFML::Update(window, delta_clock.restart());
		static bool is_once_load_init = false;
		if (!is_once_load_init) {
			is_once_load_init = true;
			ImGui::LoadIniSettingsFromDisk(init_file_name.c_str());
		}

		const size_t window_width = window.getSize().x;
		const size_t window_height = window.getSize().y;

		ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(
			window_width,
			(window_height - config.indent_basement)),
			ImGuiCond_Always);

		//{ Начало основного меню
		ImGui::Begin("EasyPlot2ViewerMainMenu", nullptr,
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_MenuBar);

		bool file_dialog_open = false, file_dialog_save = false;
		bool clipboard_error_dialog_open = false;
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open")) {
					file_dialog_open = true;
				}
				if (ImGui::MenuItem("Save", nullptr, nullptr, is_open_file)) {
					// сохраняем файл
					ep2::save_plot(plot_config);
				}
				if (ImGui::MenuItem("Save as...", nullptr, nullptr, is_open_file)) {
					file_dialog_save = true;
				}
				if (ImGui::MenuItem("Quit", "Alt-F4"))
					break;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Auto update data", nullptr, &is_auto_update_data, is_open_file)) {

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (is_open_file_error) {
			error_popup.open("ERROR", "File opening error!");
			is_open_file_error = false;
		}

		//{ Отрисовываем все тепловые карты
		for (size_t i = 0; i < plot_config.heatmap.size(); ++i) {
			ep2::PlotConfig::Heatmap &heatmap = plot_config.heatmap[i];

			// Получаем имя тепловой карты
			std::string heatmap_name;
			if (!heatmap.name.empty()) heatmap_name = heatmap.name;
			else heatmap_name = "none";

			std::string heatmap_title_id("##Heatmap_" + std::to_string(i));
			std::string child_title_id("##ChildHeatmap_" + std::to_string(i));

			//{ Определяем размер окна
			sf::Vector2u window_size = window.getSize();
			const float window_indent = 8;
			const float window_left_indent = 128;
			const float window_bottom_indent = 128 + 32;
			const float max_window_scale = std::min(window_size.x - 3 * window_indent - window_left_indent, window_size.y - 3 * window_indent - window_bottom_indent);
			const float max_heatmap_scale = std::max(heatmap.w, heatmap.h);


			ImVec2 plot_size(
				((float)heatmap.w * max_window_scale / max_heatmap_scale),
				((float)heatmap.h * max_window_scale / max_heatmap_scale));

			const float max_heatmap_width = plot_size.x + window_left_indent + 2 * window_indent;
			//} Определяем размер окна

			ImGui::BeginChild(child_title_id.c_str(), ImVec2(max_heatmap_width, plot_size.y + 2 * window_indent + window_bottom_indent), false);
			ImGui::BeginGroup();

			if (ImGui::BeginCombo("##HeatmapName", heatmap_name.c_str())) {
				if (!heatmap.note.empty()) {
					ImGui::InputTextMultiline("Description##HeatmapDescription",
					(char*)heatmap.note.c_str(), heatmap.note.size(),
					ImVec2(0,96), ImGuiInputTextFlags_ReadOnly);
				}
				ImGui::Text("Size: %u X %u", heatmap.w, heatmap.h);
				ImGui::Text("Min: %f / Max: %f", heatmap.min, heatmap.max);
				ImGui::EndCombo();
			}

			//ImGui::Text(heatmap_name.c_str());

			std::vector<int> style_to_implot_colormap = {
				ImPlotColormap_Jet,
				ImPlotColormap_Plasma,
				ImPlotColormap_Greys,
				ImPlotColormap_Hot,
				ImPlotColormap_RdBu,
				ImPlotColormap_Spectral
			};

			//static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoLabel;
			static ImPlotAxisFlags axes_flags = 0;//ImPlotAxisFlags_NoTickLabels;
			int value_x = 0, value_y = 0;

			ImPlotColormap heatmap_style = style_to_implot_colormap[heatmap.style];
			ImPlot::PushColormap(heatmap_style);

			if (ImPlot::BeginPlot(heatmap_title_id.c_str(), plot_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText | ImPlotFlags_NoTitle)) {
				ImPlot::SetupAxes(heatmap.text_x.c_str(), heatmap.text_y.c_str(), axes_flags, axes_flags);
				ImPlot::SetupMouseText(ImPlotLocation_Center, ImPlotMouseTextFlags_None);

				std::vector<float> temp = heatmap.get_data<float>();

				ImPlot::PlotHeatmap(
					heatmap_name.c_str(),
					&temp[0],
					heatmap.h,
					heatmap.w,
					heatmap.min,
					heatmap.max,
					nullptr,
					ImPlotPoint(0, 0.0),
					ImPlotPoint(heatmap.w, heatmap.h));

				if (ImPlot::IsPlotHovered()) {
					ImPlotPoint mouse_pos = ImPlot::GetPlotMousePos();

					value_x = (double)mouse_pos.x;// * (double)heatmap.h;
					value_y = (double)mouse_pos.y;// * (double)heatmap.w;

					if (value_y < 0) value_y = 0;
					if (value_y >= heatmap.h) value_y = heatmap.h - 1;
					if (value_x < 0) value_x = 0;
					if (value_x >= heatmap.w) value_x = heatmap.w - 1;

					static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None | ImPlotDragToolFlags_NoInputs;

					ImPlot::DragLineX(1, &mouse_pos.x, ImVec4(heatmap.mouse.r,heatmap.mouse.g,heatmap.mouse.b, 1), 1, flags);
					ImPlot::DragLineY(2, &mouse_pos.y, ImVec4(heatmap.mouse.r,heatmap.mouse.g,heatmap.mouse.b, 1), 1, flags);
				}
				ImPlot::EndPlot();
			}

			ImGui::SameLine();
			ImPlot::ColormapScale("##HeatScale", heatmap.min, heatmap.max, ImVec2(window_left_indent / 1.5, plot_size.y));
			// Вывод значения ячейки
			ImGui::Text("(%u, %u) = %f", value_x, value_y, heatmap.data[value_x][value_y]);
			// Вывод мин-макс значений
			ImGui::Text("Min: %f / Max: %f", heatmap.min, heatmap.max);

			// Смена цвета тепловой карты
			if (ImPlot::ColormapButton(ImPlot::GetColormapName(heatmap_style),
					ImVec2(128,0), heatmap_style)) {
				++heatmap.style;
				if (heatmap.style >= style_to_implot_colormap.size()) heatmap.style = 0;
				heatmap_style = style_to_implot_colormap[heatmap.style];
				ImPlot::BustColorCache(heatmap_title_id.c_str());
			}
			// Вывод мин-макс значений
			ImGui::SameLine();
			ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");

			ImPlot::PopColormap();

			ImGui::Separator();
			ImGui::EndGroup();
			ImGui::EndChild();

			//{ Проверяем, рисуем графики на той же линии, или переносим на новую
			if (window_size.x > (2*max_heatmap_width) && i < (plot_config.heatmap.size() - 1)) {
				ImGui::SameLine();
			}
			//} Проверяем, рисуем графики на той же линии, или переносим на новую
		} // for i
		//} Отрисовываем все тепловые карты

		// Получаем имя файла
		std::string plot_name;
		if (!plot_config.name.empty()) plot_name = plot_config.name;
		else plot_name = "none";

		if (ImGui::BeginCombo("##PlotName", plot_name.c_str())) {

			ImGui::InputText("File##PlotFileName",
				(char*)plot_config.file_name.c_str(),
				plot_config.file_name.size(),
				ImGuiInputTextFlags_ReadOnly);

			if (!plot_config.note.empty()) {
				ImGui::InputTextMultiline("Description##PlotDescription",
					(char*)plot_config.note.c_str(), plot_config.note.size(),
					ImVec2(0,96), ImGuiInputTextFlags_ReadOnly);
			}
			ImGui::EndCombo();
		}

		ImGui::End();
		//} Конец основного меню

		//{ Работа с файлом

		if(file_dialog_open) {
			ImGui::OpenPopup("Open File##PopupOpenFile");
		}
		if(file_dialog_save) {
			ImGui::OpenPopup("Save File##PopupSaveFile");
		}

		const size_t indent_file_dialog = 64;
		const size_t file_dialog_min_width = 750;
		const size_t file_dialog_min_height = 400;

		if(file_dialog.showFileDialog(
				"Open File##PopupOpenFile",
				imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
				ImVec2((uint64_t)std::max(file_dialog_min_width, window_width - indent_file_dialog),
					(uint64_t)std::max(file_dialog_min_height, window_height - indent_file_dialog)),
				".ep2")){
			plot_config.file_name = file_dialog.selected_path;
			if (!ep2::open_plot(plot_config)) {
				error_popup.open("ERROR", "File opening error!");
			} else {
				is_open_file = true;
			}
		}

		if(file_dialog.showFileDialog(
				"Save File##PopupSaveFile",
				imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
				ImVec2((uint64_t)std::max(file_dialog_min_width, window_width - indent_file_dialog),
					(uint64_t)std::max(file_dialog_min_height, window_height - indent_file_dialog)),
				".ep2")){
			plot_config.file_name = file_dialog.selected_path;
			if (!ep2::save_plot(plot_config)) {
				error_popup.open("ERROR", "File save error!");
			}
		}
		//} Работа с файлом

		error_popup.show_dialog();

		window.clear(config.window_background);
		ImGui::SFML::Render(window);
		window.display();
	}
	ImGui::SaveIniSettingsToDisk(init_file_name.c_str());
	ImPlot::DestroyContext();
	ImGui::SFML::Shutdown();
	return 0;
}
