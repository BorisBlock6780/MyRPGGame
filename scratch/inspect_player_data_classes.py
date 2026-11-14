with open(r'include/PlayerData.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
print('Classes and structs in PlayerData.hpp:')
for m in re.finditer(r'(class|struct)\s+(\w+)', text):
    print(m.group(1), m.group(2))
