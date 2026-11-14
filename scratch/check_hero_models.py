with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
chars = re.findall(r'else if \(charId == "([^"]+)"\)', text)
first = re.findall(r'if \(charId == "([^"]+)"\)', text)
all_chars = first + chars
print(f'Total characters customized in renderAnimatedHeroModel: {len(all_chars)}')
print(all_chars)
