#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>

namespace DesktopAM {
	class UIManager
	{
	public:
		UIManager();
		void run();
	private:
		//sfml objects
		sf::RenderWindow window;
		sf::Font titleFont;
		sf::Font bodyFont;

		//button struct
		struct Button {
			sf::RectangleShape shape;
			bool hasIcon = false;
			std::optional<sf::Sprite> icon;
			std::optional<sf::Text> label;

			bool contains(sf::Vector2f pos) {
				return shape.getGlobalBounds().contains(pos);
			}
		};

		//button inits
		Button mixerButton;
		sf::Texture mixerIconTexture;
		Button menuButton;
		sf::Texture menuIconTexture;
		Button settingsButton;
		sf::Texture settingsIconTexture;
		
		//scene management
		enum class SceneType {
			MainMenu,
			Mixer,
			Settings
		};
		SceneType currentScene;

		//functions
		void renderScene();
	};
}