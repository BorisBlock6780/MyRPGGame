with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
items = re.findall(r'items\["([^"]+)"\]\s*=\s*std::make_shared<Item>\([^,]+,\s*"([^"]+)",\s*ItemCategory::(\w+),\s*([0-9]+),\s*([0-9]+),\s*"([^"]+)"', text)
print(f'Total items in Database: {len(items)}')
icons = [it[5] for it in items]
print(f'Unique icon paths: {len(set(icons))}')
for it in items[:10]:
    print(it[0], '->', it[5])
