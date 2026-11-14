import os
import math
import random
from PIL import Image, ImageDraw

ICONS_DIR = r"assets\icons"
os.makedirs(ICONS_DIR, exist_ok=True)

def make_base(bg_color, border_color):
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # Beveled rounded frame
    d.rounded_rectangle((6, 6, 122, 122), radius=16, fill=bg_color, outline=border_color, width=3)
    d.rounded_rectangle((12, 12, 116, 116), radius=12, outline=(border_color[0], border_color[1], border_color[2], 90), width=1)
    return img, d

def save_icon(img, name):
    path = os.path.join(ICONS_DIR, f"{name}.png")
    img.save(path, format="PNG")

# 1. WEAPONS
def create_weapons():
    # Rusty blade
    img, d = make_base((35, 30, 25, 240), (160, 110, 60, 255))
    d.line([(34, 94), (94, 34)], fill=(170, 150, 130, 255), width=6)
    d.line([(30, 90), (90, 30)], fill=(130, 80, 45, 255), width=3) # rust
    d.line([(28, 88), (40, 100)], fill=(120, 75, 35, 255), width=5)
    d.ellipse((22, 96, 30, 104), fill=(100, 60, 30, 255))
    save_icon(img, "rusty_blade")

    # Old cleaver
    img, d = make_base((30, 32, 38, 240), (140, 150, 170, 255))
    d.polygon([(36, 88), (50, 102), (96, 56), (92, 32), (64, 40)], fill=(190, 195, 210, 255), outline=(220, 225, 240, 255), width=2)
    d.line([(24, 100), (38, 86)], fill=(110, 70, 40, 255), width=6)
    save_icon(img, "old_cleaver")

    # Berserker axe
    img, d = make_base((45, 18, 20, 240), (230, 60, 50, 255))
    d.line([(30, 98), (98, 30)], fill=(120, 80, 50, 255), width=5)
    # Twin blades
    d.polygon([(82, 46), (108, 20), (104, 58), (86, 54)], fill=(180, 40, 40, 255), outline=(255, 120, 90, 255), width=2)
    d.polygon([(64, 28), (40, 54), (74, 50)], fill=(180, 40, 40, 255), outline=(255, 120, 90, 255), width=2)
    save_icon(img, "berserker_axe")

    # Vampire blade
    img, d = make_base((40, 12, 25, 240), (240, 40, 110, 255))
    d.arc((32, 32, 104, 104), 180, 315, fill=(230, 30, 70, 255), width=6)
    d.line([(28, 92), (38, 102)], fill=(200, 160, 50, 255), width=5)
    d.ellipse((36, 90, 44, 98), fill=(255, 20, 60, 255))
    save_icon(img, "vampire_blade")

    # Apprentice staff
    img, d = make_base((20, 30, 45, 240), (80, 160, 255, 255))
    d.line([(32, 98), (92, 38)], fill=(135, 90, 50, 255), width=5)
    d.ellipse((84, 24, 106, 46), fill=(40, 140, 240, 255), outline=(160, 220, 255, 255), width=3)
    d.ellipse((90, 30, 100, 40), fill=(220, 245, 255, 255))
    save_icon(img, "apprentice_staff")

    # Hunter bow
    img, d = make_base((22, 38, 26, 240), (100, 210, 120, 255))
    d.arc((26, 26, 102, 102), 45, 225, fill=(160, 110, 50, 255), width=5)
    d.line([(42, 42), (42, 86)], fill=(220, 230, 240, 200), width=2)
    d.line([(30, 64), (96, 64)], fill=(200, 210, 220, 255), width=3) # arrow
    d.polygon([(96, 64), (88, 58), (88, 70)], fill=(120, 220, 140, 255))
    save_icon(img, "hunter_bow")

    # Shadow dagger
    img, d = make_base((30, 18, 42, 240), (180, 80, 240, 255))
    d.polygon([(46, 82), (54, 90), (94, 34), (86, 38)], fill=(70, 30, 95, 255), outline=(210, 110, 255, 255), width=2)
    d.line([(38, 90), (48, 100)], fill=(180, 130, 50, 255), width=4)
    d.ellipse((88, 38, 94, 44), fill=(100, 255, 140, 255)) # poison drop
    save_icon(img, "shadow_dagger")

    # Bastion blade
    img, d = make_base((26, 32, 45, 240), (150, 190, 255, 255))
    d.line([(36, 92), (92, 36)], fill=(210, 225, 245, 255), width=7)
    d.line([(28, 84), (44, 100)], fill=(220, 175, 60, 255), width=6)
    # Crossguard mini-shield
    d.polygon([(46, 74), (56, 84), (48, 92), (38, 82)], fill=(60, 120, 200, 255), outline=(230, 240, 255, 255), width=2)
    save_icon(img, "bastion_blade")

