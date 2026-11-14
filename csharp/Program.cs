using System;
using System.Runtime.CompilerServices;
using MyRPGGame;

namespace MyRPGGame;

internal class Program
{
    private static void Main()
    {
        // Настройка кодировки вывода для корректного отображения кириллицы в консоли Windows
        Console.OutputEncoding = System.Text.Encoding.UTF8;

        Console.WriteLine("================================================================================");
        Console.WriteLine("    ЛАБОРАТОРНАЯ РАБОТА 3 (ЧАСТЬ 1: C# - УПРАВЛЯЕМАЯ ПАМЯТЬ)");
        Console.WriteLine("    Курсовой проект: RPG Game System (Пошаговая RPG система)");
        Console.WriteLine("================================================================================\n");

        // -----------------------------------------------------------------------------
        // РАЗДЕЛ 1. ДЕМОНСТРАЦИЯ СЦЕНАРИЯ ИЗ ЛР 2
        // Композиция, агрегация, проверки бизнес-правил, серия атак и ультимейт
        // -----------------------------------------------------------------------------
        Console.WriteLine("--- 1. Сценарий предметной области (ЛР 2) на языке C# ---");

        // 1.1 Создание оружия (будет агрегироваться)
        var sword = new WeaponTemplate("iron_sword", "Стальной Меч", baseDamage: 25.0);
        Console.WriteLine($"[Создано оружие] {sword}");

        // 1.2 Создание персонажа (композиция: врожденный навык создается внутри)
        var hero = new CharacterTemplate("adeir", "Адеир");
        Console.WriteLine($"[Создан герой]   {hero}");
        Console.WriteLine($"Врожденный навык (композиция): {hero.InnateSkillName} (объект скрыт внутри героя)");

        // 1.3 Проверка правила 1: Атака без оружия запрещена
        Console.WriteLine("\n[Проверка правила 1] Попытка атаковать без экипированного оружия:");
        if (!hero.PerformAttack(out _))
        {
            Console.WriteLine(" -> ОШИБКА: Запрещено проводить атаку, если оружие не экипировано! (Правило сработало)");
        }

        // 1.4 Агрегация: экипировка внешнего оружия
        Console.WriteLine("\n[Агрегация] Экипируем оружие герою:");
        hero.EquipWeapon(sword);
        Console.WriteLine($" -> {hero}");

        // 1.5 Проверка правила 2: Заточка оружия не может превышать 20 уровень
        Console.WriteLine("\n[Проверка правила 2] Заточка оружия и ограничение максимального уровня (20):");
        for (int i = 0; i < 19; ++i)
        {
            sword.Upgrade();
        }
        Console.WriteLine($"Оружие после успешной серии заточек: {sword}");
        bool canUpgradeFurther = sword.Upgrade();
        Console.WriteLine($"Попытка заточить выше 20 уровня: {(canUpgradeFurther ? "Успех" : "ОТКЛОНЕНО (Правило: максимум 20-й уровень)")}");

        // 1.6 Проверка правила 3: Отрицательный кулдаун умения запрещен в конструкторе
        Console.WriteLine("\n[Проверка правила 3] Создание навыка с отрицательным кулдауном (-5):");
        try
        {
            var invalidSkill = new SkillTemplate("invalid", "Сломанный навык", -5);
        }
        catch (ArgumentOutOfRangeException ex)
        {
            Console.WriteLine($" -> Перехвачено исключение: {ex.Message}");
        }

        // 1.7 Боевой сценарий из ЛР 2: атака 1, серия атак, ультимейт
        Console.WriteLine("\n=== ХОД БОЯ (Боевая симуляция) ===");
        
        // Применена 1 атака
        if (hero.PerformAttack(out double dmg1))
        {
            Console.WriteLine($"Применена 1 атака: Боец {hero.Name} наносит {dmg1:F1} урона. {hero}");
        }

        // Применена серия атак
        Console.WriteLine("\nПрименена серия атак (накопление боевых стаков):");
        for (int i = 0; i < 8; ++i)
        {
            hero.PerformAttack(out double seriesDmg);
            Console.WriteLine($"  Атака {i + 2}: нанесен урон {seriesDmg:F1} | Боевые стаки: {hero.SkillStacks}");
        }
        Console.WriteLine($"Итог серии: {hero}");

        // Применен ультимативный навык
        Console.WriteLine("\nПрименен ультимативный навык (расходует накопленные стаки):");
        double ultDmg = hero.PerformUltimate();
        Console.WriteLine($"  >>> ВЗРВЫВ СТИХИЙ! Нанесен колоссальный урон: {ultDmg:F1} | Стаки сброшены: {hero.SkillStacks}");
        Console.WriteLine($"Состояние героя после ульты: {hero}\n");

        // -----------------------------------------------------------------------------
        // РАЗДЕЛ 2. ПАМЯТЬ: ССЫЛКИ И КОПИИ (CLASS VS STRUCT)
        // -----------------------------------------------------------------------------
        Console.WriteLine("================================================================================");
        Console.WriteLine("--- 2. Демонстрация работы с памятью: Ссылки (class) и Копии (struct) ---");

        // 2.1 Ссылочный тип (class): обе переменные указывают на один и тот же объект в куче
        Console.WriteLine("\n[2.1 Ссылочный тип - class CharacterTemplate]:");
        var heroFirst = new CharacterTemplate("mage", "Маг Огня");
        heroFirst.AddSkillStack(2);
        Console.WriteLine($"Исходный объект heroFirst:              {heroFirst}");

        var heroSecond = heroFirst; // Копируется ССЫЛКА, а не сам объект!
        Console.WriteLine($"Присвоили heroSecond = heroFirst (копия ссылки).");

        heroSecond.AddSkillStack(10); // Изменяем объект через ВТОРУЮ переменную
        Console.WriteLine($"Изменили стаки через heroSecond (+10).");
        Console.WriteLine($"Результат heroFirst:                    {heroFirst} <-- ИЗМЕНИЛСЯ!");
        Console.WriteLine($"Результат heroSecond:                   {heroSecond}");
        Console.WriteLine($"Проверка ссылочной идентичности (heroFirst == heroSecond): {ReferenceEquals(heroFirst, heroSecond)}");

        // 2.2 Тип-значение (struct): переменные хранят независимые значения на стеке
        Console.WriteLine("\n[2.2 Тип-значение - struct CombatStats]:");
        var statsA = new CombatStats(attack: 100, defense: 50, hp: 500);
        Console.WriteLine($"Исходная структура statsA:              {statsA}");

        var statsB = statsA; // Копируется ЗНАЧЕНИЕ целиком (независимая копия)!
        Console.WriteLine($"Присвоили statsB = statsA (полная копия значения в памяти).");

        statsB.Attack = 999; // Модифицируем поле только во второй копии
        statsB.Hp = 1;
        Console.WriteLine($"Изменили поля в statsB (Attack=999, HP=1).");
        Console.WriteLine($"Результат statsA:                       {statsA} <-- НЕ ИЗМЕНИЛАСЬ!");
        Console.WriteLine($"Результат statsB:                       {statsB}");
        Console.WriteLine("Вывод: классы передаются по ссылке и разделяют состояние в куче,");
        Console.WriteLine("а структуры копируют значения целиком и полностью независимы.\n");

        // -----------------------------------------------------------------------------
        // РАЗДЕЛ 3. ОСВОБОЖДЕНИЕ РЕСУРСА БЕЗ ДЕСТРУКТОРА (IDisposable и using)
        // -----------------------------------------------------------------------------
        Console.WriteLine("================================================================================");
        Console.WriteLine("--- 3. Освобождение ресурса без деструктора (IDisposable и using) ---");
        Console.WriteLine("Вход в блок using...");
        using (var session = new BattleSession("Рейд на Костяного Колосса"))
        {
            session.WriteLog("Игрок нанес критический удар мечом.");
            session.WriteLog("Босс контратаковал заклинанием тьмы.");
            Console.WriteLine("Достигнут конец блока using (сейчас будет неявно вызван session.Dispose())...");
        }
        Console.WriteLine("Выход из блока using завершен. Ресурс гарантированно закрыт без ожидания GC!\n");

        // -----------------------------------------------------------------------------
        // РАЗДЕЛ 4. СБОРЩИК МУСОРА (GC.Collect и WeakReference)
        // -----------------------------------------------------------------------------
        Console.WriteLine("================================================================================");
        Console.WriteLine("--- 4. Демонстрация работы сборщика мусора (GC.Collect и WeakReference) ---");

        var watcher = CreateTemporaryHero();

        // Проверяем, жив ли объект сразу после выхода из метода-создателя
        bool isAliveBeforeGc = watcher.TryGetTarget(out var aliveRef);
        Console.WriteLine($"Объект покинул метод создания. Жив ли он в памяти до вызова GC? -> {isAliveBeforeGc}");
        if (isAliveBeforeGc && aliveRef != null)
        {
            Console.WriteLine($"  (Объект '{aliveRef.Name}' все еще существует в куче, так как GC еще не запускался)");
        }
        aliveRef = null; // Убираем сильную локальную ссылку

        Console.WriteLine("\nЗапрашиваем принудительную сборку мусора: GC.Collect() + WaitForPendingFinalizers()...");
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect();

        bool isAliveAfterGc = watcher.TryGetTarget(out _);
        Console.WriteLine($"Жив ли объект после выполнения сборки мусора?                  -> {isAliveAfterGc}");
        Console.WriteLine(isAliveAfterGc ? "  Объект еще жив." : "  -> ОБЪЕКТ УНИЧТОЖЕН СБОРЩИКОМ МУСОРА!");

        Console.WriteLine("\nСравнение с ЛР 2 (C++):");
        Console.WriteLine("В C++ локальный объект на стеке уничтожался НЕМЕДЛЕННО в момент выхода из блока {},");
        Console.WriteLine("и деструктор отрабатывал синхронно. В C# выход из блока не уничтожает объект в куче;");
        Console.WriteLine("память освобождается асинхронно сборщиком мусора, когда на объект больше нет сильных ссылок.");
        Console.WriteLine("================================================================================");
    }

    /// <summary>
    /// Метод для создания временного объекта.
    /// Атрибут NoInlining гарантирует, что JIT-компилятор не удержит локальную переменную
    /// в стековом кадре вызывающего метода Main в Debug/Release сборке.
    /// </summary>
    [MethodImpl(MethodImplOptions.NoInlining)]
    private static WeakReference<CharacterTemplate> CreateTemporaryHero()
    {
        var tempHero = new CharacterTemplate("temp_ghost", "Призрачный фантом");
        Console.WriteLine($"Внутри CreateTemporaryHero: создан временный объект '{tempHero.Name}'");
        return new WeakReference<CharacterTemplate>(tempHero);
    }
}
