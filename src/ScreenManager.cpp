#include "ScreenManager.hpp"
#include <iostream>

bool ScreenManager::loadResources() {
    std::unordered_map<ItemTab, std::string> paths = {
        {ItemTab::Weapons,    "assets/icons/weapon.png"},
        {ItemTab::Artifacts,  "assets/icons/artifact.png"},
        {ItemTab::Food,       "assets/icons/food.png"},
        {ItemTab::Materials,  "assets/icons/materials.png"},
        {ItemTab::Jewelry,    "assets/icons/jewelry.png"},
        {ItemTab::QuestItems, "assets/icons/quest.png"}
    };

    for (const auto& [tab, path] : paths) {
        if (!tabTextures[tab].loadFromFile(path)) {
            std::cerr << "[ScreenManager] Иконка не найдена: " << path << std::endl;
        }
    }
    return true;
}

// Обработка переключения консоли по клавише ~ (Tilde)
void ScreenManager::processEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tilde) {
            isConsoleOpen = !isConsoleOpen;
        }
    }
}

void ScreenManager::update(sf::RenderWindow& window) {
    if (shouldQuit) {
        window.close();
    }
}

void ScreenManager::renderCurrentScreen() {
    // 1. Рендерим основной экран во весь размер
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    std::string windowTitle = "ScreenContainer##" + std::to_string(static_cast<int>(currentScreen));

    ImGui::Begin(windowTitle.c_str(), nullptr, windowFlags);

    switch (currentScreen) {
    case ScreenState::MainMenu:               renderMainMenu(); break;
    case ScreenState::Settings:               renderSettings(); break;
    case ScreenState::Saves:                  renderSaves(); break;
    case ScreenState::Lobby:                  renderLobby(); break;
    case ScreenState::Shop:                   renderShop(); break;
    case ScreenState::Inventory:              renderInventory(); break;
    case ScreenState::Map:                    renderMap(); break;
    case ScreenState::Achievements:           renderAchievements(); break;
    case ScreenState::SquadEditor:            renderSquadEditor(); break;
    case ScreenState::CharacterEditor:        renderCharacterEditor(); break;
    case ScreenState::Combat:                 renderCombat(); break;
    case ScreenState::EnvironmentInteraction: renderEnvironmentInteraction(); break;
    }

    renderBottomNavigation();

    ImGui::End();

    // 2. Рендерим консоль поверх основного экрана (если она открыта)
    if (isConsoleOpen) {
        renderConsole();
    }
}

