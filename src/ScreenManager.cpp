#pragma execution_character_set("utf-8")
#include "ScreenManager.hpp"
#include "Database.hpp"
#include <iostream>
#include <imgui.h>
#include <algorithm>
#include "PlayerData.hpp"

bool ScreenManager::loadResources() {
    if (!mainMenuBgTexture.loadFromFile("assets/bg/MainMenu.png")) {
        std::cerr << "[ScreenManager] Ошибка: фон assets/bg/MainMenu.png не найден!" << std::endl;
    }
    else {
        mainMenuBgSprite.setTexture(mainMenuBgTexture);
    }
    
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
    nodeIconTextures["shop"].loadFromFile("assets/icons/jewelry.png");
    nodeIconTextures["combat"].loadFromFile("assets/icons/weapon.png");
    nodeIconTextures["resource"].loadFromFile("assets/icons/materials.png");
    nodeIconTextures["cooking"].loadFromFile("assets/icons/food.png");
    return true;

}

void ScreenManager::initMapPoints() {
    mapPoints.clear();

    // =========================================================================
    // ЭТАЖ 1: НЕКРОПОЛЬ
    // =========================================================================

    // 1. Постоянная точка: Торговец 1-го этажа
    MapPointOfInterest scrapMerchant;
    scrapMerchant.id = "f1_merchant_scrap";
    scrapMerchant.title = "Лавка старьевщика";
    scrapMerchant.description = "Скупает лом и продает базовое снаряжение Некрополя.";
    scrapMerchant.floor = 1;
    scrapMerchant.position = ImVec2(320.0f, 420.0f); // Ручная настройка координат X, Y
    scrapMerchant.type = MapNodeType::Shop;
    scrapMerchant.iconKey = "shop";
    scrapMerchant.isTemporary = false; // Постоянная точка
    scrapMerchant.merchantName = "Старьевщик Некрополя";
    scrapMerchant.merchantInventory.setBalance(350);
    scrapMerchant.merchantInventory.addItem("rusty_blade", 2);
    scrapMerchant.merchantInventory.addItem("iron_ore", 10);
    scrapMerchant.merchantInventory.addItem("bone_meal", 6);
    scrapMerchant.merchantInventory.addItem("dirty_water", 8);
    mapPoints.push_back(scrapMerchant);

    // 2. Временная точка: Засада патруля (Бой)
    MapPointOfInterest ratAmbush;
    ratAmbush.id = "f1_combat_rats";
    ratAmbush.title = "Патруль падальщиков";
    ratAmbush.description = "Стая голодных крыс и скелетов преграждает путь к руинам.\nПосле победы точка исчезнет с карты.";
    ratAmbush.floor = 1;
    ratAmbush.position = ImVec2(580.0f, 300.0f);
    ratAmbush.type = MapNodeType::Combat;
    ratAmbush.iconKey = "combat";
    ratAmbush.isTemporary = true; // Временная точка — скроется после перехода
    ratAmbush.enemies = {
        {"necropolis_rat", "Пепельная крыса", 1, 3},
        {"skeleton_scout", "Скелет-разведчик", 2, 1}
    };
    mapPoints.push_back(ratAmbush);

    // 3. Временная точка: Добыча ресурсов
    MapPointOfInterest ashCrater;
    ashCrater.id = "f1_resource_crater";
    ashCrater.title = "Пепельный кратер";
    ashCrater.description = "Скопление светящегося мха и костной муки. Одноразовая точка сбора.";
    ashCrater.floor = 1;
    ashCrater.position = ImVec2(820.0f, 460.0f);
    ashCrater.type = MapNodeType::Environment;
    ashCrater.iconKey = "resource";
    ashCrater.isTemporary = true; // Исчезнет после взаимодействия
    mapPoints.push_back(ashCrater);

    // =========================================================================
    // ЭТАЖ 2: ОАЗИС ТИШИНЫ
    // =========================================================================

    // 1. Постоянная точка: Торговец провизией 2-го этажа
    MapPointOfInterest oasisMerchant;
    oasisMerchant.id = "f2_merchant_food";
    oasisMerchant.title = "Торговец Оазиса";
    oasisMerchant.description = "Продает свежее молоко, рыбу и тепличные яблоки.";
    oasisMerchant.floor = 2;
    oasisMerchant.position = ImVec2(400.0f, 360.0f);
    oasisMerchant.type = MapNodeType::Shop;
    oasisMerchant.iconKey = "shop";
    oasisMerchant.isTemporary = false;
    oasisMerchant.merchantName = "Хранитель теплиц";
    oasisMerchant.merchantInventory.setBalance(600);
    oasisMerchant.merchantInventory.addItem("cave_wheat", 12);
    oasisMerchant.merchantInventory.addItem("goat_milk", 8);
    oasisMerchant.merchantInventory.addItem("gutted_fish", 5);
    oasisMerchant.merchantInventory.addItem("wild_berries", 15);
    oasisMerchant.merchantInventory.addItem("greenhouse_apple", 10);
    mapPoints.push_back(oasisMerchant);

    // 2. Временная точка: Охрана водоема (Бой)
    MapPointOfInterest lakeGuards;
    lakeGuards.id = "f2_combat_lake";
    lakeGuards.title = "Озерные хищники";
    lakeGuards.description = "Опасные твари у берега подземного озера.";
    lakeGuards.floor = 2;
    lakeGuards.position = ImVec2(720.0f, 260.0f);
    lakeGuards.type = MapNodeType::Combat;
    lakeGuards.iconKey = "combat";
    lakeGuards.isTemporary = true;
    lakeGuards.enemies = {
        {"lake_lurker", "Озерный удильщик", 3, 2},
        {"overgrown_golem", "Заросший страж", 4, 1}
    };
    mapPoints.push_back(lakeGuards);
}

void ScreenManager::renderBackground(sf::RenderWindow& window) {
    if (currentScreen == ScreenState::MainMenu) {
        sf::Vector2u texSize = mainMenuBgTexture.getSize();
        sf::Vector2u winSize = window.getSize();

        if (texSize.x > 0 && texSize.y > 0) {
            mainMenuBgSprite.setScale(
                static_cast<float>(winSize.x) / texSize.x,
                static_cast<float>(winSize.y) / texSize.y
            );
        }
        window.draw(mainMenuBgSprite);
    }
}

