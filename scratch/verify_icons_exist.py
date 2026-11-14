import os
import re

with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

icon_paths = re.findall(r'"assets/icons/[^"]+"', text)
print(f'Total icon path mentions in Database.hpp: {len(icon_paths)}')
missing = []
for p in set(icon_paths):
    clean_p = p.strip('"')
    if not os.path.exists(clean_p):
        missing.append(clean_p)

print('Missing files:', missing)
