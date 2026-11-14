with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

import re
m_ctor = re.search(r'ScreenManager::ScreenManager\(\)\s*\{[\s\S]*?logConsole\([^;]+;\s*\}', text)
if m_ctor:
    print('Found ctor:')
    print(repr(m_ctor.group(0)[:150]))

m_lobby = re.search(r'NavTarget targets\[\]\s*=\s*\{[\s\S]*?int count = \d;', text)
if m_lobby:
    print('Found lobby:')
    print(repr(m_lobby.group(0)[:150]))