# 2. MATERIALS
def create_materials():
    # Iron ore
    img, d = make_base((35, 36, 40, 240), (160, 165, 180, 255))
    d.polygon([(34, 76), (46, 44), (78, 36), (98, 62), (86, 92), (50, 96)], fill=(90, 95, 105, 255), outline=(190, 195, 210, 255), width=3)
    d.polygon([(52, 54), (74, 48), (82, 68), (62, 74)], fill=(140, 150, 170, 255))
    save_icon(img, "iron_ore")

    # Scrap metal
    img, d = make_base((38, 32, 28, 240), (180, 130, 80, 255))
    d.ellipse((44, 44, 84, 84), outline=(160, 110, 60, 255), width=6) # gear
    for a in range(0, 360, 45):
        rad = math.radians(a)
        x1 = 64 + math.cos(rad) * 20
        y1 = 64 + math.sin(rad) * 20
        x2 = 64 + math.cos(rad) * 30
        y2 = 64 + math.sin(rad) * 30
        d.line([(x1, y1), (x2, y2)], fill=(190, 140, 80, 255), width=4)
    save_icon(img, "scrap_metal")

    # Mithril ore
    img, d = make_base((20, 35, 50, 240), (90, 210, 255, 255))
    d.polygon([(40, 84), (50, 38), (76, 28), (94, 60), (84, 94), (54, 98)], fill=(45, 110, 170, 255), outline=(140, 235, 255, 255), width=3)
    d.polygon([(56, 44), (72, 38), (82, 60), (66, 72)], fill=(160, 240, 255, 255))
    save_icon(img, "mithril_ore")

    # Fire crystal
    img, d = make_base((45, 20, 15, 240), (255, 120, 40, 255))
    d.polygon([(64, 22), (88, 54), (76, 102), (52, 102), (40, 54)], fill=(220, 60, 20, 255), outline=(255, 180, 70, 255), width=3)
    d.polygon([(64, 32), (78, 56), (70, 92), (58, 92), (50, 56)], fill=(255, 140, 40, 255))
    save_icon(img, "fire_crystal")

    # Void shard
    img, d = make_base((28, 16, 42, 240), (200, 80, 255, 255))
    d.polygon([(64, 18), (92, 60), (64, 108), (36, 60)], fill=(90, 25, 140, 255), outline=(225, 130, 255, 255), width=3)
    d.polygon([(64, 30), (80, 60), (64, 94), (48, 60)], fill=(160, 60, 240, 255))
    save_icon(img, "void_shard")

    # Celestial ingot
    img, d = make_base((40, 36, 20, 240), (255, 215, 60, 255))
    d.polygon([(36, 74), (50, 48), (98, 48), (84, 74)], fill=(240, 190, 50, 255), outline=(255, 240, 140, 255), width=2)
    d.polygon([(36, 74), (84, 74), (76, 92), (28, 92)], fill=(200, 150, 30, 255), outline=(255, 220, 90, 255), width=2)
    save_icon(img, "celestial_ingot")

