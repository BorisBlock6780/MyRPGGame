import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

helper = """
static void drawEllipseFilled(ImDrawList* drawList, const ImVec2& center, float rx, float ry, ImU32 col, int num_segments = 16) {
    std::vector<ImVec2> pts;
    pts.reserve(num_segments);
    for (int i = 0; i < num_segments; ++i) {
        float a = (static_cast<float>(i) / static_cast<float>(num_segments)) * 6.2831853f;
        pts.push_back(ImVec2(center.x + cosf(a) * rx, center.y + sinf(a) * ry));
    }
    drawList->AddConvexPolyFilled(pts.data(), static_cast<int>(pts.size()), col);
}
"""

marker = "// ПРОЦЕДУРНЫЕ АНИМИРОВАННЫЕ МОДЕЛИ БОЯ (ГЕРОИ И ПРОТИВНИКИ)"
if "static void drawEllipseFilled" not in content:
    content = content.replace(marker, helper + "\n" + marker, 1)

content = content.replace("drawList->AddEllipseFilled(", "drawEllipseFilled(drawList, ")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("AddEllipseFilled replaced with drawEllipseFilled!")
