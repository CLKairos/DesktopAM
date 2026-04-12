#include "uimanager.h"
#include "audiomixer.h"
#include "global.hpp"

static const float TOPBAR_BTN_SIZE = 30.f;
static const float TOPBAR_MARGIN   = 8.f;
static const float TOPBAR_BTN_GAP  = 6.f;
static const float CONTENT_TOP     = 60.f;
static const float CONTENT_MARGIN  = 20.f;

static const float CH_ROW_HEIGHT   = 56.f;
static const float CH_ROW_GAP      = 10.f;
static const float CH_NAME_WIDTH   = 70.f;
static const float CH_MUTE_SIZE    = 28.f;
static const float CH_VOL_WIDTH    = 44.f;
static const float CH_TRACK_HEIGHT = 6.f;
static const float CH_HANDLE_W     = 12.f;
static const float CH_HANDLE_H     = 22.f;

static const float SET_ROW_HEIGHT  = 48.f;
static const float SET_ROW_GAP     = 10.f;
static const float TOGGLE_W        = 40.f;
static const float TOGGLE_H        = 22.f;
static const float TOGGLE_THUMB    = 16.f;

static const float CARD_GAP        = 12.f;

static sf::Color rowBgColor()     { return sf::Color(0, 0, 0, 64);  }
static sf::Color whiteAlpha(int a){ return sf::Color(255, 255, 255, a); }

// ---- Constructor ----
DesktopAM::UIManager::UIManager()
    : window(sf::VideoMode({ (unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT }), WINDOW_TITLE)
{
    if (!mixerIconTexture.loadFromFile("res/icons/mixer_icon.png"))
        std::cout << "Failed to load mixer icon\n";
    if (!menuIconTexture.loadFromFile("res/icons/menu_icon.png"))
        std::cout << "Failed to load menu icon\n";
    if (!settingsIconTexture.loadFromFile("res/icons/settings_icon.png"))
        std::cout << "Failed to load settings icon\n";
    if (!titleFont.openFromFile("res/fonts/Sekuya.ttf"))
        std::cout << "Failed to load title font\n";
    if (!bodyFont.openFromFile("res/fonts/Nunito.ttf"))
        std::cout << "Failed to load body font\n";

    initChannels();
    initToggles();
    initMenuCards();
}

// ---- Init channels ----
void DesktopAM::UIManager::initChannels() {
    struct ChanDef { std::string name; float vol; };
    std::vector<ChanDef> defs = {
        {"Game",   0.80f},
        {"Chat",   0.65f},
        {"Media",  0.50f},
        {"System", 0.40f},
    };

    channels.clear();
    for (auto& d : defs) {
        Channel c;
        c.name   = d.name;
        c.volume = d.vol;
        c.muted  = false;

        c.nameLabel.emplace(bodyFont, d.name, 14);
        c.nameLabel->setFillColor(sf::Color::White);

        c.volLabel.emplace(bodyFont, std::to_string((int)(d.vol * 100)) + "%", 13);
        c.volLabel->setFillColor(whiteAlpha(200));

        c.rowBg.setFillColor(rowBgColor());

        c.track.setFillColor(whiteAlpha(80));
        c.handle.setFillColor(sf::Color::White);

        c.muteBtn.shape.setFillColor(sf::Color::Transparent);
        c.muteBtn.shape.setOutlineColor(whiteAlpha(100));
        c.muteBtn.shape.setOutlineThickness(1.f);
        c.muteBtn.label.emplace(bodyFont, "M", 11);
        c.muteBtn.label->setFillColor(sf::Color::White);

        channels.push_back(std::move(c));
    }
}

// ---- Init toggles ----
void DesktopAM::UIManager::initToggles() {
    struct TogDef { std::string label; bool on; };
    std::vector<TogDef> defs = {
        {"Start minimized",    false},
        {"Show notifications", true },
        {"Run on startup",     false},
    };

    settingsToggles.clear();
    for (auto& d : defs) {
        Toggle t;
        t.label = d.label;
        t.on    = d.on;

        t.labelText.emplace(bodyFont, d.label, 14);
        t.labelText->setFillColor(sf::Color::White);

        t.rowBg.setFillColor(rowBgColor());
        t.thumb.setFillColor(sf::Color::White);

        settingsToggles.push_back(std::move(t));
    }
}

// ---- Init menu cards ----
void DesktopAM::UIManager::initMenuCards() {
    struct CardDef { std::string title; std::string sub; sf::Color color; SceneType target; };
    std::vector<CardDef> defs = {
        {"Mixer",    "Adjust channel volumes",  Colors::mixerBackground,    SceneType::Mixer},
        {"Settings", "Configure audio devices", Colors::settingsBackground, SceneType::Settings},
    };

    menuCards.clear();
    for (auto& d : defs) {
        MenuCard card;
        card.target = d.target;

        card.shape.setFillColor(rowBgColor());
        card.shape.setOutlineColor(whiteAlpha(40));
        card.shape.setOutlineThickness(1.f);

        card.iconBg.setSize({32.f, 32.f});
        card.iconBg.setFillColor(d.color);

        card.titleText.emplace(titleFont, d.title, 15);
        card.titleText->setFillColor(sf::Color::White);

        card.subText.emplace(bodyFont, d.sub, 12);
        card.subText->setFillColor(whiteAlpha(165));

        menuCards.push_back(std::move(card));
    }
}

// ---- Run loop ----
void DesktopAM::UIManager::run() {
    AudioMixer audioMixer;
    audioMixer.init();
    currentScene = SceneType::MainMenu;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                AudioMixer::term();
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f pos(sf::Mouse::getPosition(window));
                handleMousePress(pos);
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                handleMouseRelease();
            }
            if (event->is<sf::Event::MouseMoved>()) {
                sf::Vector2f pos(sf::Mouse::getPosition(window));
                handleMouseMove(pos);
            }
        }

        window.clear(sf::Color::Black);
        renderScene();
        window.display();
    }
}

