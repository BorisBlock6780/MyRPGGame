with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
pos = text.find('NavTarget targets[]')
print('pos of NavTarget:', pos)
if pos != -1:
    print(repr(text[pos:pos+300]))
