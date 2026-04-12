#include "uimanager.h"
#include "audiomixer.h"
#include "global.hpp"

DesktopAM::UIManager::UIManager() : window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Desktop Audio Mixer") {
	if (!mixerIconTexture.loadFromFile("res/icons/mixer_icon.png")) {
		std::cout << "Failed to load mixer icon texture!" << std::endl;
	}
	if (!menuIconTexture.loadFromFile("res/icons/menu_icon.png")) {
		std::cout << "Failed to load mixer icon texture!" << std::endl;
	}
	if (!settingsIconTexture.loadFromFile("res/icons/settings_icon.png")) {
		std::cout << "Failed to load mixer icon texture!" << std::endl;
	}
	if (!titleFont.openFromFile("res/fonts/Sekuya.ttf")) {
		std::cout << "Failed to load title font!" << std::endl;
	}
	if (!bodyFont.openFromFile("res/fonts/Nunito.ttf")) {
		std::cout << "Failed to load body font!" << std::endl;
	}
}

void DesktopAM::UIManager::run() {
	UIManager();
	AudioMixer audioMixer;
	audioMixer.init();

	currentScene = SceneType::MainMenu;

	while (window.isOpen()) {
		// Process events
        while (const std::optional event = window.pollEvent())
        {
			//if (event->is<sf::Event::KeyPressed>())

			if (event->is<sf::Event::MouseButtonPressed>()) {
				sf::Vector2f mousePos(sf::Mouse::getPosition(window));

				if (settingsButton.contains(mousePos) && currentScene == SceneType::MainMenu) {
					currentScene = SceneType::Settings;
				}

				if (mixerButton.contains(mousePos) && currentScene == SceneType::MainMenu) {
					currentScene = SceneType::Mixer;
				}
				else if (menuButton.contains(mousePos) && currentScene != SceneType::MainMenu) {
					currentScene = SceneType::MainMenu;
				}
			}

            // Close window: exit
			if (event->is<sf::Event::Closed>()) {
				DesktopAM::AudioMixer::term();
				window.close();
			}
        }
		window.clear(sf::Color::Black);

		renderScene();

		window.display();
	}
}

void DesktopAM::UIManager::renderScene() {
	// Render the current scene based on the value of currentScene
	
	//global menu button
	sf::Vector2f menuButtonSize(30.f, 30.f);
	sf::Vector2f menuButtonPos((WINDOW_WIDTH - (menuButtonSize.x + 10 /* margin */)), 5.f);
	menuButton.shape.setSize(menuButtonSize);
	menuButton.shape.setFillColor(Colors::menuBackground);
	menuButton.shape.setPosition(menuButtonPos);
	menuButton.hasIcon = true;
	menuButton.icon = sf::Sprite(menuIconTexture);
	menuButton.icon->setPosition(menuButtonPos);
	menuButton.icon->setScale({
		menuButtonSize.x / menuIconTexture.getSize().x,
		menuButtonSize.y / menuIconTexture.getSize().y
	});

	//Main Menu Assets
	sf::RectangleShape mainMenuBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	mainMenuBackground.setFillColor(Colors::menuBackground);
	sf::Text menuTitle(titleFont, "Menu Scene");
	menuTitle.setCharacterSize(24);
	menuTitle.setFillColor(sf::Color::White);

	//mixer button (only on main menu)
	sf::Vector2f mixerButtonSize(30.f, 30.f);
	sf::Vector2f mixerButtonPos((WINDOW_WIDTH - ( mixerButtonSize.x + 10 /* margin */)), 5.f);
	mixerButton.shape.setSize(mixerButtonSize);
	mixerButton.shape.setFillColor(Colors::mixerBackground);
	mixerButton.shape.setPosition(mixerButtonPos);
	mixerButton.hasIcon = true;
	mixerButton.icon = sf::Sprite(mixerIconTexture);
	mixerButton.icon->setPosition(mixerButtonPos);
	mixerButton.icon->setScale({
		mixerButtonSize.x / mixerIconTexture.getSize().x,
		mixerButtonSize.y / mixerIconTexture.getSize().y
	});

	//settings button (only on main menu scene)
	sf::Vector2f settingsButtonSize(30.f, 30.f);
	sf::Vector2f settingsButtonPos((WINDOW_WIDTH - (( settingsButtonSize.x + mixerButtonSize.x) + 20 /* margin */)), 5.f);
	settingsButton.shape.setSize(settingsButtonSize);
	settingsButton.shape.setFillColor(Colors::settingsBackground);
	settingsButton.shape.setPosition(settingsButtonPos);
	settingsButton.hasIcon = true;
	settingsButton.icon = sf::Sprite(settingsIconTexture);
	settingsButton.icon->setPosition(settingsButtonPos);
	settingsButton.icon->setScale({
		settingsButtonSize.x / settingsIconTexture.getSize().x,
		settingsButtonSize.y / settingsIconTexture.getSize().y
		});

	//Mixer Assets
	sf::RectangleShape mixerBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	mixerBackground.setFillColor(Colors::mixerBackground);
	sf::Text mixerTitle(titleFont, "Mixer Scene");
	mixerTitle.setCharacterSize(24);
	mixerTitle.setFillColor(sf::Color::White);

	//Settings Assets
	sf::RectangleShape settingsBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	settingsBackground.setFillColor(Colors::settingsBackground);
	sf::Text settingsTitle(titleFont, "Settings Scene");
	settingsTitle.setCharacterSize(24);
	settingsTitle.setFillColor(sf::Color::White);

	switch (currentScene) {
		case SceneType::MainMenu:
			window.draw(mainMenuBackground);
			window.draw(menuTitle);
			window.draw(mixerButton.shape);
			if (mixerButton.hasIcon) {
				window.draw(mixerButton.icon.value());
			}
			window.draw(settingsButton.shape);
			if (settingsButton.hasIcon) {
				window.draw(settingsButton.icon.value());
			}
			break;
		case SceneType::Mixer:
			window.draw(mixerBackground);
			window.draw(mixerTitle);
			window.draw(menuButton.shape);
			if (menuButton.hasIcon) {
				window.draw(menuButton.icon.value());
			}
			break;
		case SceneType::Settings:
			window.draw(settingsBackground);
			window.draw(settingsTitle);
			window.draw(menuButton.shape);
			if (menuButton.hasIcon) {
				window.draw(menuButton.icon.value());
			}
			break;
	}
}