with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

in_model = False
accessories = []
for i, line in enumerate(lines):
    if 'void ScreenManager::renderAnimatedHeroModel' in line:
        in_model = True
    elif in_model and 'void ScreenManager::' in line:
        break
    if in_model and '// 4.' in line:
        for j in range(i, min(i+140, len(lines))):
            accessories.append(lines[j])
        break

for l in accessories[:60]:
    print(l.rstrip())
