import os
from PIL import Image, ImageDraw

os.makedirs("assets/icons", exist_ok=True)

def create_base(bg_color, border_color):
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([4, 4, 123, 123], radius=16, fill=bg_color, outline=border_color, width=3)
    d.rounded_rectangle([8, 8, 119, 119], radius=12, fill=None, outline=(border_color[0], border_color[1], border_color[2], 60), width=1)
    return img, d

def draw_star(d, cx, cy, r_outer, r_inner, fill, outline=None):
    import math
    pts = []
    for i in range(10):
        r = r_outer if i % 2 == 0 else r_inner
        angle = i * math.pi / 5 - math.pi / 2
        pts.append((cx + r * math.cos(angle), cy + r * math.sin(angle)))
    d.polygon(pts, fill=fill, outline=outline)

items = {
    "frozen_cream": ((25, 40, 60, 240), (130, 210, 255), "ice_cup"),
    "ether_spice": ((45, 20, 50, 240), (220, 100, 255), "spice_bowl"),
    "neon_extract": ((15, 35, 45, 240), (60, 255, 220), "flask_glow"),
    "chem_concentrate": ((20, 45, 25, 240), (80, 255, 120), "vial_green"),
    "food_powder": ((45, 40, 30, 240), (220, 190, 120), "sack"),
    "predator_nectar": ((50, 20, 20, 240), (255, 100, 80), "nectar_jar"),
    "vitamin_powder": ((45, 40, 20, 240), (240, 220, 80), "powder_can"),
    "crystal_juice": ((20, 30, 55, 240), (100, 180, 255), "flask_blue"),
    "missing": ((40, 40, 45, 240), (150, 150, 160), "question"),
    "ice_apple": ((20, 35, 60, 240), (120, 220, 255), "ice_apple"),
    "sky_nectar": ((25, 40, 65, 240), (160, 220, 255), "honey_dipper"),
    "saline": ((20, 30, 45, 240), (120, 170, 220), "saline_bag"),
    "lightbird_breast": ((50, 45, 25, 240), (255, 220, 120), "roast_meat"),
    "sacred_vine_leaf": ((20, 45, 30, 240), (100, 240, 140), "leaf"),
    "chem_sweetener": ((35, 25, 50, 240), (200, 130, 240), "sugar_cubes"),
    "taste_matrix": ((15, 35, 40, 240), (70, 220, 200), "chip"),
    "synth_base": ((25, 35, 45, 240), (140, 180, 220), "brick"),
    "live_grub": ((40, 35, 25, 240), (200, 170, 90), "caterpillar"),
    "antitox_root": ((40, 30, 20, 240), (180, 130, 70), "root"),
    "magic_ice_crumb": ((20, 45, 65, 240), (140, 230, 255), "crystals"),
    "lizard_meat": ((45, 25, 25, 240), (230, 90, 80), "meat_steak"),
    "protein_mass": ((45, 30, 35, 240), (210, 120, 140), "can"),
    "plastic_protein": ((30, 35, 45, 240), (120, 160, 210), "ration_pack"),
    "golden_wheat": ((50, 42, 20, 240), (255, 215, 60), "wheat_sheaf"),
    "synth_water": ((15, 35, 55, 240), (80, 190, 255), "water_bottle"),
    "giant_fruit": ((55, 25, 35, 240), (255, 110, 130), "big_fruit"),
    "holy_water": ((30, 45, 65, 240), (240, 230, 120), "holy_vial"),
    "blessed_seed": ((45, 45, 25, 240), (255, 235, 110), "golden_seed"),
    "infected_mycelium": ((35, 20, 45, 240), (190, 70, 210), "mycelium"),
    "frost_truffle": ((25, 35, 55, 240), (130, 190, 230), "truffle"),
    "snow_wolf_meat": ((40, 25, 35, 240), (210, 100, 130), "chop_meat")
}

