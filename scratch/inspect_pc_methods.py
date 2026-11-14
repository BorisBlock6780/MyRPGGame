with open(r'include/PlayerData.hpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

in_pc = False
for i, l in enumerate(lines):
    if 'class PlayerCharacter' in l:
        in_pc = True
    elif in_pc and 'class PlayerProfile' in l:
        break
    if in_pc and ('Hp' in l or 'Mana' in l or 'Stats' in l or 'get' in l):
        print(l.rstrip())