// ---- Input handling ----
void DesktopAM::UIManager::handleMousePress(sf::Vector2f pos) {
    if (currentScene == SceneType::MainMenu) {
        if (mixerButton.contains(pos))    { currentScene = SceneType::Mixer;    return; }
        if (settingsButton.contains(pos)) { currentScene = SceneType::Settings; return; }
        for (auto& card : menuCards) {
            if (card.contains(pos)) { currentScene = card.target; return; }
        }
    } else {
        if (menuButton.contains(pos)) { currentScene = SceneType::MainMenu; return; }
    }

    if (currentScene == SceneType::Mixer) {
        for (int i = 0; i < (int)channels.size(); i++) {
            auto& c = channels[i];
            if (c.muteBtn.contains(pos)) {
                c.muted = !c.muted;
                c.muteBtn.shape.setFillColor(c.muted ? whiteAlpha(90) : sf::Color::Transparent);
                return;
            }
            if (c.track.getGlobalBounds().contains(pos) || c.handle.getGlobalBounds().contains(pos)) {
                draggingChannel = i;
                float trackLeft  = c.track.getPosition().x;
                float trackRight = trackLeft + c.track.getSize().x;
                float t = (pos.x - trackLeft) / (trackRight - trackLeft);
                c.volume = std::max(0.f, std::min(1.f, t));
                return;
            }
        }
    }

    if (currentScene == SceneType::Settings) {
        for (auto& tog : settingsToggles) {
            if (tog.contains(pos)) {
                tog.on = !tog.on;
                return;
            }
        }
    }
}

void DesktopAM::UIManager::handleMouseRelease() {
    draggingChannel = -1;
}

void DesktopAM::UIManager::handleMouseMove(sf::Vector2f pos) {
    if (draggingChannel < 0) return;
    auto& c = channels[draggingChannel];
    float trackLeft  = c.track.getPosition().x;
    float trackRight = trackLeft + c.track.getSize().x;
    float t = (pos.x - trackLeft) / (trackRight - trackLeft);
    c.volume = std::max(0.f, std::min(1.f, t));
}

// ---- Nav button helper ----
void DesktopAM::UIManager::renderNavButton(Button& btn, sf::Texture& tex, sf::Vector2f pos, sf::Color color) {
    btn.shape.setSize({TOPBAR_BTN_SIZE, TOPBAR_BTN_SIZE});
    btn.shape.setFillColor(color);
    btn.shape.setPosition(pos);

    btn.hasIcon = true;
    btn.icon = sf::Sprite(tex);
    btn.icon->setPosition(pos);
    btn.icon->setScale({
        TOPBAR_BTN_SIZE / tex.getSize().x,
        TOPBAR_BTN_SIZE / tex.getSize().y
    });

    window.draw(btn.shape);
    window.draw(btn.icon.value());
}