ScreenManager::ScreenManager() {
    // Инициализация базовой базы данных при создании менеджера
    GameDatabase::getInstance().init();
}
// Обработка переключения консоли по клавише ~ (Tilde)
void ScreenManager::processEvent(const sf::Event& event) {
    // Если открыт диалог — перехватываем нажатия клавиш только для него
    if (isDialogueOpen) {
        if (event.type == sf::Event::KeyPressed) {
            if (currentDialogueIndex >= 0 && currentDialogueIndex < static_cast<int>(currentDialogue.size())) {
                const auto& node = currentDialogue[currentDialogueIndex];
                // Переход по любой клавише работает только тогда, когда не нужно выбирать ответ
                if (node.options.empty()) {
                    advanceDialogue(node.nextNodeIndex);
                }
            }
        }
        return; // Блокируем открытие консоли и прочие клавиши во время диалога
    }

    // Обычная обработка клавиши ~ для консоли, когда диалог закрыт
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tilde) {
            isConsoleOpen = !isConsoleOpen;
        }
    }
}
// -- ДИАЛОГ --
void ScreenManager::advanceDialogue(int nextIndex) {
    if (nextIndex == END_DIALOGUE) {
        isDialogueOpen = false;
        currentDialogue.clear();
        currentDialogueIndex = 0;
        return;
    }

    if (nextIndex == NEXT_SEQUENTIAL) {
        currentDialogueIndex++;
    }
    else {
        currentDialogueIndex = nextIndex;
    }

    // Если дошли до конца списка фраз — закрываем окно
    if (currentDialogueIndex < 0 || currentDialogueIndex >= static_cast<int>(currentDialogue.size())) {
        isDialogueOpen = false;
        currentDialogue.clear();
        currentDialogueIndex = 0;
    }
}

