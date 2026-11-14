with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
foods = re.findall(r'registerItem\(\s*"([^"]+)",\s*"([^"]+)",\s*ItemCategory::Food', text)
print(f'Total Food items: {len(foods)}')
for f_id, f_name in foods:
    print(f'{f_id}: {f_name}')
