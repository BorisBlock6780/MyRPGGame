namespace MyRPGGame;

/// <summary>
/// Шаблон умения (SkillTemplate).
/// Отвечает за хранение параметров навыка и динамический расчет урона.
/// </summary>
public class SkillTemplate
{
    public string Id { get; }
    public string Name { get; }
    public int Cooldown { get; }
    public int StacksGranted { get; }

    public SkillTemplate(string id, string name, int cooldown, int stacksGranted = 1)
    {
        // Проверка правила предметной области:
        // Кулдаун умения не может быть отрицательным числом
        if (cooldown < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(cooldown), "Кулдаун умения не может быть отрицательным!");
        }

        Id = id;
        Name = name;
        Cooldown = cooldown;
        StacksGranted = Math.Max(0, stacksGranted);
    }

    /// <summary>
    /// Динамический расчет силы умения с учетом накопленных стаков
    /// </summary>
    public virtual double CalculateValue(double baseStat, int skillStacks)
    {
        return (baseStat * 0.60) + (skillStacks * 15.0);
    }

    public override string ToString() => $"Навык '{Name}' (Кулдаун: {Cooldown} хода, +{StacksGranted} стак)";
}
