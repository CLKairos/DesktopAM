#include <SFML/Graphics.hpp>
#include <iostream>

namespace DesktopAM {
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const std::string WINDOW_TITLE = "Desktop Audio Mixer";

	namespace Colors {
		sf::Color menuBackground(150, 150, 150); // Gray
		sf::Color mixerBackground(255, 0, 0); // Red
		sf::Color settingsBackground(0, 0, 255); // Blue
	}
}