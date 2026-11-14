with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
matches = re.findall(r'registerItem\(\s*"([^"]+)",\s*"([^"]+)",\s*ItemCategory::(\w+),\s*"([^"]+)"', text)
print(f'Total registerItem calls: {len(matches)}')
for m in matches[:10]:
    print(m)

w_matches = re.findall(r'registerWeapon\(\s*"([^"]+)",\s*"([^"]+)"[^)]+"(assets/[^"]+)"', text)
print(f'Total registerWeapon with icons: {len(w_matches)}')
for m in w_matches:
    print(m)

a_matches = re.findall(r'registerArtifact\(\s*"([^"]+)",\s*"([^"]+)"[^)]+"(assets/[^"]+)"', text)
print(f'Total registerArtifact with icons: {len(a_matches)}')
for m in a_matches:
    print(m)
