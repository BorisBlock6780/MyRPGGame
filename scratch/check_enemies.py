import re

with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

enemies = re.findall(r'enemies\["([^"]+)"\]', text)
print(f'Total enemies in Database.hpp: {len(enemies)}')
print(enemies)
