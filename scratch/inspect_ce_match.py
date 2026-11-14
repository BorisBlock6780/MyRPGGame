with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
m_ce = re.search(r'void ScreenManager::renderCharacterEditor\(\)\s*\{[\s\S]*?renderSlotPickerModal\(currentChar\);\s*\}', text)
if m_ce:
    print('Found renderCharacterEditor:', m_ce.span())