void ScreenManager::renderDialogue() {
    if (currentDialogueIndex < 0 || currentDialogueIndex >= static_cast<int>(currentDialogue.size())) {
        isDialogueOpen = false;
        return;
    }

    const auto& node = currentDialogue[currentDialogueIndex];
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Размеры и позиция: окно в нижней части экрана с отступами по краям
    float marginX = 40.0f;
    float marginBottom = 25.0f;
    float windowHeight = 230.0f;
    ImVec2 windowSize = ImVec2(displaySize.x - (marginX * 2.0f), windowHeight);
    ImVec2 windowPos = ImVec2(marginX, displaySize.y - windowHeight - marginBottom);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowFocus(); // Держим фокус поверх остальных окон

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    // Стилизация под темное диалоговое окно с контрастной рамкой
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.09f, 0.12f, 0.96f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.65f, 0.55f, 0.30f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 15.0f));

    ImGui::Begin("##DialogueOverlayWindow", nullptr, flags);

    SpeakerInfo speaker = getSpeakerInfo(node.speaker);

    // 1. Верхняя часть окна: Имя говорящего персонажа в его уникальном цвете
    ImGui::TextColored(speaker.color, "[ %s ]", speaker.name.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // 2. Основная часть: Текст реплики с автоматическим переносом строк
    ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 25.0f);
    ImGui::TextUnformatted(node.text.c_str());
    ImGui::PopTextWrapPos();

    ImGui::Spacing();

    // 3. Нижняя часть: Либо кнопки выбора ответа, либо подсказка "Нажмите любую клавишу"
    if (!node.options.empty()) {
        ImGui::Spacing();
        for (size_t i = 0; i < node.options.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            std::string btnLabel = " > " + node.options[i].text;
            if (ImGui::Button(btnLabel.c_str(), ImVec2(windowSize.x - 40.0f, 32.0f))) {
                advanceDialogue(node.options[i].targetNodeIndex);
            }
            ImGui::PopID();
        }
    }
    else {
        // Подсказка в правом нижнем углу диалогового окна
        ImGui::SetCursorPosY(windowHeight - 35.0f);
        ImGui::SetCursorPosX(windowSize.x - 360.0f);
        ImGui::TextDisabled("[Нажмите любую клавишу для продолжения...]");
    }

    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
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

    if (isDialogueOpen) {
        ImGui::BeginDisabled(true);
    }

    switch (currentScreen) {
    case ScreenState::MainMenu: renderMainMenu(); break;
    case ScreenState::Settings: renderSettings(); break;
    case ScreenState::Saves: renderSaves(); break;
    case ScreenState::Lobby: renderLobby(); break;
    case ScreenState::Shop: renderShop(); break;
    case ScreenState::Inventory: renderInventory(); break;
    case ScreenState::Map: renderMap(); break;
    case ScreenState::Achievements: renderAchievements(); break;
    case ScreenState::SquadEditor: renderSquadEditor(); break;
    case ScreenState::CharacterEditor: renderCharacterEditor(); break;
    case ScreenState::Combat: renderCombat(); break;
    case ScreenState::EnvironmentInteraction: renderEnvironmentInteraction(); break;
    case ScreenState::Crafting: renderCrafting(); break;
    case ScreenState::Cooking: renderCooking(); break;
    }

    renderBottomNavigation();

    if (isDialogueOpen) {
        ImGui::EndDisabled();
    }

    ImGui::End();

    // Отрисовка диалогового окна поверх текущего экрана
    if (isDialogueOpen) {
        renderDialogue();
    }

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

    // Заголовок активной вкладки и постоянный баланс игрока
    ImGui::Text("=== ИНВЕНТАРЬ | Вкладка: %s ===", activeTabName.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 280.0f);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Баланс игрока: %d монет", playerInventory.getBalance());

    ImGui::Separator();
    ImGui::Spacing();

    // Кнопки переключения вкладок (картинками)
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
        ImTextureID textureId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentInventoryTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));

        if (ImGui::ImageButton(inventoryTabs[i].strId, textureId, iconSize)) {
            currentInventoryTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Отрисовка предметов выбранной категории
    ItemCategory selectedCategory = tabToCategory(currentInventoryTab);
    auto itemsList = playerInventory.getItemsByCategory(selectedCategory);
    auto& dbItems = GameDatabase::getInstance().items;

    if (itemsList.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "В категории «%s» пока нет предметов.", activeTabName.c_str());
    }
    else {
        int columns = 4;
        for (size_t i = 0; i < itemsList.size(); ++i) {
            const auto& stack = itemsList[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginGroup();

            ImGui::ImageButton("##InvItemBtn", itemData->getImGuiTextureID(), ImVec2(64, 64));
            ImGui::SameLine();
            ImGui::Text("%s\nКол-во: %d\nПродажа: %d монет",
                itemData->getName().c_str(), stack.count, itemData->getSellPrice());

            ImGui::EndGroup();

            if ((i + 1) % columns != 0 && i + 1 < itemsList.size()) {
                ImGui::SameLine(0.0f, 30.0f);
            }
            ImGui::PopID();
        }
    }
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

    ImGui::Text("=== ТОРГОВЛЯ: %s | Категория: %s ===", currentMerchantName.c_str(), activeTabName.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 220.0f);
    if (ImGui::Button("<- Вернуться на карту", ImVec2(190.0f, 30.0f))) {
        currentScreen = ScreenState::Map;
    }

    ImGui::Separator();
    ImGui::Spacing();

    // 1. ВЕРХНЯЯ ПАНЕЛЬ: Переключатели вкладок категорий (без квестовых предметов)
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
        ImTextureID textureId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentShopTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));

        if (ImGui::ImageButton(shopTabs[i].strId, textureId, iconSize)) {
            currentShopTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();

    ItemCategory activeCategory = tabToCategory(currentShopTab);
    auto& dbItems = GameDatabase::getInstance().items;

    // 2. РАЗДЕЛЕНИЕ НА 2 КОЛОНКИ: Слева — Торговец, Справа — Игрок
    ImGui::Columns(2, "ShopTradeColumns", true);

    // =========================================================================
    // ЛЕВАЯ ЧАСТЬ: ИНВЕНТАРЬ ТОРГОВЦА (Клик по предмету = ПОКУПКА)
    // =========================================================================
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Инвентарь торговца (%s)", currentMerchantName.c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "| Баланс: %d монет", activeMerchantInventory->getBalance());
    ImGui::TextDisabled("Нажмите на предмет, чтобы КУПИТЬ его");
    ImGui::Separator();
    ImGui::Spacing();

    auto merchantItems = activeMerchantInventory->getItemsByCategory(activeCategory);
    if (merchantItems.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "У торговца нет товаров в этой категории.");
    }
    else {
        for (size_t i = 0; i < merchantItems.size(); ++i) {
            const auto& stack = merchantItems[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            int buyPrice = itemData->getBuyPrice();
            bool canPlayerAfford = (playerInventory.getBalance() >= buyPrice);

            ImGui::PushID(1000 + static_cast<int>(i));

            // Если у игрока не хватает монет, блокируем кнопку покупки
            if (!canPlayerAfford) {
                ImGui::BeginDisabled();
            }

            if (ImGui::ImageButton("##BuyItemBtn", itemData->getImGuiTextureID(), ImVec2(56, 56))) {
                // Покупка: списываем у игрока, начисляем торговцу, передаем 1 шт. предмета
                if (playerInventory.spendFunds(buyPrice)) {
                    activeMerchantInventory->addFunds(buyPrice);
                    activeMerchantInventory->removeItem(stack.itemId, 1);
                    playerInventory.addItem(stack.itemId, 1);
                }
            }

            if (!canPlayerAfford) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s (x%d)", itemData->getName().c_str(), stack.count);
            if (canPlayerAfford) {
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Цена покупки: %d монет", buyPrice);
            }
            else {
                ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "Цена покупки: %d монет (Не хватает средств)", buyPrice);
            }
            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    ImGui::NextColumn();

    // =========================================================================
    // ПРАВАЯ ЧАСТЬ: ИНВЕНТАРЬ ИГРОКА (Клик по предмету = ПРОДАЖА)
    // =========================================================================
    ImGui::TextColored(ImVec4(0.3f, 0.95f, 0.5f, 1.0f), "Инвентарь игрока");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "| Баланс: %d монет", playerInventory.getBalance());
    ImGui::TextDisabled("Нажмите на предмет, чтобы ПРОДАТЬ его торговцу");
    ImGui::Separator();
    ImGui::Spacing();

    auto playerItems = playerInventory.getItemsByCategory(activeCategory);
    if (playerItems.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "В вашем инвентаре пусто в этой категории.");
    }
    else {
        for (size_t i = 0; i < playerItems.size(); ++i) {
            const auto& stack = playerItems[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            int sellPrice = itemData->getSellPrice();
            bool canMerchantAfford = (activeMerchantInventory->getBalance() >= sellPrice);

            ImGui::PushID(2000 + static_cast<int>(i));

            // Если у торговца закончились деньги, продажа не происходит
            if (!canMerchantAfford) {
                ImGui::BeginDisabled();
            }

            if (ImGui::ImageButton("##SellItemBtn", itemData->getImGuiTextureID(), ImVec2(56, 56))) {
                // Продажа: списываем у торговца, начисляем игроку, передаем предмет торговцу
                if (activeMerchantInventory->spendFunds(sellPrice)) {
                    playerInventory.addFunds(sellPrice);
                    playerInventory.removeItem(stack.itemId, 1);
                    activeMerchantInventory->addItem(stack.itemId, 1);
                }
            }

            if (!canMerchantAfford) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s (x%d)", itemData->getName().c_str(), stack.count);
            if (canMerchantAfford) {
                ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Цена продажи: %d монет", sellPrice);
            }
            else {
                ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "Цена продажи: %d монет (У торговца нет монет)", sellPrice);
            }
            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    ImGui::Columns(1);
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
    float spacing = 20.0f;
    int buttonCount = 4;

    // Если шрифт загружен, применяем его
    if (mainMenuFont) {
        ImGui::PushFont(mainMenuFont);
    }

    // Делаем фон кнопок полностью прозрачным
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    // Вычисляем высоту всех кнопок для центрирования
    float textHeight = ImGui::GetFontSize();
    float totalHeight = (textHeight * buttonCount) + (spacing * (buttonCount - 1));
    float startY = (displaySize.y - totalHeight) * 0.5f;

    ImGui::SetCursorPosY(startY);

    // Массив кнопок для удобства
    struct MenuBtn { const char* label; ScreenState target; bool isQuit; };
    MenuBtn buttons[] = {
        {"Начать игру", ScreenState::Lobby, false},
        {"Загрузить игру", ScreenState::Saves, false},
        {"Настройки", ScreenState::Settings, false},
        {"Выход", ScreenState::MainMenu, true} // Флаг выхода
    };

    for (int i = 0; i < 4; ++i) {
        float textWidth = ImGui::CalcTextSize(buttons[i].label).x;
        ImGui::SetCursorPosX((displaySize.x - textWidth) * 0.5f); // Центрируем каждую строку

        // Отрисовка прозрачной кнопки
        // Мы меняем цвет самого текста при наведении для визуального отклика
        bool isHovered = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + textWidth, ImGui::GetCursorScreenPos().y + textHeight));

        if (isHovered) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Желтоватый при наведении
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Белый по умолчанию
        }

        if (ImGui::Button(buttons[i].label)) {
            if (buttons[i].isQuit) shouldQuit = true;
            else currentScreen = buttons[i].target;
        }

        ImGui::PopStyleColor(); // Возвращаем цвет текста
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
    }

    ImGui::PopStyleColor(3); // Возвращаем стили кнопок

    if (mainMenuFont) {
        ImGui::PopFont();
    }
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

