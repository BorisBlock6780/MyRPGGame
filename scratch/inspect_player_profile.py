with open(r'include/PlayerData.hpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

in_prof = False
for i, l in enumerate(lines):
    if 'class PlayerProfile' in l:
        in_prof = True
    if in_prof:
        print(l.rstrip())
