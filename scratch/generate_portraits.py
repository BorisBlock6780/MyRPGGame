import os
import math
from PIL import Image, ImageDraw

PORTRAITS_DIR = r"assets\portraits"
os.makedirs(PORTRAITS_DIR, exist_ok=True)

# 20 unique characters with individual signature details
CHARS = [
    {
        "id": "adeir", "name": "Адеир", "role": "ОГОНЬ",
        "bg": (65, 20, 15), "accent": (255, 120, 40), "acc_type": "fire_staff"
    },
    {
        "id": "renwick", "name": "Ренвик", "role": "РЫЦАРЬ",
        "bg": (25, 38, 62), "accent": (140, 200, 255), "acc_type": "knight_plume"
    },
    {
        "id": "garosh", "name": "Гарош", "role": "ДЖАГГЕР",
        "bg": (42, 42, 48), "accent": (190, 195, 210), "acc_type": "spiked_shield"
    },
    {
        "id": "vex", "name": "Векс", "role": "СЛЕДОПЫТ",
        "bg": (20, 48, 28), "accent": (110, 230, 130), "acc_type": "quiver_bow"
    },
    {
        "id": "kaelim", "name": "Каэлим", "role": "АССАСИН",
        "bg": (38, 16, 48), "accent": (210, 90, 255), "acc_type": "dual_daggers"
    },
    {
        "id": "velari", "name": "Велари", "role": "ЖРИЦА",
        "bg": (20, 48, 52), "accent": (100, 245, 225), "acc_type": "halo_ankh"
    },
    {
        "id": "tauros", "name": "Таурос", "role": "БЕРСЕРК",
        "bg": (58, 15, 20), "accent": (255, 75, 75), "acc_type": "bull_horns"
    },
    {
        "id": "lumien", "name": "Люмиен", "role": "ПАЛАДИН",
        "bg": (58, 52, 22), "accent": (255, 225, 75), "acc_type": "sun_crest"
    },
    {
        "id": "silas", "name": "Сайлас", "role": "НЕКРОМАНТ",
        "bg": (28, 16, 36), "accent": (160, 75, 235), "acc_type": "skull_orb"
    },
    {
        "id": "finn", "name": "Финн", "role": "ПЛУТ",
        "bg": (48, 36, 18), "accent": (235, 175, 70), "acc_type": "feather_hat"
    },
    {
        "id": "elix", "name": "Эликс", "role": "АЛХИМИК",
        "bg": (20, 48, 38), "accent": (85, 235, 165), "acc_type": "goggles_flask"
    },
    {
        "id": "tork", "name": "Торк", "role": "МЕХАНИК",
        "bg": (48, 34, 22), "accent": (235, 140, 50), "acc_type": "wrench_steam"
    },
    {
        "id": "mira", "name": "Мира", "role": "МОРОЗ",
        "bg": (20, 36, 62), "accent": (120, 215, 255), "acc_type": "frost_tiara"
    },
    {
        "id": "roland", "name": "Роланд", "role": "ДУЭЛЯНТ",
        "bg": (42, 28, 52), "accent": (220, 150, 245), "acc_type": "rose_cape"
    },
    {
        "id": "born", "name": "Борн", "role": "ГЕОМАНТ",
        "bg": (48, 38, 26), "accent": (200, 165, 95), "acc_type": "rock_boulder"
    },
    {
        "id": "mortis", "name": "Мортис", "role": "РЫЦАРЬ СМЕРТИ",
        "bg": (22, 18, 30), "accent": (145, 60, 190), "acc_type": "runeblade"
    },
    {
        "id": "siegfried", "name": "Зигфрид", "role": "УБИЙЦА ДРАКОНОВ",
        "bg": (52, 24, 18), "accent": (255, 140, 55), "acc_type": "dragon_crest"
    },
    {
        "id": "sylvana", "name": "Сильвана", "role": "ВИХРЬ",
        "bg": (20, 42, 36), "accent": (110, 235, 185), "acc_type": "wind_sickles"
    },
    {
        "id": "drake", "name": "Дрейк", "role": "КОРСАР",
        "bg": (26, 36, 48), "accent": (100, 185, 245), "acc_type": "eyepatch_cutlass"
    },
    {
        "id": "cormac", "name": "Кормак", "role": "ДРУИД",
        "bg": (26, 42, 22), "accent": (145, 225, 85), "acc_type": "stag_antlers"
    }
]

