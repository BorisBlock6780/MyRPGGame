with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, l in enumerate(lines):
    if 'void ScreenManager::renderLobby()' in l:
        for j in range(i, min(i+75, len(lines))):
            print(lines[j].rstrip())
        break