// ---- Update channel widget positions ----
void DesktopAM::UIManager::updateChannelWidgets() {
    float rowWidth = WINDOW_WIDTH - CONTENT_MARGIN * 2.f;
    float x = CONTENT_MARGIN;
    float y = CONTENT_TOP + CONTENT_MARGIN;

    for (auto& c : channels) {
        c.rowBg.setSize({rowWidth, CH_ROW_HEIGHT});
        c.rowBg.setPosition({x, y});

        if (c.nameLabel)
            c.nameLabel->setPosition({x + 12.f, y + (CH_ROW_HEIGHT - 14.f) / 2.f});

        float muteX = x + rowWidth - CH_MUTE_SIZE - 12.f;
        float muteY = y + (CH_ROW_HEIGHT - CH_MUTE_SIZE) / 2.f;
        c.muteBtn.shape.setSize({CH_MUTE_SIZE, CH_MUTE_SIZE});
        c.muteBtn.shape.setPosition({muteX, muteY});
        if (c.muteBtn.label) {
            auto lb = c.muteBtn.label->getLocalBounds();
            c.muteBtn.label->setPosition({
                muteX + (CH_MUTE_SIZE - lb.size.x) / 2.f - lb.position.x,
                muteY + (CH_MUTE_SIZE - lb.size.y) / 2.f - lb.position.y
            });
        }

        if (c.volLabel) {
            c.volLabel->setString(std::to_string((int)(c.volume * 100)) + "%");
            float volLabelX = muteX - CH_VOL_WIDTH - 4.f;
            auto vlb = c.volLabel->getLocalBounds();
            c.volLabel->setPosition({
                volLabelX + (CH_VOL_WIDTH - vlb.size.x) / 2.f - vlb.position.x,
                y + (CH_ROW_HEIGHT - vlb.size.y) / 2.f - vlb.position.y
            });

            float trackX = x + CH_NAME_WIDTH + 12.f;
            float trackW = volLabelX - trackX - 8.f;
            float trackY = y + (CH_ROW_HEIGHT - CH_TRACK_HEIGHT) / 2.f;
            c.track.setSize({trackW, CH_TRACK_HEIGHT});
            c.track.setPosition({trackX, trackY});

            float handleX = trackX + c.volume * trackW - CH_HANDLE_W / 2.f;
            float handleY = y + (CH_ROW_HEIGHT - CH_HANDLE_H) / 2.f;
            c.handle.setSize({CH_HANDLE_W, CH_HANDLE_H});
            c.handle.setPosition({handleX, handleY});
        }

        y += CH_ROW_HEIGHT + CH_ROW_GAP;
    }
}

// ---- Update toggle widget positions ----
void DesktopAM::UIManager::updateToggleWidgets() {
    float rowWidth = WINDOW_WIDTH - CONTENT_MARGIN * 2.f;
    float x = CONTENT_MARGIN;
    float y = CONTENT_TOP + CONTENT_MARGIN;

    for (auto& t : settingsToggles) {
        t.rowBg.setSize({rowWidth, SET_ROW_HEIGHT});
        t.rowBg.setPosition({x, y});

        if (t.labelText) {
            auto lb = t.labelText->getLocalBounds();
            t.labelText->setPosition({
                x + 12.f,
                y + (SET_ROW_HEIGHT - lb.size.y) / 2.f - lb.position.y
            });
        }

        float togX = x + rowWidth - TOGGLE_W - 12.f;
        float togY = y + (SET_ROW_HEIGHT - TOGGLE_H) / 2.f;
        t.track.setSize({TOGGLE_W, TOGGLE_H});
        t.track.setPosition({togX, togY});
        t.track.setFillColor(t.on ? whiteAlpha(180) : whiteAlpha(64));

        float thumbX = t.on ? togX + TOGGLE_W - TOGGLE_THUMB - 3.f : togX + 3.f;
        float thumbY = togY + (TOGGLE_H - TOGGLE_THUMB) / 2.f;
        t.thumb.setSize({TOGGLE_THUMB, TOGGLE_THUMB});
        t.thumb.setPosition({thumbX, thumbY});

        y += SET_ROW_HEIGHT + SET_ROW_GAP;
    }
}

