with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

in_char = False
char_lines = []
for i, l in enumerate(lines):
    if 'void ScreenManager::renderCharacterEditor()' in l:
        in_char = True
    elif in_char and l.startswith('void ScreenManager::'):
        break
    if in_char:
        char_lines.append((i+1, l))

for idx, l in char_lines[260:]:
    print(f'{idx}: {l.encode("ascii", "replace").decode("ascii").rstrip()[:100]}')
