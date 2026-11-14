with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

pos_cook = text.find('void ScreenManager::renderCooking()')
pos_craft = text.find('void ScreenManager::renderCrafting()', pos_cook)

chunk = text[pos_cook+2500:pos_craft]
for line in chunk.splitlines()[:60]:
    print(line.encode('ascii', 'replace').decode('ascii'))
