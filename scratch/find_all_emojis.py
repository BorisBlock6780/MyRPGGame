import re

def find_emojis(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    results = []
    for line_idx, line in enumerate(lines):
        # Find characters that are neither standard ASCII (0-127) nor Cyrillic (0x0400-0x04FF)
        for char_idx, ch in enumerate(line):
            code = ord(ch)
            if code > 127 and not (0x0400 <= code <= 0x04FF) and ch not in ['«', '»', '—', '–', '№', '“', '”', '…', '\t', '\r', '\n']:
                results.append((line_idx + 1, ch, hex(code), line.strip()[:80]))
    return results

emojis_cpp = find_emojis(r'src/ScreenManager.cpp')
print(f'Total emoji/special occurrences in ScreenManager.cpp: {len(emojis_cpp)}')
for line_no, ch, code, snippet in emojis_cpp[:40]:
    print(f'Line {line_no} [{ch} / {code}]: {snippet.encode("ascii", "replace").decode("ascii")}')
