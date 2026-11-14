namespace MyRPGGame;

/// <summary>
/// Шаблон оружия (WeaponTemplate).
/// Описывает базовые характеристики оружия и логику заточки.
/// </summary>
public class WeaponTemplate
{
    private int _level;

    public string Id { get; }
    public string Name { get; }
    public double BaseDamage { get; }
    public int MaxLevel { get; } = 20;

    public int Level => _level;

    /// <summary>
    /// Динамический расчет урона: +8% базового урона за каждый уровень заточки выше первого
    /// </summary>
    public double CurrentDamage => BaseDamage * (1.0 + (_level - 1) * 0.08);

    public WeaponTemplate(string id, string name, double baseDamage, int initialLevel = 1)
    {
        if (initialLevel < 1 || initialLevel > MaxLevel)
        {
            throw new ArgumentOutOfRangeException(nameof(initialLevel), $"Уровень заточки должен быть в пределах от 1 до {MaxLevel}!");
        }

        Id = id;
        Name = name;
        BaseDamage = Math.Max(0.0, baseDamage);
        _level = initialLevel;
    }

    /// <summary>
    /// Заточка оружия.
    /// Проверка правила предметной области: уровень заточки не может превышать 20-й уровень.
    /// </summary>
    public bool Upgrade()
    {
        if (_level >= MaxLevel)
        {
            return false; // Правило: заточка выше 20 уровня запрещена
        }
        _level++;
        return true;
    }

    public override string ToString() => $"Оружие '{Name}' (Ур. {Level}/{MaxLevel}, Урон: {CurrentDamage:F1})";
}
