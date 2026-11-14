namespace MyRPGGame;

/// <summary>
/// Тип-значение (структура) для демонстрации работы с памятью в C#.
/// При присваивании и передаче копируется целиком по значению, а не по ссылке.
/// </summary>
public struct CombatStats
{
    public double Attack;
    public double Defense;
    public double Hp;

    public CombatStats(double attack, double defense, double hp)
    {
        Attack = attack;
        Defense = defense;
        Hp = hp;
    }

    public override string ToString() => $"[HP: {Hp:F0}, ATK: {Attack:F0}, DEF: {Defense:F0}]";
}
