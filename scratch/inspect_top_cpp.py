with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for j in range(0, 20):
    print(lines[j].rstrip())
