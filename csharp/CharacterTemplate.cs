namespace MyRPGGame;

/// <summary>
/// Шаблон персонажа (CharacterTemplate).
/// Управляет накоплением боевых стаков, врожденным умением (композиция)
/// и экипированным оружием (агрегация).
/// </summary>
public class CharacterTemplate
{
    // Композиция: часть (SkillTemplate) создается внутри конструктора и наружу не отдается.
    // Время жизни части полностью контролируется объектом CharacterTemplate.
    private readonly SkillTemplate _innateSkill;

    // Агрегация: объект оружия создается снаружи и передается персонажу готовым.
    private WeaponTemplate? _activeWeapon;

    private int _skillStacks;

    public string Id { get; }
    public string Name { get; }
    public int SkillStacks => _skillStacks;
    public string InnateSkillName => _innateSkill.Name;
    public string? ActiveWeaponName => _activeWeapon?.Name;
    public bool HasWeapon => _activeWeapon != null;

    public CharacterTemplate(string id, string name)
    {
        Id = id;
        Name = name;
        _skillStacks = 0;

        // Композиция: целое создает часть само
        _innateSkill = new SkillTemplate($"{id}_innate_blast", "Врожденный взрыв стихий", 0, stacksGranted: 1);
    }

    /// <summary>
    /// Агрегация: экипировка внешнего объекта оружия
    /// </summary>
    public void EquipWeapon(WeaponTemplate weapon)
    {
        _activeWeapon = weapon ?? throw new ArgumentNullException(nameof(weapon), "Нельзя экипировать пустое оружие!");
    }

    public void UnequipWeapon()
    {
        _activeWeapon = null;
    }

    public void AddSkillStack(int amount = 1)
    {
        if (amount > 0)
        {
            _skillStacks += amount;
        }
    }

    public void ResetSkillStacks()
    {
        _skillStacks = 0;
    }

    /// <summary>
    /// Совершить атаку.
    /// Проверка правила предметной области:
    /// Запрещено проводить атаку, если оружие не экипировано (указатель/ссылка равна null).
    /// </summary>
    public bool PerformAttack(out double damageDealt)
    {
        if (_activeWeapon == null)
        {
            damageDealt = 0.0;
            return false; // Срабатывание проверки правила
        }

        AddSkillStack(_innateSkill.StacksGranted);
        double innateBonus = _innateSkill.CalculateValue(15.0, _skillStacks);
        damageDealt = _activeWeapon.CurrentDamage + innateBonus;
        return true;
    }

    /// <summary>
    /// Применить ультимативное умение (расходует накопленные стаки)
    /// </summary>
    public double PerformUltimate()
    {
        double weaponDmg = _activeWeapon?.CurrentDamage ?? 0.0;
        double ultDmg = weaponDmg + (_innateSkill.CalculateValue(30.0, _skillStacks) * 1.5);
        ResetSkillStacks();
        return ultDmg;
    }

    public override string ToString() => $"Герой '{Name}' [Оружие: {(_activeWeapon != null ? _activeWeapon.Name : "нет")}, Стаки: {_skillStacks}]";
}
