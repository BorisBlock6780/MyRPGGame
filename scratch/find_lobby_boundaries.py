with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

start_idx = text.find('NavTarget targets[]')
end_str = 'ImGui::SetCursorPos(ImVec2(posX, posY));'
end_idx = text.find(end_str, start_idx) + len(end_str)

print('start_idx:', start_idx, 'end_idx:', end_idx)
print('Length of block:', end_idx - start_idx)
