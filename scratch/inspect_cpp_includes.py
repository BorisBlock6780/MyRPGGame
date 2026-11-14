with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

for l in text.splitlines()[:25]:
    print(l)
