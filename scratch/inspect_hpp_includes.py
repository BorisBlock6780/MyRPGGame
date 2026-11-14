with open(r'include/ScreenManager.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

for l in text.splitlines()[:30]:
    print(l)
