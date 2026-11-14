import re

with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

print('enemies in text:', 'enemies' in text)
matches = re.findall(r'(\w+enemy\w*|\w*Enemy\w*)', text)
print('matches in Database.hpp:', set(matches[:20]))

with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text2 = f.read()

m2 = re.findall(r'(\bf\d+_\w+)', text2)
print('floor ids:', set(m2))
