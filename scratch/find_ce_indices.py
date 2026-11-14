with open(r'src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

start_lower = text.find("float colW1 = 330.0f;")
end_lower = text.find("renderSlotPickerModal(currentChar);", start_lower)
print('start_lower:', start_lower, 'end_lower:', end_lower)
