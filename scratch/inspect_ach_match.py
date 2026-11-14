with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
m_ach = re.search(r'void ScreenManager::renderAchievements\(\)\s*\{[\s\S]*?ImGui::Separator\(\);\s*\}', text)
if m_ach:
    print('Found renderAchievements:', m_ach.span())