void ScreenManager::renderCooking() {
    // В зависимости от ваших настроек окна, можно использовать ImGui::SetNextWindow... 
    // или рендерить внутри текущего полноэкранного контейнера
    ImGui::Text("=== ПОЛЕВАЯ КУХНЯ ===");
    ImGui::Separator();
    ImGui::Spacing();

    // Кнопка "Назад"
    if (ImGui::Button("<- В лобби", ImVec2(150, 40))) {
        clearCookingGrid(); // Очищаем стол при уходе
        currentScreen = ScreenState::Lobby;
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Разделяем экран на 2 колонки: Инвентарь (слева) и Поле крафта (справа)
    ImGui::Columns(2, "CookingColumns", false);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.4f); // Левая колонка занимает 40%

    // --- ЛЕВАЯ КОЛОНКА: Доступные ингредиенты ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Ваши ингредиенты:");
    ImGui::Spacing();

    auto foodItems = playerInventory.getItemsByCategory(ItemCategory::Food);

    // Подсчитываем, сколько каждого предмета УЖЕ лежит на столе
    std::unordered_map<std::string, int> gridCounts;
    for (const auto& id : cookingGrid) {
        if (!id.empty()) gridCounts[id]++;
    }

    // Выводим предметы инвентаря
    for (size_t i = 0; i < foodItems.size(); ++i) {
        const auto& stack = foodItems[i];
        int availableToUse = stack.count - gridCounts[stack.itemId];

        if (availableToUse > 0) {
            const auto& itemData = GameDatabase::getInstance().items[stack.itemId];

            ImGui::PushID(static_cast<int>(i));

            // Клик по ингредиенту в инвентаре переносит его на первую свободную клетку сетки
            if (ImGui::ImageButton(itemData->getImGuiTextureID(), ImVec2(48, 48))) {
                for (int slotIndex = 0; slotIndex < 9; ++slotIndex) {
                    if (cookingGrid[slotIndex].empty()) {
                        cookingGrid[slotIndex] = stack.itemId;
                        break;
                    }
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s\nДоступно: %d", itemData->getName().c_str(), availableToUse);
            ImGui::PopID();
        }
    }

    ImGui::NextColumn();

    // --- ПРАВАЯ КОЛОНКА: Сетка 3x3 и Результат ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Поле рецепта (3x3):");
    ImGui::Spacing();

    // Отрисовка поля 3x3
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int index = row * 3 + col;

            ImGui::PushID(100 + index); // Сдвиг ID, чтобы не конфликтовать с левой панелью

            if (cookingGrid[index].empty()) {
                // Пустой слот
                ImGui::Button("##EmptySlot", ImVec2(64, 64));
            }
            else {
                // Занятый слот (нажатие убирает предмет обратно в инвентарь)
                std::string itemId = cookingGrid[index];
                const auto& itemData = GameDatabase::getInstance().items[itemId];

                if (ImGui::ImageButton(itemData->getImGuiTextureID(), ImVec2(64, 64))) {
                    cookingGrid[index] = ""; // Убираем со стола
                }
            }

            if (col < 2) ImGui::SameLine(); // Выстраиваем в ряд

            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Результат:");
    ImGui::Spacing();

    // --- ЛОГИКА ПРОВЕРКИ РЕЦЕПТОВ ---
    // 1. Собираем всё, что лежит на столе (без пустых клеток)
    std::vector<std::string> currentIngredients;
    for (const auto& id : cookingGrid) {
        if (!id.empty()) currentIngredients.push_back(id);
    }

    // Сортируем текущие ингредиенты по алфавиту (чтобы порядок расстановки не имел значения)
    std::sort(currentIngredients.begin(), currentIngredients.end());

    // 2. База рецептов (В будущем можно вынести в GameDatabase)
    struct SimpleRecipe {
        std::string resultId;
        std::vector<std::string> requiredIngredients;
    };

    static std::vector<SimpleRecipe> recipes = {
        // Пример 1: Печёное яблоко (Яблоко + Ячмень)
        {"baked_apple", {"apple", "barley"}},

        // Пример 2: Сытный суп (Гриб + Вода + Соль)
        {"hearty_mushroom_soup", {"mushroom", "ocean_salt", "dirty_water"}},

        // Пример 3: Мясное рагу (Мясо + Гриб + Соль)
        {"meat_stew", {"rat_meat", "rusty_mushroom", "ocean_salt"}}
    };

    // 3. Ищем совпадение
    std::string matchedResult = "";
    for (auto& recipe : recipes) {
        std::vector<std::string> req = recipe.requiredIngredients;
        std::sort(req.begin(), req.end()); // Сортируем требования рецепта

        // Если отсортированные массивы идентичны — рецепт верный!
        if (req == currentIngredients) {
            matchedResult = recipe.resultId;
            break;
        }
    }

    // 4. Отрисовка результата
    if (!matchedResult.empty()) {
        const auto& resultData = GameDatabase::getInstance().items[matchedResult];

        if (ImGui::ImageButton(resultData->getImGuiTextureID(), ImVec2(80, 80))) {
            // ИГРОК ЗАБИРАЕТ БЛЮДО
            // 1. Добавляем готовое блюдо
            // Замените addItem на ваш актуальный метод, если он принимает другие параметры
            playerInventory.addItem(matchedResult, 1);

            // 2. Списываем задействованные ингредиенты из инвентаря
            for (const auto& id : currentIngredients) {
                playerInventory.removeItem(id, 1);
            }

            // 3. Очищаем стол
            clearCookingGrid();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Готово: %s!\nКликните по иконке, чтобы забрать.", resultData->getName().c_str());
    }
    else {
        // Ничего не совпало или пусто
        ImGui::BeginDisabled();
        ImGui::Button("?", ImVec2(80, 80));
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Скомбинируйте ингредиенты,\nчтобы получить блюдо.");
    }

    ImGui::Columns(1);
}

void ScreenManager::renderSettings() { 
    ImGui::Text("=== НАСТРОЙКИ ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderSaves() { 
    ImGui::Text("=== ЭКРАН СОХРАНЕНИЙ ==="); 
    ImGui::Separator(); 
}

// КАРТА
void ScreenManager::renderMap() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    bool isPopupOpen = (selectedMapPointIndex != -1);

    if (isPopupOpen) {
        ImGui::BeginDisabled(true);
    }

    // --- 1. Основное содержимое карты ---
    ImGui::Text("=== КАРТА БАШНИ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Отображается этаж: %d", currentFloor);
    ImGui::Text("Здесь будет рендер текстуры карты для этажа №%d...", currentFloor);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < mapPoints.size(); ++i) {
        auto& point = mapPoints[i];

        // Показываем только точки выбранного этажа, которые еще не скрыты
        if (point.floor != currentFloor || point.isCompleted) {
            continue;
        }

        ImGui::PushID(static_cast<int>(i));

        float r = point.radius;
        ImVec2 center = point.position;
        ImVec2 topLeft = ImVec2(center.x - r, center.y - r);

        // Размещаем невидимую кнопку точно по размеру круга для обработки клика и наведения
        ImGui::SetCursorPos(topLeft);
        if (ImGui::InvisibleButton("##MapPointBtn", ImVec2(r * 2.0f, r * 2.0f))) {
            selectedMapPointIndex = static_cast<int>(i);
        }

        bool isHovered = ImGui::IsItemHovered();

        // Подбираем цвет заливки круга в зависимости от типа точки
        ImU32 fillColor = IM_COL32(55, 65, 85, 235);
        ImU32 borderColor = isHovered ? IM_COL32(255, 225, 100, 255) : IM_COL32(190, 195, 210, 255);

        switch (point.type) {
        case MapNodeType::Shop:        fillColor = IM_COL32(45, 110, 75, 235); break;  // Изумрудный (Торговец)
        case MapNodeType::Combat:      fillColor = IM_COL32(140, 45, 45, 235); break;  // Багровый (Бой)
        case MapNodeType::Environment: fillColor = IM_COL32(50, 95, 145, 235); break;  // Синий (Ресурсы/Окружение)
        case MapNodeType::Cooking:     fillColor = IM_COL32(150, 95, 35, 235); break;  // Оранжевый (Костер)
        case MapNodeType::Crafting:    fillColor = IM_COL32(100, 70, 140, 235); break; // Фиолетовый (Мастерская)
        }

        // Рисуем сам круг и его обводку
        drawList->AddCircleFilled(center, r, fillColor, 32);
        drawList->AddCircle(center, r, borderColor, 32, isHovered ? 3.5f : 2.0f);

        // Рисуем иконку внутри круга (если текстура загружена)
        auto itTex = nodeIconTextures.find(point.iconKey);
        if (itTex != nodeIconTextures.end() && itTex->second.getNativeHandle() != 0) {
            ImTextureID texId = (ImTextureID)(uintptr_t)itTex->second.getNativeHandle();
            float iconHalf = r * 0.65f;
            drawList->AddImage(texId,
                ImVec2(center.x - iconHalf, center.y - iconHalf),
                ImVec2(center.x + iconHalf, center.y + iconHalf));
        }
        else {
            // Запасной символ в центре круга, если текстура не найдена
            const char* symbol = "?";
            if (point.type == MapNodeType::Shop) symbol = "$";
            else if (point.type == MapNodeType::Combat) symbol = "!";
            else if (point.type == MapNodeType::Environment) symbol = "*";

            ImVec2 symSize = ImGui::CalcTextSize(symbol);
            drawList->AddText(ImVec2(center.x - symSize.x * 0.5f, center.y - symSize.y * 0.5f),
                IM_COL32(255, 255, 255, 255), symbol);
        }

        // Короткий текст с названием точки строго под кругом
        ImVec2 textSize = ImGui::CalcTextSize(point.title.c_str());
        ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, center.y + r + 6.0f);

        // Полупрозрачная подложка под текст для отличной читаемости на любом фоне карты
        drawList->AddRectFilled(ImVec2(textPos.x - 5.0f, textPos.y - 2.0f),
            ImVec2(textPos.x + textSize.x + 5.0f, textPos.y + textSize.y + 2.0f),
            IM_COL32(15, 15, 20, 200), 4.0f);
        drawList->AddText(textPos, IM_COL32(240, 240, 240, 255), point.title.c_str());

        ImGui::PopID();
    }


    // --- 2. Столбец переключателя этажей у правого края ---
    float panelWidth = 100.0f;
    float buttonHeight = 40.0f;
    float spacing = 8.0f;
    int totalFloors = 10;

    // Вычисляем высоту панели и центрируем её по вертикали
    float panelHeight = (buttonHeight * totalFloors) + (spacing * (totalFloors - 1));
    float posX = displaySize.x - panelWidth - 20.0f; // 20px от правого края
    float posY = (displaySize.y - panelHeight) * 0.5f; // По центру высоты экрана

    // Устанавливаем позицию для столбца
    ImGui::SetCursorPos(ImVec2(posX, posY));
    ImGui::Spacing();
    ImGui::BeginGroup();

    // Заголовок над столбцом
    ImGui::Text("  Уровни");
    ImGui::Separator();
    ImGui::Spacing();

    // Отрисовка кнопок от 10 (вверху) до 1 (внизу)
    for (int floor = 10; floor >= 1; --floor) {
        bool isUnlocked = (floor <= maxUnlockedFloor);
        bool isSelected = (currentFloor == floor);

        // Блокируем кнопку, если этаж еще не открыт
        if (!isUnlocked) {
            ImGui::BeginDisabled();
        }

        // Подсветка активно выбранного этажа
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.25f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
        }

        std::string btnLabel = isUnlocked ? ("Этаж " + std::to_string(floor)) : ("Блок " + std::to_string(floor));
        btnLabel += "##FloorBtn"; // Уникальный ID

        if (ImGui::Button(btnLabel.c_str(), ImVec2(panelWidth, buttonHeight))) {
            currentFloor = floor;
        }

        ImGui::PopStyleColor(2);

        if (!isUnlocked) {
            ImGui::EndDisabled();
        }

        ImGui::Spacing();
    }

    ImGui::EndGroup();
    
    if (isPopupOpen) {
        ImGui::EndDisabled();
    }

    // --- 3. ОТРИСОВКА ВСПЛЫВАЮЩЕГО ОКНА ПОВЕРХ КАРТЫ ---
    if (isPopupOpen) {
        renderMapPopup();
    }
}
void ScreenManager::renderMapPopup() {
    if (selectedMapPointIndex < 0 || selectedMapPointIndex >= static_cast<int>(mapPoints.size())) {
        selectedMapPointIndex = -1;
        return;
    }

    auto& point = mapPoints[selectedMapPointIndex];
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 popupSize = ImVec2(440.0f, 270.0f);
    ImVec2 popupPos = ImVec2((displaySize.x - popupSize.x) * 0.5f, (displaySize.y - popupSize.y) * 0.5f);

    ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.12f, 0.16f, 0.98f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.75f, 0.65f, 0.35f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

    std::string windowTitle = point.title + "##MapPointPopup";
    ImGui::Begin(windowTitle.c_str(), nullptr, popupFlags);

    // Пометка о временности точки
    if (point.isTemporary) {
        ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.25f, 1.0f), "[Временная точка — исчезнет после посещения]");
    }
    else {
        ImGui::TextColored(ImVec4(0.5f, 0.85f, 0.5f, 1.0f), "[Постоянная локация]");
    }

    ImGui::Spacing();
    ImGui::TextWrapped("%s", point.description.c_str());
    ImGui::Spacing();
    ImGui::Separator();

    // Дополнительная сводка в зависимости от типа точки
    if (point.type == MapNodeType::Shop) {
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.2f, 1.0f), "Торговец: %s (Баланс: %d монет)",
            point.merchantName.c_str(), point.merchantInventory.getBalance());
    }
    else if (point.type == MapNodeType::Combat) {
        ImGui::TextColored(ImVec4(0.95f, 0.4f, 0.4f, 1.0f), "Противники на уровне:");
        for (const auto& enemy : point.enemies) {
            ImGui::BulletText("%s (Ур. %d) x%d", enemy.enemyName.c_str(), enemy.level, enemy.count);
        }
    }

    // Кнопки "Перейти" и "Отмена" в нижней части всплывающего окна
    float btnWidth = 180.0f;
    float btnHeight = 38.0f;
    ImGui::SetCursorPos(ImVec2(25.0f, popupSize.y - btnHeight - 18.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.80f, 0.40f, 1.0f));
    if (ImGui::Button("Перейти", ImVec2(btnWidth, btnHeight))) {
        // 1. Если точка временная — скрываем её после взаимодействия
        if (point.isTemporary) {
            point.isCompleted = true;
        }

        // 2. Передаем данные и переключаем экран
        switch (point.type) {
        case MapNodeType::Shop:
            openShop(point.merchantName, &point.merchantInventory);
            break;

        case MapNodeType::Combat: {
            // Получаем текущий отряд игрока (если в профиле есть getActiveSquad(), берем его, иначе стартовую тройку)
            std::vector<std::string> currentSquad = { "adeir", "renwick", "garosh" };
            startCombat(currentSquad, point.enemies);
            break;
        }

        case MapNodeType::Environment:
            currentScreen = ScreenState::EnvironmentInteraction;
            break;

        case MapNodeType::Cooking:
            currentScreen = ScreenState::Cooking;
            break;

        case MapNodeType::Crafting:
            currentScreen = ScreenState::Crafting;
            break;
        }

        // Закрываем всплывающее окно
        selectedMapPointIndex = -1;
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0.0f, 25.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.30f, 0.30f, 1.0f));
    if (ImGui::Button("Отмена", ImVec2(btnWidth, btnHeight))) {
        // Просто сворачиваем всплывающее окно
        selectedMapPointIndex = -1;
    }
    ImGui::PopStyleColor(2);

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void ScreenManager::renderAchievements() {
    ImGui::Text("=== ДОСТИЖЕНИЯ ==="); 
    ImGui::Separator(); 
}
void ScreenManager::renderSquadEditor() {
    ImGui::Text("=== РЕДАКТОР ОТРЯДА ===");
    ImGui::Separator();
    ImGui::Spacing();

    // 1. Инициализация временного отряда при входе на экран
    if (!isSquadEditorInitialized) {
        // Копируем текущий активный отряд во временный массив
        tempSquad = playerProfile.getActiveSquad();
        isSquadEditorInitialized = true;
    }

    // 2. Отрисовка трех слотов отряда
    ImGui::Columns(3, "SquadSlots", false);
    for (int i = 0; i < 3; ++i) {
        ImGui::PushID(i);

        // Оформление слота
        ImGui::BeginChild("Slot", ImVec2(0, 200), true);
        ImGui::Text("Слот %d", i + 1);
        ImGui::Separator();

        if (tempSquad[i] != nullptr) {
            // Если персонаж есть, выводим его имя/иконку
            ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "%s", tempSquad[i]->getName().c_str());
            ImGui::Text("Уровень: %d", tempSquad[i]->getLevel());
        }
        else {
            // Если пусто
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "[ ПУСТО ]");
        }

        // Кнопка взаимодействия со слотом
        ImGui::SetCursorPosY(160.0f); // Прижимаем кнопку к низу слота
        if (ImGui::Button(tempSquad[i] ? "Изменить" : "Добавить", ImVec2(-1, 30))) {
            currentEditingSlot = i;
            showCharacterSelector = true;
        }

        ImGui::EndChild();
        ImGui::PopID();
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Separator();

    // 3. Кнопки управления (Сохранить / В лобби)
    if (ImGui::Button("Сохранить", ImVec2(150, 40))) {
        // Применяем изменения в основной профиль
        for (int i = 0; i < 3; ++i) {
            playerProfile.setSquadSlotDirectly(i, tempSquad[i]); // Предполагаемый метод для прямой установки
        }
        isSquadEditorInitialized = false; // Сбрасываем флаг для следующего захода
        currentScreen = ScreenState::Lobby;
    }

    ImGui::SameLine();

    if (ImGui::Button("В лобби (Без сохранения)", ImVec2(200, 40))) {
        isSquadEditorInitialized = false; // Откатываем изменения
        currentScreen = ScreenState::Lobby;
    }

    // 4. Модальное окно выбора персонажа (открывается поверх)
    if (showCharacterSelector) {
        ImGui::OpenPopup("Выбор персонажа");
    }

    if (ImGui::BeginPopupModal("Выбор персонажа", &showCharacterSelector, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Выберите персонажа для слота %d", currentEditingSlot + 1);
        ImGui::Separator();

        const auto& reserve = playerProfile.getReserve();

        // Отрисовка списка персонажей
        for (size_t i = 0; i < reserve.size(); ++i) {
            auto character = reserve[i];

            // Проверяем, выбран ли уже этот персонаж в другой слот
            bool isAlreadySelected = false;
            for (int j = 0; j < 3; ++j) {
                if (tempSquad[j] == character && j != currentEditingSlot) {
                    isAlreadySelected = true;
                    break;
                }
            }

            if (isAlreadySelected) {
                // Если персонаж уже в отряде, делаем кнопку неактивной
                ImGui::BeginDisabled();
                ImGui::Button((character->getName() + " (Уже в отряде)").c_str(), ImVec2(250, 30));
                ImGui::EndDisabled();
            }
            else {
                // Доступный персонаж
                if (ImGui::Button(character->getName().c_str(), ImVec2(250, 30))) {
                    tempSquad[currentEditingSlot] = character;
                    showCharacterSelector = false;
                }
            }
        }

        ImGui::Separator();

        // Кнопка "Убрать" (освобождает слот)
        if (ImGui::Button("Убрать персонажа", ImVec2(120, 30))) {
            tempSquad[currentEditingSlot] = nullptr;
            showCharacterSelector = false;
        }

        ImGui::SameLine();

        // Кнопка "Закрыть" (без изменений)
        if (ImGui::Button("Закрыть", ImVec2(120, 30))) {
            showCharacterSelector = false;
        }

        ImGui::EndPopup();
    }
}

