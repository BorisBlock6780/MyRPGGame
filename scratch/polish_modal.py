cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

old_block = """    if (ImGui::BeginPopupModal("##EquipItemModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {"""
new_block = """    bool isModalOpen = true;
    if (ImGui::BeginPopupModal("##EquipItemModal", &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {"""

if "\r\n" in content:
    old_block = old_block.replace("\n", "\r\n")
    new_block = new_block.replace("\n", "\r\n")

assert old_block in content, "old_block not found"
content = content.replace(old_block, new_block, 1)

old_end = """        ImGui::EndPopup();
    }
}

void ScreenManager::renderCharacterEditor() {"""

new_end = """        ImGui::EndPopup();
    }

    if (!isModalOpen || (!requestOpenSlotPicker && !ImGui::IsPopupOpen("##EquipItemModal"))) {
        activeSlotPicker = ArtifactType::None;
    }
}

void ScreenManager::renderCharacterEditor() {"""

if "\r\n" in content:
    old_end = old_end.replace("\n", "\r\n")
    new_end = new_end.replace("\n", "\r\n")

assert old_end in content, "old_end not found"
content = content.replace(old_end, new_end, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Modal popup state handling perfected!")
