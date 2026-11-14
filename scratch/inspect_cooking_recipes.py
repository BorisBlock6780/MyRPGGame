with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

pos_cook = text.find('void ScreenManager::renderCooking()')
pos_end = text.find('void ScreenManager::renderCrafting()', pos_cook)

chunk = text[pos_cook+1200:pos_end]
for line in chunk.splitlines()[:50]:
    print(line.encode('ascii', 'replace').decode('ascii'))