void ScreenManager::renderCharacterEditor() {
    ImGui::Text("=== РЕДАКТОР ПЕРСОНАЖА ==="); 
    ImGui::Separator(); 
}

void ScreenManager::renderCombat() {
    ImGui::Text("=== ЭКРАН БОЯ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Columns(2, "CombatSetupColumns", true);

    // Левая колонка: переданный отряд игрока
    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "Текущий отряд игрока:");
    ImGui::Spacing();
    auto& dbChars = GameDatabase::getInstance().characters;
    for (const auto& charId : activeCombatPlayerSquad) {
        if (dbChars.find(charId) != dbChars.end()) {
            ImGui::BulletText("%s (ID: %s)", dbChars[charId]->getName().c_str(), charId.c_str());
        }
        else {
            ImGui::BulletText("Боец ID: %s", charId.c_str());
        }
    }

    ImGui::NextColumn();

    // Правая колонка: переданный состав противников на уровне
    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "Состав противников на уровне:");
    ImGui::Spacing();
    for (const auto& enemy : activeCombatEnemies) {
        ImGui::BulletText("%s | Уровень: %d | Кол-во: %d",
            enemy.enemyName.c_str(), enemy.level, enemy.count);
    }

    ImGui::Columns(1);

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("<- Отступить на карту", ImVec2(200, 40))) {
        currentScreen = ScreenState::Map;
    }
}

