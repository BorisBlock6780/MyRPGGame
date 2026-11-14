with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

in_fx = False
fx_lines = []
for i, line in enumerate(lines):
    if 'void ScreenManager::renderCombatVisualEffects' in line:
        in_fx = True
    elif in_fx and 'void ScreenManager::' in line:
        break
    if in_fx:
        fx_lines.append(lines[i])

print(f'Total lines in renderCombatVisualEffects: {len(fx_lines)}')
for l in fx_lines[:50]:
    print(l.rstrip())