// --- ОТРИСОВКА КОНСОЛИ ---
void ScreenManager::renderConsole() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Консоль компактнее основного экрана (например, верхняя часть экрана)
    ImVec2 consoleSize = ImVec2(displaySize.x * 0.8f, 250.0f);
    ImVec2 consolePos = ImVec2((displaySize.x - consoleSize.x) * 0.5f, 20.0f);

    ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

    ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize;

    // Окно консоли с полупрозрачным фоном
    ImGui::Begin("Дев-Консоль (Нажмите ~ чтобы закрыть)", &isConsoleOpen, consoleFlags);

    ImGui::Text("Консоль разработчика активна.");
    ImGui::Separator();

    // Место для будущей истории вывода команд
    ImGui::BeginChild("ConsoleLog", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[Система] Введите команду ниже...");
    ImGui::EndChild();

    // Поле ввода текста с клавиатуры
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##ConsoleInput", consoleInputBuf, IM_ARRAYSIZE(consoleInputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        // Тут будет обработка введенного текста
        consoleInputBuf[0] = '\0'; // Очищаем буфер после ввода
        ImGui::SetKeyboardFocusHere(-1); // Оставляем фокус ввода на поле
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

// --- ЭКРАН ИНВЕНТАРЯ ---
void ScreenManager::renderInventory() {
    std::string activeTabName = "";
    switch (currentInventoryTab) {
    case ItemTab::Weapons:    activeTabName = "Оружие"; break;
    case ItemTab::Artifacts:  activeTabName = "Артефакты"; break;
    case ItemTab::Food:       activeTabName = "Еда"; break;
    case ItemTab::Materials:  activeTabName = "Материалы"; break;
    case ItemTab::Jewelry:    activeTabName = "Драгоценности"; break;
    case ItemTab::QuestItems: activeTabName = "Квестовые предметы"; break;
    }

    ImGui::Text("=== ИНВЕНТАРЬ | Вкладка: %s ===", activeTabName.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ImVec2 iconSize = ImVec2(48.0f, 48.0f);
    struct TabButtonData { ItemTab tab; const char* strId; };
    TabButtonData inventoryTabs[] = {
        {ItemTab::Weapons,    "##InvWeapon"},
        {ItemTab::Artifacts,  "##InvArtifact"},
        {ItemTab::Food,       "##InvFood"},
        {ItemTab::Materials,  "##InvMaterials"},
        {ItemTab::Jewelry,    "##InvJewelry"},
        {ItemTab::QuestItems, "##InvQuest"}
    };

    for (int i = 0; i < 6; ++i) {
        if (i > 0) ImGui::SameLine();
        ItemTab tab = inventoryTabs[i].tab;
        ImTextureID textureId = (ImTextureID)(intptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentInventoryTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));

        if (ImGui::ImageButton(inventoryTabs[i].strId, textureId, iconSize)) {
            currentInventoryTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Содержимое инвентаря для категории «%s»...", activeTabName.c_str());
}

// --- ЭКРАН МАГАЗИНА ---
void ScreenManager::renderShop() {
    std::string activeTabName = "";
    switch (currentShopTab) {
    case ItemTab::Weapons:   activeTabName = "Оружие"; break;
    case ItemTab::Artifacts: activeTabName = "Артефакты"; break;
    case ItemTab::Food:      activeTabName = "Еда"; break;
    case ItemTab::Materials: activeTabName = "Материалы"; break;
    case ItemTab::Jewelry:   activeTabName = "Драгоценности"; break;
    default: break;
    }

    ImGui::Text("=== МАГАЗИН | Вкладка: %s ===", activeTabName.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ImVec2 iconSize = ImVec2(48.0f, 48.0f);
    struct TabButtonData { ItemTab tab; const char* strId; };
    TabButtonData shopTabs[] = {
        {ItemTab::Weapons,   "##ShopWeapon"},
        {ItemTab::Artifacts, "##ShopArtifact"},
        {ItemTab::Food,      "##ShopFood"},
        {ItemTab::Materials, "##ShopMaterials"},
        {ItemTab::Jewelry,   "##ShopJewelry"}
    };

    for (int i = 0; i < 5; ++i) {
        if (i > 0) ImGui::SameLine();
        ItemTab tab = shopTabs[i].tab;
        ImTextureID textureId = (ImTextureID)(intptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentShopTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));

        if (ImGui::ImageButton(shopTabs[i].strId, textureId, iconSize)) {
            currentShopTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Товары магазина категории «%s»...", activeTabName.c_str());
}

// --- НАВИГАЦИОННЫЕ КНОПКИ НИЗА ЭКРАНА ---
void ScreenManager::renderBottomNavigation() {
    if (currentScreen == ScreenState::MainMenu) return;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 buttonSize = ImVec2(180.0f, 40.0f);
    float padding = 20.0f;
    float spacing = 10.0f;

    ImGui::SetCursorPos(ImVec2(padding, displaySize.y - buttonSize.y - padding));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.80f, 0.15f, 0.15f, 1.0f));
    if (ImGui::Button("<- Главное меню", buttonSize)) {
        currentScreen = ScreenState::MainMenu;
    }
    ImGui::PopStyleColor();

    if (currentScreen != ScreenState::Lobby) {
        ImGui::SetCursorPos(ImVec2(padding + buttonSize.x + spacing, displaySize.y - buttonSize.y - padding));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
        if (ImGui::Button("<- В Лобби", buttonSize)) {
            currentScreen = ScreenState::Lobby;
        }
        ImGui::PopStyleColor();
    }
}

// --- ОСТАЛЬНЫЕ ЭКРАНЫ ---
void ScreenManager::renderMainMenu() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 buttonSize = ImVec2(240.0f, 50.0f);
    float spacing = 15.0f;
    int buttonCount = 4;

    float totalHeight = (buttonSize.y * buttonCount) + (spacing * (buttonCount - 1));
    float startX = (displaySize.x - buttonSize.x) * 0.5f;
    float startY = (displaySize.y - totalHeight) * 0.5f;

    ImGui::SetCursorPos(ImVec2(startX, startY));

    if (ImGui::Button("Начать игру", buttonSize)) currentScreen = ScreenState::Lobby;
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Загрузить игру", buttonSize)) currentScreen = ScreenState::Saves;
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Настройки", buttonSize)) currentScreen = ScreenState::Settings;
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Выход", buttonSize)) shouldQuit = true;
}

void ScreenManager::renderLobby() {
    ImGui::Text("=== ИГРОВОЕ ЛОББИ (ХАБ) ===");
    ImGui::Separator();

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 buttonSize = ImVec2(220.0f, 45.0f);
    float spacingX = 20.0f;
    float spacingY = 15.0f;

    struct NavTarget { const char* label; ScreenState state; };
    NavTarget targets[] = {
        {"Редактор отряда",    ScreenState::SquadEditor},
        {"Редактор персонажа", ScreenState::CharacterEditor},
        {"Инвентарь",          ScreenState::Inventory},
        {"Магазин",            ScreenState::Shop},
        {"Карта",              ScreenState::Map},
        {"Экран боя",          ScreenState::Combat},
        {"Окружение",          ScreenState::EnvironmentInteraction},
        {"Достижения",         ScreenState::Achievements},
        {"Сохранения",         ScreenState::Saves},
        {"Настройки",          ScreenState::Settings}
    };

    int cols = 2;
    float totalWidth = (buttonSize.x * cols) + spacingX;
    float totalHeight = (buttonSize.y * 5) + (spacingY * 4);

    float startX = (displaySize.x - totalWidth) * 0.5f;
    float startY = (displaySize.y - totalHeight) * 0.5f;

    for (int i = 0; i < 10; ++i) {
        int row = i / cols;
        int col = i % cols;
        ImGui::SetCursorPos(ImVec2(startX + col * (buttonSize.x + spacingX), startY + row * (buttonSize.y + spacingY)));
        if (ImGui::Button(targets[i].label, buttonSize)) {
            currentScreen = targets[i].state;
        }
    }
}

void ScreenManager::renderSettings() { 
    ImGui::Text("=== НАСТРОЙКИ ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderSaves() { 
    ImGui::Text("=== ЭКРАН СОХРАНЕНИЙ ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderMap() {
    ImGui::Text("=== КАРТА ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderAchievements() {
    ImGui::Text("=== ДОСТИЖЕНИЯ ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderSquadEditor() {
    ImGui::Text("=== РЕДАКТОР ОТРЯДА ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderCharacterEditor() {
    ImGui::Text("=== РЕДАКТОР ПЕРСОНАЖА ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderCombat() {
    ImGui::Text("=== ЭКРАН БОЯ ==="); ImGui::Separator(); 
}
void ScreenManager::renderEnvironmentInteraction() {
    ImGui::Text("=== ВЗАИМОДЕЙСТВИЕ С ОКРУЖЕНИЕМ ===");
    ImGui::Separator(); 
}