with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for j in range(1860, 1900):
    print(lines[j].encode('ascii', 'replace').decode('ascii').rstrip())