void ScreenManager::renderCrafting() {
    std::string activeTabName = (currentCraftingTab == ItemTab::Materials) ? "Материалы" : "Оружие";

    ImGui::Text("=== МАСТЕРСКАЯ (СОЗДАНИЕ) | Вкладка: %s ===", activeTabName.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // Кнопка возврата в Лобби с очисткой сетки
    if (ImGui::Button("<- В лобби", ImVec2(150, 40))) {
        clearCraftingGrid();
        currentScreen = ScreenState::Lobby;
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Разделяем экран на 2 колонки: Инвентарь с вкладками (слева) и Сетка 3x3 (справа)
    ImGui::Columns(2, "CraftingColumns", false);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.4f);

    // --- ЛЕВАЯ КОЛОНКА: Переключатель вкладок и список предметов ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Категория компонентов:");
    ImGui::Spacing();

    ImVec2 tabIconSize = ImVec2(48.0f, 48.0f);
    struct CraftTabButton { ItemTab tab; const char* strId; const char* fallbackText; };
    CraftTabButton craftTabs[] = {
        {ItemTab::Materials, "##CraftTabMaterials", "Материалы"},
        {ItemTab::Weapons,   "##CraftTabWeapons",   "Оружие"}
    };

    // Отрисовка 2 кнопок переключения вкладок (как в инвентаре)
    for (int i = 0; i < 2; ++i) {
        if (i > 0) ImGui::SameLine();

        ItemTab tab = craftTabs[i].tab;
        bool isActive = (currentCraftingTab == tab);

        if (isActive) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
        }

        // Если текстура вкладки загружена — рисуем ImageButton, иначе обычную кнопку
        if (tabTextures.count(tab) && tabTextures[tab].getNativeHandle() != 0) {
            ImTextureID texId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();
            if (ImGui::ImageButton(craftTabs[i].strId, texId, tabIconSize)) {
                currentCraftingTab = tab;
            }
        }
        else {
            if (ImGui::Button(craftTabs[i].fallbackText, ImVec2(120.0f, 40.0f))) {
                currentCraftingTab = tab;
            }
        }

        if (isActive) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Доступно в инвентаре (%s):", activeTabName.c_str());
    ImGui::Spacing();

    // Определяем нужную категорию предметов по выбранной вкладке
    ItemCategory selectedCategory = (currentCraftingTab == ItemTab::Materials)
        ? ItemCategory::Material
        : ItemCategory::Weapon;

    auto availableItems = playerInventory.getItemsByCategory(selectedCategory);

    // Подсчитываем, сколько каждого предмета УЖЕ выложено на сетку 3x3
    std::unordered_map<std::string, int> gridCounts;
    for (const auto& id : craftingGrid) {
        if (!id.empty()) gridCounts[id]++;
    }

    // Выводим карточки предметов выбранной вкладки
    for (size_t i = 0; i < availableItems.size(); ++i) {
        const auto& stack = availableItems[i];
        int availableToUse = stack.count - gridCounts[stack.itemId];

        if (availableToUse > 0) {
            auto& dbItems = GameDatabase::getInstance().items;
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;

            const auto& itemData = dbItems[stack.itemId]; // shared_ptr<ItemTemplate>

            ImGui::PushID(static_cast<int>(i));

            // Клик переносит 1 предмет на первую свободную ячейку сетки 3x3
            if (ImGui::ImageButton("##CraftItemBtn", itemData->getImGuiTextureID(), ImVec2(48, 48))) {
                for (int slotIndex = 0; slotIndex < 9; ++slotIndex) {
                    if (craftingGrid[slotIndex].empty()) {
                        craftingGrid[slotIndex] = stack.itemId;
                        break;
                    }
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s\nДоступно: %d", itemData->getName().c_str(), availableToUse);

            ImGui::PopID();
        }
    }

    ImGui::NextColumn();

    // --- ПРАВАЯ КОЛОНКА: Сетка 3x3 и Результат ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Сетка создания (3x3):");
    ImGui::Spacing();

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int index = row * 3 + col;

            ImGui::PushID(200 + index);

            if (craftingGrid[index].empty()) {
                ImGui::Button("##EmptyCraftSlot", ImVec2(64, 64));
            }
            else {
                std::string itemId = craftingGrid[index];
                const auto& itemData = GameDatabase::getInstance().items[itemId];

                // Клик по занятой ячейке возвращает предмет обратно в список
                if (ImGui::ImageButton("##FilledCraftSlot", itemData->getImGuiTextureID(), ImVec2(64, 64))) {
                    craftingGrid[index] = "";
                }
            }

            if (col < 2) ImGui::SameLine();

            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Результат создания:");
    ImGui::Spacing();

    // --- ЛОГИКА ПРОВЕРКИ РЕЦЕПТОВ СОЗДАНИЯ ---
    std::vector<std::string> currentComponents;
    for (const auto& id : craftingGrid) {
        if (!id.empty()) currentComponents.push_back(id);
    }

    // Сортируем компоненты, чтобы порядок выкладки в сетке 3x3 не имел значения
    std::sort(currentComponents.begin(), currentComponents.end());

    struct CraftRecipe {
        std::string resultId;
        std::vector<std::string> requiredComponents;
    };

    // Список рецептов крафта (комбинации материалов и/или базового оружия)
    static std::vector<CraftRecipe> craftRecipes = {
        // Пример 1: Старый тесак из Ржавого клинка и 2 кусков Железной руды
        {"old_cleaver", {"rusty_blade", "iron_ore", "iron_ore"}},

        // Пример 2: Топор Разъяренного из Старого тесака и 2 кусков Железной руды
        {"berserker_axe", {"old_cleaver", "iron_ore", "iron_ore"}}
    };

    std::string matchedResult = "";
    for (const auto& recipe : craftRecipes) {
        std::vector<std::string> req = recipe.requiredComponents;
        std::sort(req.begin(), req.end());

        if (req == currentComponents) {
            matchedResult = recipe.resultId;
            break;
        }
    }

    // --- ОТРИСОВКА ЯЧЕЙКИ РЕЗУЛЬТАТА ---
    auto& dbItems = GameDatabase::getInstance().items;
    if (!matchedResult.empty() && dbItems.find(matchedResult) != dbItems.end()) {
        const auto& resultData = dbItems[matchedResult];

        if (ImGui::ImageButton("##CraftResultBtn", resultData->getImGuiTextureID(), ImVec2(80, 80))) {
            // 1. Выдаем созданный предмет игроку в инвентарь
            playerInventory.addItem(matchedResult, 1);

            // 2. Списываем задействованные материалы и оружие
            for (const auto& id : currentComponents) {
                playerInventory.removeItem(id, 1);
            }

            // 3. Очищаем сетку 3x3
            clearCraftingGrid();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f),
            "Готово: %s!\nНажмите на иконку, чтобы создать.",
            resultData->getName().c_str());
    }
    else {
        ImGui::BeginDisabled();
        ImGui::Button("?", ImVec2(80, 80));
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f),
            "Разместите материалы и оружие,\nчтобы создать новый предмет.");
    }

    ImGui::Columns(1);
}

void ScreenManager::renderEnvironmentInteraction() {
    ImGui::Text("=== ВЗАИМОДЕЙСТВИЕ С ОКРУЖЕНИЕМ ===");
    ImGui::Separator(); 
}