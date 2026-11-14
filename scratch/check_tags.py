import re

with open("src/ScreenManager.cpp", "r", encoding="utf-8") as f:
    text = f.read()

literals = re.findall(r'\"([^\"]*?\[[^\"]*?\][^\"]*?)\"', text)
unique_lit = sorted(list(set(literals)))
for l in unique_lit:
    if any(k in l for k in ['Карта', 'Отряд', 'Герои', 'Сумка', 'Награды', 'Задания', 'Атака', 'Защита', 'Крит', 'Вампиризм', 'Бонус', 'Шлем', 'Доспех', 'Амулет', 'Кольцо', 'Оружие', 'Базовая', 'Навык', 'Ульта', 'СЮЖЕТНОЕ', 'ПОБОЧНОЕ', 'Пустой', '*', '+', 'x']):
        print(l)
