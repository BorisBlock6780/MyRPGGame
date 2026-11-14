with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

start_ach = text.find("void ScreenManager::renderAchievements()")
end_ach = text.find("void ScreenManager::renderSquadEditor()", start_ach)
print('start_ach:', start_ach, 'end_ach:', end_ach)
print(repr(text[start_ach:end_ach]))
