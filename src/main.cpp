#pragma execution_character_set("utf-8")
#include <SFML/Graphics.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "ScreenManager.hpp"
#include "Lab3Demo.hpp"

int main(int argc, char* argv[]) {
    // Поддержка запуска консольной демонстрации Лабораторной работы 3
    if (argc > 1 && (std::string(argv[1]) == "--lab3" || std::string(argv[1]) == "--demo")) {
        Lab3::runLab3Demo();
        return 0;
    }
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "RPG Game System", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        return -1;
    }

    // Применяем общее стилизованное оформление интерфейса
    ScreenManager::applyGameStyle();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Очищаем дефолтные шрифты
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!defaultFont) {
        defaultFont = io.Fonts->AddFontDefault();
    }
    ImFont* menuFont = io.Fonts->AddFontFromFileTTF("assets/fonts/MainMenu.ttf", 48.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!menuFont) {
        menuFont = defaultFont;
    }
    if (!ImGui::SFML::UpdateFontTexture()) {
        std::cerr << "Не удалось обновить текстуру шрифта ImGui!" << std::endl;
    }

    ScreenManager screenManager;
    screenManager.mainMenuFont = menuFont;
    screenManager.loadResources();

    sf::Clock deltaClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            // Обработка клавиш в ScreenManager (включая ~ для консоли)
            screenManager.processEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
            }
        }

        screenManager.update(window);
        ImGui::SFML::Update(window, deltaClock.restart());

        screenManager.renderCurrentScreen();

        window.clear(sf::Color(10, 10, 16));
        screenManager.renderBackground(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
