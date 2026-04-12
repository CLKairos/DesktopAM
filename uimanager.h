#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>
#include <vector>
#include <string>

namespace DesktopAM {
	class UIManager
	{
	public:
		UIManager();
		void run();

	private:
		sf::RenderWindow window;
		sf::Font titleFont;
		sf::Font bodyFont;

		enum class SceneType {
			MainMenu,
			Mixer,
			Settings
		};

		struct Button {
			sf::RectangleShape shape;
			bool hasIcon = false;
			std::optional<sf::Sprite> icon;
			std::optional<sf::Text> label;

			bool contains(sf::Vector2f pos) const {
				return shape.getGlobalBounds().contains(pos);
			}
		};

		struct Channel {
			std::string name;
			float volume = 0.8f;
			bool muted = false;

			sf::RectangleShape rowBg;
			sf::RectangleShape track;
			sf::RectangleShape handle;
			Button muteBtn;

			std::optional<sf::Text> nameLabel;
			std::optional<sf::Text> volLabel;
		};

		struct Toggle {
			std::string label;
			bool on = false;

			sf::RectangleShape rowBg;
			sf::RectangleShape track;
			sf::RectangleShape thumb;
			std::optional<sf::Text> labelText;

			bool contains(sf::Vector2f pos) const {
				return track.getGlobalBounds().contains(pos);
			}
		};

		struct MenuCard {
			sf::RectangleShape shape;
			sf::RectangleShape iconBg;
			std::optional<sf::Text> titleText;
			std::optional<sf::Text> subText;
			SceneType target = SceneType::MainMenu;

			bool contains(sf::Vector2f pos) const {
				return shape.getGlobalBounds().contains(pos);
			}
		};

		std::vector<Channel> channels;
		std::vector<Toggle> settingsToggles;
		std::vector<MenuCard> menuCards;

		Button mixerButton;
		sf::Texture mixerIconTexture;
		Button menuButton;
		sf::Texture menuIconTexture;
		Button settingsButton;
		sf::Texture settingsIconTexture;

		SceneType currentScene = SceneType::MainMenu;
		int draggingChannel = -1;

		void initChannels();
		void initToggles();
		void initMenuCards();
		void handleMousePress(sf::Vector2f pos);
		void handleMouseRelease();
		void handleMouseMove(sf::Vector2f pos);
		void renderScene();
		void renderMainMenu();
		void renderMixer();
		void renderSettings();
		void renderNavButton(Button& btn, sf::Texture& tex, sf::Vector2f pos, sf::Color color);
		void updateChannelWidgets();
		void updateToggleWidgets();
	};
}