def draw_portrait(char_info):
    W, H = 140, 160
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    bg = char_info["bg"]
    acc = char_info["accent"]
    acc_type = char_info["acc_type"]

    # Frame
    d.rounded_rectangle((4, 4, W - 4, H - 4), radius=12, fill=(bg[0], bg[1], bg[2], 250), outline=acc, width=3)
    d.rounded_rectangle((8, 8, W - 8, H - 8), radius=8, outline=(acc[0], acc[1], acc[2], 100), width=1)

    cx, cy = W // 2, 66

    # Specific Headgear / Horns / Hair / Features BEFORE Head
    if acc_type == "bull_horns":
        # Massive Minotaur ivory horns
        d.arc((cx - 38, cy - 42, cx - 14, cy - 14), 180, 360, fill=(240, 230, 200, 255), width=6)
        d.arc((cx + 14, cy - 42, cx + 38, cy - 14), 180, 360, fill=(240, 230, 200, 255), width=6)
    elif acc_type == "stag_antlers":
        # Branching stag antlers
        d.line([(cx - 18, cy - 28), (cx - 36, cy - 50)], fill=(160, 110, 50, 255), width=4)
        d.line([(cx - 28, cy - 42), (cx - 40, cy - 38)], fill=(160, 110, 50, 255), width=3)
        d.line([(cx + 18, cy - 28), (cx + 36, cy - 50)], fill=(160, 110, 50, 255), width=4)
        d.line([(cx + 28, cy - 42), (cx + 40, cy - 38)], fill=(160, 110, 50, 255), width=3)
    elif acc_type == "halo_ankh":
        # Floating golden halo
        d.ellipse((cx - 20, cy - 46, cx + 20, cy - 36), outline=(255, 235, 100, 255), width=3)
    elif acc_type == "frost_tiara":
        # Crystalline tiara
        d.polygon([(cx - 16, cy - 26), (cx, cy - 38), (cx + 16, cy - 26), (cx, cy - 22)], fill=(180, 240, 255, 255), outline=acc, width=2)
    elif acc_type == "knight_plume":
        # Helmet plume
        d.arc((cx - 8, cy - 46, cx + 24, cy - 22), 180, 360, fill=(70, 140, 255, 255), width=6)
    elif acc_type == "feather_hat":
        # Swashbuckler hat
        d.ellipse((cx - 32, cy - 32, cx + 32, cy - 18), fill=(40, 30, 20, 255), outline=acc, width=2)
        d.line([(cx - 16, cy - 28), (cx - 28, cy - 44)], fill=(255, 215, 60, 255), width=4)

    # Shoulders
    d.polygon([(cx - 46, cy + 44), (cx - 26, cy + 10), (cx + 26, cy + 10), (cx + 46, cy + 44)],
              fill=(acc[0]//3 + 20, acc[1]//3 + 20, acc[2]//3 + 20, 255), outline=acc, width=2)

    # Head
    d.ellipse((cx - 20, cy - 30, cx + 20, cy + 12), fill=(bg[0]+30, bg[1]+30, bg[2]+30, 255), outline=acc, width=2)

    # Eyes / Visor / Mask
    if acc_type == "eyepatch_cutlass":
        d.ellipse((cx - 10, cy - 12, cx - 4, cy - 6), fill=(10, 10, 15, 255)) # eyepatch
        d.ellipse((cx + 4, cy - 12, cx + 10, cy - 6), fill=acc) # bright eye
    elif acc_type == "goggles_flask":
        # Brass goggles on forehead
        d.ellipse((cx - 14, cy - 24, cx - 2, cy - 14), fill=(120, 240, 180, 255), outline=(220, 180, 50, 255), width=2)
        d.ellipse((cx + 2, cy - 24, cx + 14, cy - 14), fill=(120, 240, 180, 255), outline=(220, 180, 50, 255), width=2)
        d.ellipse((cx - 8, cy - 10, cx - 4, cy - 6), fill=acc)
        d.ellipse((cx + 4, cy - 10, cx + 8, cy - 6), fill=acc)
    else:
        d.ellipse((cx - 9, cy - 12, cx - 3, cy - 6), fill=acc)
        d.ellipse((cx + 3, cy - 12, cx + 9, cy - 6), fill=acc)

    # Signature Front Accessories
    if acc_type == "fire_staff":
        # Pulsing flame orb
        d.ellipse((cx + 26, cy - 10, cx + 42, cy + 6), fill=(255, 120, 30, 255), outline=(255, 220, 100, 255), width=2)
    elif acc_type == "spiked_shield":
        # Giant spiked shield on left shoulder
        d.rectangle((cx - 44, cy + 2, cx - 24, cy + 38), fill=(55, 60, 70, 255), outline=(200, 205, 215, 255), width=2)
        d.polygon([(cx - 48, cy + 18), (cx - 44, cy + 14), (cx - 44, cy + 22)], fill=(220, 225, 235, 255))
    elif acc_type == "quiver_bow":
        # Longbow curve
        d.arc((cx + 20, cy - 20, cx + 46, cy + 40), 270, 90, fill=(160, 110, 50, 255), width=3)
    elif acc_type == "dual_daggers":
        # Cross daggers
        d.line([(cx - 14, cy + 20), (cx + 14, cy + 38)], fill=acc, width=3)
        d.line([(cx + 14, cy + 20), (cx - 14, cy + 38)], fill=acc, width=3)
    elif acc_type == "sun_crest":
        # Sunburst
        d.ellipse((cx - 10, cy + 16, cx + 10, cy + 36), fill=(255, 215, 60, 255), outline=(255, 245, 180, 255), width=2)
    elif acc_type == "skull_orb":
        # Green skull orb
        d.ellipse((cx + 24, cy + 10, cx + 38, cy + 24), fill=(60, 240, 120, 255), outline=(200, 255, 220, 255), width=2)
    elif acc_type == "goggles_flask":
        # Potion flask
        d.ellipse((cx + 24, cy + 16, cx + 38, cy + 34), fill=(80, 240, 160, 255), outline=(240, 245, 255, 255), width=2)
    elif acc_type == "wrench_steam":
        # Big brass wrench
        d.line([(cx + 20, cy - 8), (cx + 38, cy + 28)], fill=(220, 160, 50, 255), width=5)
    elif acc_type == "rose_cape":
        # Red rose badge
        d.ellipse((cx - 8, cy + 18, cx + 8, cy + 34), fill=(220, 30, 60, 255))
    elif acc_type == "rock_boulder":
        # Floating boulders over shoulders
        d.polygon([(cx - 36, cy - 6), (cx - 24, cy - 14), (cx - 18, cy - 4), (cx - 28, cy + 6)], fill=(120, 105, 85, 255), outline=acc, width=2)
    elif acc_type == "runeblade":
        # Glowing cyan runes
        d.line([(cx + 22, cy - 6), (cx + 34, cy + 36)], fill=(20, 20, 25, 255), width=5)
        d.line([(cx + 24, cy - 2), (cx + 32, cy + 30)], fill=(80, 220, 255, 255), width=2)
    elif acc_type == "dragon_crest":
        # Dragon wing pauldrons
        d.polygon([(cx - 46, cy + 6), (cx - 30, cy + 16), (cx - 42, cy + 28)], fill=(200, 50, 30, 255), outline=(255, 180, 60, 255), width=2)
    elif acc_type == "wind_sickles":
        # Crescent sickle
        d.arc((cx + 18, cy + 10, cx + 38, cy + 36), 180, 0, fill=(120, 245, 200, 255), width=4)

    # Role badge at top
    role_w = len(char_info["role"]) * 7 + 16
    d.rounded_rectangle((cx - role_w//2, 8, cx + role_w//2, 22), radius=4, fill=(10, 10, 15, 230), outline=acc, width=1)
    d.text((cx - role_w//2 + 8, 9), char_info["role"], fill=acc)

    # Name badge at bottom
    name_w = len(char_info["name"]) * 8
    d.rounded_rectangle((10, H - 32, W - 10, H - 10), radius=5, fill=(10, 12, 18, 235), outline=acc, width=1)
    d.text((cx - name_w//2 + 4, H - 28), char_info["name"], fill=(245, 245, 250, 255))

    out_path = os.path.join(PORTRAITS_DIR, f"{char_info['id']}.png")
    img.save(out_path, format="PNG")

if __name__ == "__main__":
    for c in CHARS:
        draw_portrait(c)
    print("All 20 detailed portraits generated successfully!")
