import re

with open(r'include/PlayerData.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

print('PlayerData.hpp achievements:')
for line in text.splitlines():
    if 'achievement' in line.lower():
        print(line)

with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text2 = f.read()

print('Database.hpp achievements:')
for line in text2.splitlines():
    if 'achievement' in line.lower():
        print(line)
