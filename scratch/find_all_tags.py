import re

with open("src/ScreenManager.cpp", "r", encoding="utf-8") as f:
    lines = f.readlines()

tags_to_check = [
    "[Карта]", "[Отряд]", "[Герои]", "[Сумка]", "[Награды]", "[Задания]",
    "[Атака]", "[Защита]", "[Крит]", "[Крит.урон]", "[Вампиризм]", "[Бонус]",
    "[Золото]", "[Шлем]", "[Доспех]", "[Амулет]", "[Кольцо]", "[Оружие]",
    "[Базовая]", "[Навык]", "[Ульта]", "[ СЮЖЕТНОЕ ]", "[ ПОБОЧНОЕ ]",
    "[*]", "[+]", "[x]", "[УДАР]", "[Цель]", "[Босс]", "[Элита]"
]

for i, line in enumerate(lines):
    found = [t for t in tags_to_check if t in line]
    if found:
        print(f"Line {i+1}: {found} -> {line.strip()[:100]}")