// ---- Render ----
void DesktopAM::UIManager::renderScene() {
    switch (currentScene) {
        case SceneType::MainMenu: renderMainMenu(); break;
        case SceneType::Mixer:    renderMixer();    break;
        case SceneType::Settings: renderSettings(); break;
    }
}

void DesktopAM::UIManager::renderMainMenu() {
    sf::RectangleShape bg({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
    bg.setFillColor(Colors::menuBackground);
    window.draw(bg);

    sf::Text title(titleFont, "Desktop Audio Mixer", 22);
    title.setFillColor(sf::Color::White);
    title.setPosition({16.f, 16.f});
    window.draw(title);

    float cardW = (WINDOW_WIDTH - CONTENT_MARGIN * 2.f - CARD_GAP) / 2.f;
    float cardH = 120.f;
    float cardY = CONTENT_TOP + CONTENT_MARGIN;

    // Pair each card with its icon texture
    sf::Texture* cardTextures[] = { &mixerIconTexture, &settingsIconTexture };

    for (int i = 0; i < (int)menuCards.size(); i++) {
        auto& card = menuCards[i];
        float cardX = CONTENT_MARGIN + i * (cardW + CARD_GAP);

        card.shape.setSize({cardW, cardH});
        card.shape.setPosition({cardX, cardY});
        window.draw(card.shape);

        // Icon background
        card.iconBg.setPosition({cardX + 16.f, cardY + 16.f});
        window.draw(card.iconBg);

        // Icon sprite inside the icon background
        sf::Sprite cardIcon(*cardTextures[i]);
        cardIcon.setPosition({cardX + 16.f, cardY + 16.f});
        cardIcon.setScale({
            32.f / cardTextures[i]->getSize().x,
            32.f / cardTextures[i]->getSize().y
        });
        window.draw(cardIcon);

        if (card.titleText) {
            card.titleText->setPosition({cardX + 16.f, cardY + 58.f});
            window.draw(card.titleText.value());
        }
        if (card.subText) {
            card.subText->setPosition({cardX + 16.f, cardY + 78.f});
            window.draw(card.subText.value());
        }
    }
}

void DesktopAM::UIManager::renderMixer() {
    sf::RectangleShape bg({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
    bg.setFillColor(Colors::mixerBackground);
    window.draw(bg);

    sf::Text title(titleFont, "Mixer", 22);
    title.setFillColor(sf::Color::White);
    title.setPosition({16.f, 16.f});
    window.draw(title);

    float btnX = WINDOW_WIDTH - TOPBAR_MARGIN - TOPBAR_BTN_SIZE;
    renderNavButton(menuButton, menuIconTexture, {btnX, TOPBAR_MARGIN}, Colors::menuBackground);

    updateChannelWidgets();
    for (auto& c : channels) {
        window.draw(c.rowBg);
        if (c.nameLabel) window.draw(c.nameLabel.value());

        window.draw(c.track);

        float fillW = c.volume * c.track.getSize().x;
        sf::RectangleShape fill({fillW, CH_TRACK_HEIGHT});
        fill.setPosition(c.track.getPosition());
        fill.setFillColor(whiteAlpha(200));
        window.draw(fill);

        window.draw(c.handle);
        if (c.volLabel)  window.draw(c.volLabel.value());
        window.draw(c.muteBtn.shape);
        if (c.muteBtn.label) window.draw(c.muteBtn.label.value());
    }
}

void DesktopAM::UIManager::renderSettings() {
    sf::RectangleShape bg({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
    bg.setFillColor(Colors::settingsBackground);
    window.draw(bg);

    sf::Text title(titleFont, "Settings", 22);
    title.setFillColor(sf::Color::White);
    title.setPosition({16.f, 16.f});
    window.draw(title);

    float btnX = WINDOW_WIDTH - TOPBAR_MARGIN - TOPBAR_BTN_SIZE;
    renderNavButton(menuButton, menuIconTexture, {btnX, TOPBAR_MARGIN}, Colors::menuBackground);

    updateToggleWidgets();
    for (auto& t : settingsToggles) {
        window.draw(t.rowBg);
        if (t.labelText) window.draw(t.labelText.value());
        window.draw(t.track);
        window.draw(t.thumb);
    }
}