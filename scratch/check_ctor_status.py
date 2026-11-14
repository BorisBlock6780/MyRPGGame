with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

print('initAchievements in text:', 'initAchievements' in text)
print('ScreenManager::ScreenManager in text:', 'ScreenManager::ScreenManager()' in text)
