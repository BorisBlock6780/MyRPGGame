import unicodedata

def scan_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        text = f.read()

    emojis = {}
    for ch in text:
        code = ord(ch)
        if code > 127 and not (0x0400 <= code <= 0x04FF) and ch not in ['«', '»', '—', '–', '№', '“', '”', '…', '\t', '\r', '\n']:
            try:
                name = unicodedata.name(ch)
            except:
                name = "UNKNOWN"
            emojis[ch] = (hex(code), name, emojis.get(ch, (hex(code), name, 0))[2] + 1)
    return emojis

res = scan_file(r'src/ScreenManager.cpp')
print(f'Unique non-ascii/cyrillic chars: {len(res)}')
for ch, (code, name, cnt) in res.items():
    print(f'{code} ({name}): count = {cnt}')

res_hpp = scan_file(r'include/ScreenManager.hpp')
print(f'HPP non-ascii/cyrillic chars: {len(res_hpp)}')
for ch, (code, name, cnt) in res_hpp.items():
    print(f'{code} ({name}): count = {cnt}')
