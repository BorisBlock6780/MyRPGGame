#include <SFML/Graphics.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "ScreenManager.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "RPG Game System");
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        return -1;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear(); // Очищаем дефолтные шрифты

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!ImGui::SFML::UpdateFontTexture()) {
        std::cerr << "Не удалось обновить текстуру шрифта ImGui!" << std::endl;
    }
    ScreenManager screenManager;
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
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        screenManager.update(window);
        screenManager.renderCurrentScreen();

        window.clear(sf::Color(15, 15, 15));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}