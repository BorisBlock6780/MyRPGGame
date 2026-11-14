with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, l in enumerate(lines):
    if 'ScreenManager::ScreenManager()' in l:
        print(f'{i+1}: {l.strip()}')
        for j in range(i, min(i+40, len(lines))):
            print(lines[j].encode('ascii', 'replace').decode('ascii').rstrip())
        break
