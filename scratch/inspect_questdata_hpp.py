with open(r'include/ScreenManager.hpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, l in enumerate(lines):
    if 'struct QuestData' in l:
        for j in range(max(0, i-2), min(i+25, len(lines))):
            print(lines[j].rstrip())
        break
