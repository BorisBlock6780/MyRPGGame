with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    for f_idx in range(1, 11):
        if f'f{f_idx}_boss_camp' in line:
            print(f'Floor {f_idx}: line {i+1} and {i+2}:')
            print(repr(lines[i]))
            print(repr(lines[i+1]))
