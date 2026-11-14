with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

print('--- Start of lower section ---')
for j in range(3035, 3065):
    print(lines[j].encode('ascii', 'replace').decode('ascii').rstrip())

print('--- End of lower section ---')
for j in range(3245, 3288):
    print(lines[j].encode('ascii', 'replace').decode('ascii').rstrip())