# 3. ARTIFACTS, JEWELRY & QUEST
def create_special_items():
    # Ancient core
    img, d = make_base((25, 30, 45, 240), (100, 200, 255, 255))
    d.ellipse((34, 34, 94, 94), fill=(35, 75, 130, 255), outline=(140, 220, 255, 255), width=4)
    d.ellipse((46, 46, 82, 82), outline=(255, 215, 60, 255), width=3)
    d.ellipse((54, 54, 74, 74), fill=(100, 240, 255, 255))
    save_icon(img, "ancient_core")

    # Sun stone
    img, d = make_base((45, 30, 15, 240), (255, 180, 50, 255))
    d.ellipse((36, 36, 92, 92), fill=(240, 120, 30, 255), outline=(255, 230, 100, 255), width=4)
    d.polygon([(64, 18), (68, 30), (60, 30)], fill=(255, 200, 60, 255))
    d.polygon([(64, 110), (68, 98), (60, 98)], fill=(255, 200, 60, 255))
    d.polygon([(18, 64), (30, 68), (30, 60)], fill=(255, 200, 60, 255))
    d.polygon([(110, 64), (98, 68), (98, 60)], fill=(255, 200, 60, 255))
    save_icon(img, "sun_stone")

    # Ruby ring
    img, d = make_base((38, 20, 26, 240), (255, 100, 120, 255))
    d.ellipse((38, 48, 90, 100), outline=(230, 180, 60, 255), width=6)
    d.polygon([(64, 28), (78, 42), (64, 56), (50, 42)], fill=(230, 30, 60, 255), outline=(255, 180, 200, 255), width=2)
    save_icon(img, "ruby_ring")

    # Gold necklace
    img, d = make_base((38, 35, 20, 240), (255, 215, 60, 255))
    d.arc((36, 28, 92, 84), 0, 180, fill=(230, 185, 60, 255), width=4)
    d.polygon([(64, 68), (76, 84), (64, 102), (52, 84)], fill=(40, 120, 220, 255), outline=(255, 230, 110, 255), width=2)
    save_icon(img, "gold_necklace")

    # Oasis key
    img, d = make_base((20, 36, 32, 240), (80, 220, 170, 255))
    d.ellipse((40, 28, 68, 56), outline=(220, 180, 60, 255), width=5)
    d.line([(54, 56), (54, 102)], fill=(220, 180, 60, 255), width=5)
    d.line([(54, 88), (68, 88)], fill=(220, 180, 60, 255), width=4)
    d.line([(54, 98), (68, 98)], fill=(220, 180, 60, 255), width=4)
    save_icon(img, "oasis_key")

    # Ancient scroll
    img, d = make_base((40, 35, 28, 240), (220, 190, 120, 255))
    d.rectangle((36, 38, 92, 90), fill=(215, 195, 155, 255), outline=(160, 130, 85, 255), width=3)
    d.ellipse((32, 34, 40, 94), fill=(180, 145, 95, 255), outline=(140, 110, 65, 255), width=2)
    d.ellipse((88, 34, 96, 94), fill=(180, 145, 95, 255), outline=(140, 110, 65, 255), width=2)
    d.ellipse((60, 60, 68, 68), fill=(180, 30, 30, 255)) # wax seal
    save_icon(img, "ancient_scroll")

# 4. FOOD & INGREDIENTS (generate clean, colorful thematic food icons)
FOODS = [
    # Floor 1
    ("bone_meal", (40, 38, 35), (200, 200, 190), "dust"),
    ("rat_meat", (45, 22, 22), (210, 70, 70), "meat"),
    ("rusty_mushroom", (42, 30, 24), (200, 110, 50), "mushroom"),
    ("glowing_moss", (20, 42, 30), (80, 240, 140), "moss"),
    ("edible_ash", (35, 35, 38), (160, 160, 175), "dust"),
    ("dirty_water", (25, 35, 42), (80, 160, 210), "flask"),
    # Floor 2
    ("cave_wheat", (40, 36, 22), (230, 200, 80), "wheat"),
    ("goat_milk", (32, 38, 45), (240, 245, 255), "flask"),
    ("gutted_fish", (24, 36, 45), (100, 180, 230), "fish"),
    ("wild_berries", (42, 18, 30), (230, 60, 120), "berry"),
    ("fresh_game", (46, 20, 22), (230, 80, 80), "meat"),
    ("greenhouse_apple", (42, 20, 20), (240, 60, 60), "apple"),
    # Floor 3
    ("severed_tentacle", (28, 22, 42), (180, 80, 220), "meat"),
    ("raw_seaweed", (18, 38, 28), (60, 200, 110), "moss"),
    ("jellyfish_slime", (20, 36, 46), (90, 220, 240), "flask"),
    ("shell_meat", (38, 30, 26), (220, 150, 110), "meat"),
    ("ocean_salt", (26, 36, 45), (220, 240, 255), "dust"),
    ("live_clam", (36, 32, 30), (200, 180, 160), "meat"),
    # Floor 4
    ("fire_grain", (45, 24, 16), (255, 120, 40), "wheat"),
    ("salamander_meat", (46, 18, 16), (240, 70, 40), "meat"),
    ("slag_root", (38, 28, 22), (190, 100, 50), "moss"),
    ("coal_dust", (28, 26, 28), (110, 110, 120), "dust"),
    ("magma_resin", (45, 20, 16), (255, 100, 30), "flask"),
    ("thermal_water", (30, 32, 44), (120, 180, 240), "flask"),
    # Dishes
    ("baked_apple", (44, 25, 18), (240, 120, 50), "apple"),
    ("hearty_mushroom_soup", (40, 32, 22), (210, 150, 70), "flask"),
    ("meat_stew", (45, 25, 20), (220, 100, 60), "flask")
]

