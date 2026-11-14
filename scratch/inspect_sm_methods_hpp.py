with open(r'include/ScreenManager.hpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for j in range(315, 345):
    print(lines[j].rstrip())