for name, (bg, border, shape) in items.items():
    img, d = create_base(bg, border)
    cx, cy = 64, 64

    if shape == "ice_cup":
        d.polygon([(40, 50), (88, 50), (76, 95), (52, 95)], fill=(120, 200, 255, 220), outline=(200, 240, 255))
        d.ellipse([46, 38, 82, 54], fill=(220, 245, 255), outline=(255, 255, 255))
        d.line([(55, 35), (45, 25)], fill=(255, 255, 255), width=3)
    elif shape == "spice_bowl":
        d.arc([36, 50, 92, 90], 0, 180, fill=border, width=4)
        d.ellipse([36, 45, 92, 60], fill=(160, 40, 180), outline=border, width=2)
        d.ellipse([46, 48, 82, 57], fill=(220, 90, 255))
    elif shape == "flask_glow":
        d.rectangle([58, 30, 70, 50], fill=(160, 240, 255), outline=border)
        d.polygon([(64, 50), (36, 95), (92, 95)], fill=(40, 220, 180, 210), outline=border, width=2)
        d.ellipse([46, 80, 82, 92], fill=(120, 255, 220))
    elif shape == "vial_green":
        d.rounded_rectangle([52, 35, 76, 95], radius=10, fill=(60, 200, 90, 220), outline=border, width=2)
        d.rectangle([58, 25, 70, 35], fill=(180, 240, 190), outline=border)
        d.line([(56, 55), (72, 55)], fill=(200, 255, 210), width=2)
    elif shape == "sack":
        d.polygon([(40, 95), (88, 95), (82, 55), (64, 45), (46, 55)], fill=(180, 140, 90), outline=border, width=2)
        d.ellipse([54, 42, 74, 50], fill=(210, 170, 110), outline=border)
        d.line([(52, 48), (76, 48)], fill=(240, 220, 160), width=2)
    elif shape == "nectar_jar":
        d.rounded_rectangle([42, 45, 86, 95], radius=12, fill=(220, 80, 50, 220), outline=border, width=3)
        d.rectangle([50, 35, 78, 45], fill=(240, 160, 100), outline=border, width=2)
        d.ellipse([54, 60, 74, 80], fill=(255, 140, 80))
    elif shape == "powder_can":
        d.rounded_rectangle([45, 40, 83, 95], radius=8, fill=(200, 180, 80), outline=border, width=3)
        d.ellipse([45, 35, 83, 45], fill=(240, 220, 110), outline=border, width=2)
        draw_star(d, 64, 68, 12, 6, (255, 255, 220))
    elif shape == "flask_blue":
        d.rectangle([58, 30, 70, 48], fill=(180, 210, 240), outline=border)
        d.polygon([(64, 48), (38, 95), (90, 95)], fill=(60, 140, 230, 220), outline=border, width=2)
        d.ellipse([50, 75, 78, 90], fill=(140, 210, 255))
    elif shape == "question":
        d.ellipse([48, 32, 80, 64], fill=None, outline=border, width=6)
        d.polygon([(60, 60), (68, 60), (66, 78), (62, 78)], fill=border)
        d.ellipse([60, 84, 68, 92], fill=border)
    elif shape == "ice_apple":
        d.ellipse([42, 46, 86, 95], fill=(100, 200, 255, 220), outline=border, width=3)
        d.line([(64, 46), (68, 32)], fill=(160, 230, 255), width=3)
        d.polygon([(68, 32), (80, 36), (72, 44)], fill=(130, 240, 255))
    elif shape == "honey_dipper":
        d.line([(40, 95), (78, 42)], fill=(200, 160, 90), width=6)
        d.ellipse([70, 32, 94, 56], fill=(255, 200, 60), outline=border, width=2)
        d.ellipse([74, 38, 90, 50], fill=(255, 230, 110))
    elif shape == "saline_bag":
        d.rounded_rectangle([45, 36, 83, 90], radius=8, fill=(180, 215, 245, 200), outline=border, width=2)
        d.line([(64, 90), (64, 102)], fill=(150, 200, 240), width=3)
        d.line([(55, 58), (73, 58)], fill=(255, 100, 100), width=4)
        d.line([(64, 49), (64, 67)], fill=(255, 100, 100), width=4)
    elif shape == "roast_meat":
        d.ellipse([40, 50, 88, 88], fill=(210, 150, 80), outline=border, width=3)
        d.line([(80, 60), (100, 45)], fill=(240, 230, 210), width=6)
        d.ellipse([96, 40, 106, 50], fill=(240, 230, 210))
    elif shape == "leaf":
        d.polygon([(64, 30), (92, 60), (64, 98), (36, 60)], fill=(70, 210, 110), outline=border, width=3)
        d.line([(64, 32), (64, 96)], fill=(180, 255, 190), width=2)
        d.line([(64, 55), (80, 50)], fill=(180, 255, 190), width=2)
        d.line([(64, 70), (48, 65)], fill=(180, 255, 190), width=2)
    elif shape == "sugar_cubes":
        d.polygon([(36, 68), (56, 52), (76, 68), (56, 84)], fill=(230, 190, 255), outline=border, width=2)
        d.polygon([(60, 52), (80, 36), (100, 52), (80, 68)], fill=(245, 215, 255), outline=border, width=2)
    elif shape == "chip":
        d.rectangle([44, 44, 84, 84], fill=(30, 60, 70), outline=border, width=3)
        for offset in [-12, 0, 12]:
            d.line([(64 + offset, 36), (64 + offset, 44)], fill=(180, 220, 240), width=2)
            d.line([(64 + offset, 84), (64 + offset, 92)], fill=(180, 220, 240), width=2)
            d.line([(36, 64 + offset), (44, 64 + offset)], fill=(180, 220, 240), width=2)
            d.line([(84, 64 + offset), (92, 64 + offset)], fill=(180, 220, 240), width=2)
        draw_star(d, 64, 64, 10, 5, (80, 255, 220))
    elif shape == "brick":
        d.polygon([(36, 60), (74, 44), (96, 54), (58, 70)], fill=(140, 170, 200), outline=border, width=2)
        d.polygon([(36, 60), (58, 70), (58, 88), (36, 78)], fill=(100, 130, 160), outline=border, width=2)
        d.polygon([(58, 70), (96, 54), (96, 72), (58, 88)], fill=(120, 150, 180), outline=border, width=2)
    elif shape == "caterpillar":
        for idx, (ox, oy, rad) in enumerate([(42, 70, 10), (54, 64, 11), (68, 60, 12), (82, 64, 11), (92, 70, 9)]):
            d.ellipse([ox - rad, oy - rad, ox + rad, oy + rad], fill=(180 + idx*10, 150 - idx*8, 60), outline=border, width=2)
    elif shape == "root":
        d.polygon([(64, 36), (72, 50), (84, 76), (76, 95), (64, 88), (52, 95), (44, 76), (56, 50)], fill=(150, 100, 50), outline=border, width=3)
        d.line([(64, 45), (78, 65)], fill=(190, 140, 90), width=2)
        d.line([(64, 60), (50, 78)], fill=(190, 140, 90), width=2)
    elif shape == "crystals":
        d.polygon([(64, 30), (74, 55), (64, 85), (54, 55)], fill=(160, 240, 255), outline=border, width=2)
        d.polygon([(50, 50), (58, 70), (50, 92), (42, 70)], fill=(120, 210, 255), outline=border, width=2)
        d.polygon([(78, 48), (86, 68), (78, 90), (70, 68)], fill=(140, 225, 255), outline=border, width=2)
    elif shape == "meat_steak":
        d.polygon([(40, 52), (76, 44), (94, 62), (86, 86), (48, 84)], fill=(190, 60, 60), outline=border, width=3)
        d.ellipse([58, 58, 72, 72], fill=(245, 235, 230), outline=(220, 200, 190), width=2)
    elif shape == "can":
        d.rounded_rectangle([46, 44, 82, 90], radius=8, fill=(180, 100, 110), outline=border, width=2)
        d.ellipse([46, 38, 82, 48], fill=(220, 140, 150), outline=border, width=2)
        d.line([(52, 67), (76, 67)], fill=(240, 200, 210), width=3)
    elif shape == "ration_pack":
        d.rounded_rectangle([42, 40, 86, 92], radius=6, fill=(110, 140, 180), outline=border, width=3)
        d.line([(42, 54), (86, 54)], fill=(160, 190, 230), width=2)
        d.rectangle([54, 62, 74, 82], fill=(80, 110, 150))
    elif shape == "wheat_sheaf":
        d.line([(64, 95), (64, 45)], fill=(210, 170, 50), width=4)
        for y in range(45, 80, 8):
            d.ellipse([52, y - 4, 64, y + 4], fill=(255, 215, 60), outline=border)
            d.ellipse([64, y - 4, 76, y + 4], fill=(255, 215, 60), outline=border)
    elif shape == "water_bottle":
        d.rectangle([58, 30, 70, 42], fill=(150, 220, 255), outline=border)
        d.rounded_rectangle([48, 42, 80, 94], radius=10, fill=(70, 170, 240, 210), outline=border, width=2)
        d.line([(54, 65), (74, 65)], fill=(180, 240, 255), width=2)
    elif shape == "big_fruit":
        d.ellipse([42, 44, 86, 92], fill=(230, 70, 90), outline=border, width=3)
        d.polygon([(64, 44), (72, 32), (80, 36)], fill=(80, 200, 90))
        d.ellipse([70, 54, 80, 68], fill=(255, 140, 160))
    elif shape == "holy_vial":
        d.ellipse([46, 52, 82, 92], fill=(120, 210, 255, 220), outline=border, width=3)
        d.rectangle([58, 36, 70, 52], fill=(240, 220, 110), outline=border)
        draw_star(d, 64, 72, 9, 4, (255, 245, 140))
    elif shape == "golden_seed":
        d.polygon([(64, 32), (82, 60), (74, 88), (54, 88), (46, 60)], fill=(255, 220, 60), outline=border, width=3)
        draw_star(d, 64, 60, 8, 4, (255, 255, 220))
    elif shape == "mycelium":
        d.ellipse([48, 62, 80, 86], fill=(160, 50, 180), outline=border, width=2)
        for ang in [(34, 48), (64, 40), (94, 48)]:
            d.line([(64, 70), ang], fill=(210, 100, 230), width=3)
            d.ellipse([ang[0]-5, ang[1]-5, ang[0]+5, ang[1]+5], fill=(240, 150, 255))
    elif shape == "truffle":
        d.polygon([(44, 60), (60, 48), (84, 54), (88, 76), (68, 88), (46, 80)], fill=(110, 130, 160), outline=border, width=3)
        d.line([(52, 64), (72, 70)], fill=(170, 200, 230), width=2)
    elif shape == "chop_meat":
        d.ellipse([44, 52, 84, 84], fill=(180, 60, 80), outline=border, width=3)
        d.line([(76, 60), (98, 46)], fill=(235, 225, 215), width=6)
        d.ellipse([94, 42, 104, 50], fill=(235, 225, 215))

    img.save(f"assets/icons/{name}.png")

print(f"Generated {len(items)} icons!")
