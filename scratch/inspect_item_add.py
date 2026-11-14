with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'items[' in line:
        for j in range(i, min(i+15, len(lines))):
            print(lines[j].rstrip())
        break
