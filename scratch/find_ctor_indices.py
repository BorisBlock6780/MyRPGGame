with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

start_ctor = text.find("ScreenManager::ScreenManager()")
end_ctor = text.find("void ScreenManager::processEvent(const sf::Event& event)", start_ctor)
print('start_ctor:', start_ctor, 'end_ctor:', end_ctor)
print(repr(text[start_ctor:end_ctor]))
