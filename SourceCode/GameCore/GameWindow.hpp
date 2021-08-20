#ifndef FENGINE_SC_WindowHandler
#define FENGINE_SC_WindowHandler

#include <SFML/Graphics.hpp>
#include "UseFull/SFMLUp/View.hpp"

struct GameWindow {
	inline static const char * name = "QP version FE 12";
	inline static sf::RenderWindow window;

	inline static size_t limit_fps = 60;

	inline static sf::Clock clock;
	inline static double current_time = 0;
	inline static double current_fps = 0;

	inline static size_t width = 800;
	inline static size_t height = 600;

	inline static size_t video_mode = sf::Style::Close;

	static double calcFps() {
		current_time = clock.restart().asSeconds();
		current_fps = (double)1.0 / current_time;
		return current_fps;
	}

	static void create(size_t mode) {
		video_mode = mode;
		window.create(sf::VideoMode(width, height), name, mode);
		window.setFramerateLimit(limit_fps);
	}

	static void loadWindowSettings() {

	}

	static void resetView(sfup::CoCustomView auto & cview) {
		cview.reset(&window, width, height);
	}
};

#endif