def create_food_items():
    for (name, bg, acc, shape) in FOODS:
        img, d = make_base(bg + (240,), acc + (255,))
        if shape == "apple":
            d.ellipse((38, 44, 90, 96), fill=acc + (255,), outline=(acc[0]-40, max(0, acc[1]-40), max(0, acc[2]-40), 255), width=3)
            d.arc((58, 26, 76, 50), 45, 180, fill=(110, 70, 30, 255), width=4) # stem
            d.polygon([(68, 32), (84, 26), (76, 40)], fill=(70, 180, 80, 255)) # leaf
        elif shape == "meat":
            d.ellipse((36, 46, 92, 92), fill=acc + (255,), outline=(245, 225, 215, 255), width=3)
            d.ellipse((76, 56, 96, 76), fill=(240, 240, 240, 255)) # bone cut
        elif shape == "mushroom":
            d.arc((36, 32, 92, 78), 180, 0, fill=acc + (255,), width=14)
            d.rectangle((56, 68, 72, 96), fill=(230, 220, 205, 255), outline=(170, 160, 145, 255), width=2)
            d.ellipse((48, 42, 54, 48), fill=(255, 255, 255, 255))
            d.ellipse((74, 42, 80, 48), fill=(255, 255, 255, 255))
        elif shape == "wheat":
            d.line([(64, 96), (64, 36)], fill=acc + (255,), width=4)
            for y in [42, 54, 66, 78]:
                d.polygon([(64, y), (50, y-8), (64, y-12)], fill=acc + (255,))
                d.polygon([(64, y), (78, y-8), (64, y-12)], fill=acc + (255,))
        elif shape == "flask":
            d.rectangle((56, 32, 72, 46), fill=(180, 190, 210, 255), outline=(220, 230, 250, 255), width=2)
            d.polygon([(60, 46), (36, 92), (92, 92), (68, 46)], fill=acc + (255,), outline=(220, 230, 250, 255), width=2)
        elif shape == "fish":
            d.ellipse((34, 52, 94, 76), fill=acc + (255,), outline=(200, 220, 240, 255), width=2)
            d.polygon([(88, 64), (104, 50), (104, 78)], fill=acc + (255,))
            d.ellipse((44, 60, 48, 64), fill=(20, 20, 25, 255))
        elif shape == "berry":
            for (bx, by) in [(52, 56), (72, 56), (62, 74)]:
                d.ellipse((bx-12, by-12, bx+12, by+12), fill=acc + (255,), outline=(255, 180, 210, 255), width=2)
            d.line([(62, 56), (62, 38)], fill=(90, 140, 50, 255), width=3)
        else: # dust / moss
            d.polygon([(38, 86), (52, 48), (76, 44), (92, 86)], fill=acc + (255,), outline=(acc[0]-30, max(0, acc[1]-30), max(0, acc[2]-30), 255), width=3)
        save_icon(img, name)

if __name__ == "__main__":
    create_weapons()
    create_materials()
    create_special_items()
    create_food_items()
    print("All item icons generated successfully!")
