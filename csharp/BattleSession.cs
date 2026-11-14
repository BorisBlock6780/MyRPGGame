namespace MyRPGGame;

/// <summary>
/// Класс управления ресурсом боя без деструктора.
/// Реализует интерфейс IDisposable для использования с блоком using.
/// При выходе из блока using гарантированно освобождает ресурс (закрывает сессию).
/// </summary>
public sealed class BattleSession : IDisposable
{
    private readonly string _sessionTitle;
    private bool _isDisposed;

    public BattleSession(string sessionTitle)
    {
        _sessionTitle = sessionTitle;
        _isDisposed = false;
        Console.WriteLine($"[+] Журнал сессии открыт: {_sessionTitle} (ресурс выделен)");
    }

    public void WriteLog(string message)
    {
        if (_isDisposed)
        {
            throw new ObjectDisposedException(_sessionTitle, "Нельзя писать в закрытую сессию боя!");
        }
        Console.WriteLine($"    [Сессия] {message}");
    }

    public void Dispose()
    {
        if (!_isDisposed)
        {
            _isDisposed = true;
            Console.WriteLine($"[-] Журнал сессии закрыт: {_sessionTitle} (ресурс освобожден в Dispose)");
        }
    }
}
