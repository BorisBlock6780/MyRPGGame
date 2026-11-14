import os
import math
import random
from PIL import Image, ImageDraw, ImageFilter

BG_DIR = r"assets\bg"
os.makedirs(BG_DIR, exist_ok=True)

# Node centers roughly at 1920x1080:
# Shop: (182, 726)
# Skirmish 1: (458, 275)
# Skirmish 2: (1058, 441)
# Utility: (1473, 205)
# Elite: (712, 860)
# Boss: (1660, 720)

FLOOR_THEMES = [
    {
        "floor": 1,
        "name": "Пепельный Некрополь",
        "bg_base": (32, 28, 30),
        "parchment": (50, 42, 40),
        "accent": (140, 110, 95),
        "water_color": (35, 30, 36),
        "features": "crypts"
    },
    {
        "floor": 2,
        "name": "Оазис Тишины",
        "bg_base": (22, 34, 28),
        "parchment": (38, 52, 42),
        "accent": (75, 140, 95),
        "water_color": (25, 60, 75),
        "features": "oasis"
    },
    {
        "floor": 3,
        "name": "Затонувший Акрополь",
        "bg_base": (18, 28, 40),
        "parchment": (28, 45, 62),
        "accent": (60, 130, 170),
        "water_color": (15, 35, 55),
        "features": "sunken"
    },
    {
        "floor": 4,
        "name": "Раскаленные Магистрали",
        "bg_base": (42, 20, 18),
        "parchment": (65, 32, 26),
        "accent": (210, 85, 35),
        "water_color": (160, 45, 20), # Lava rivers
        "features": "volcanic"
    },
    {
        "floor": 5,
        "name": "Железный Карантин",
        "bg_base": (30, 32, 34),
        "parchment": (48, 50, 52),
        "accent": (160, 130, 70),
        "water_color": (35, 45, 38), # Toxic slime
        "features": "industrial"
    },
    {
        "floor": 6,
        "name": "Изумрудный Свод",
        "bg_base": (18, 36, 22),
        "parchment": (32, 58, 36),
        "accent": (65, 175, 80),
        "water_color": (20, 50, 45),
        "features": "jungle"
    },
    {
        "floor": 7,
        "name": "Застывшее Время",
        "bg_base": (20, 28, 42),
        "parchment": (32, 48, 70),
        "accent": (110, 185, 240),
        "water_color": (40, 70, 105), # Glacial ice
        "features": "frozen"
    },
    {
        "floor": 8,
        "name": "Иллюзорный Мегаполис",
        "bg_base": (25, 20, 38),
        "parchment": (42, 32, 60),
        "accent": (190, 70, 220),
        "water_color": (40, 20, 70),
        "features": "cyber"
    },
    {
        "floor": 9,
        "name": "Небесный Собор",
        "bg_base": (38, 35, 28),
        "parchment": (60, 55, 42),
        "accent": (225, 190, 80),
        "water_color": (50, 60, 80), # Clouds/ether
        "features": "celestial"
    },
    {
        "floor": 10,
        "name": "Ядро Башни / Исток",
        "bg_base": (18, 15, 28),
        "parchment": (35, 26, 48),
        "accent": (165, 85, 235),
        "water_color": (25, 10, 40), # Void rift
        "features": "void"
    }
]

def draw_compass_rose(d, cx, cy, radius=55, color=(200, 180, 120, 180)):
    # Outer ring
    d.ellipse((cx - radius, cy - radius, cx + radius, cy + radius), outline=color, width=2)
    d.ellipse((cx - radius + 8, cy - radius + 8, cx + radius - 8, cy + radius - 8), outline=color, width=1)
    # 4 cardinal points
    r2 = radius - 4
    r_in = radius * 0.35
    # North
    d.polygon([(cx, cy - r2), (cx + r_in*0.4, cy - r_in*0.4), (cx, cy), (cx - r_in*0.4, cy - r_in*0.4)], fill=color)
    # South
    d.polygon([(cx, cy + r2), (cx + r_in*0.4, cy + r_in*0.4), (cx, cy), (cx - r_in*0.4, cy + r_in*0.4)], fill=(color[0]//2, color[1]//2, color[2]//2, color[3]))
    # East
    d.polygon([(cx + r2, cy), (cx + r_in*0.4, cy + r_in*0.4), (cx, cy), (cx + r_in*0.4, cy - r_in*0.4)], fill=color)
    # West
    d.polygon([(cx - r2, cy), (cx - r_in*0.4, cy + r_in*0.4), (cx, cy), (cx - r_in*0.4, cy - r_in*0.4)], fill=(color[0]//2, color[1]//2, color[2]//2, color[3]))
    # Label N
    d.text((cx - 4, cy - radius - 18), "N", fill=color)

def draw_merchant_camp(d, x, y, accent):
    # Caravan tent & bazaar stall
    d.ellipse((x - 45, y - 35, x + 45, y + 35), fill=(accent[0]//4, accent[1]//4, accent[2]//4, 120), outline=accent, width=2)
    # Main Tent
    d.polygon([(x - 28, y + 16), (x, y - 26), (x + 28, y + 16)], fill=(accent[0]//2, accent[1]//2, accent[2]//2, 220), outline=accent, width=2)
    d.polygon([(x - 10, y + 16), (x, y - 8), (x + 10, y + 16)], fill=(20, 20, 25, 255))
    # Wagon wheels / boxes
    d.rectangle((x + 22, y - 6, x + 38, y + 12), fill=(accent[0]//3, accent[1]//3, accent[2]//3, 200), outline=accent, width=1)
    d.ellipse((x + 24, y + 8, x + 36, y + 20), outline=accent, width=2)
    # Banner / Coin
    d.line([(x - 24, y + 18), (x - 24, y - 28)], fill=accent, width=2)
    d.polygon([(x - 24, y - 28), (x - 10, y - 22), (x - 24, y - 16)], fill=(240, 200, 60, 230))

def draw_utility_hearth(d, x, y, accent):
    # Campfire / Anvil / Hearth area
    d.ellipse((x - 40, y - 30, x + 40, y + 30), fill=(accent[0]//4, accent[1]//4, accent[2]//4, 120), outline=accent, width=2)
    # Stones ring
    d.ellipse((x - 18, y - 8, x + 18, y + 14), outline=(150, 140, 130, 200), width=3)
    # Fire flames
    d.polygon([(x - 10, y + 6), (x, y - 18), (x + 10, y + 6)], fill=(255, 140, 30, 240))
    d.polygon([(x - 5, y + 6), (x, y - 12), (x + 5, y + 6)], fill=(255, 230, 80, 255))
    # Anvil / pot stand
    d.line([(x - 16, y - 14), (x + 16, y - 14)], fill=(180, 180, 180, 200), width=2)
    d.line([(x - 14, y + 8), (x - 14, y - 14)], fill=(180, 180, 180, 200), width=2)
    d.line([(x + 14, y + 8), (x + 14, y - 14)], fill=(180, 180, 180, 200), width=2)

def draw_skirmish_cavern(d, x, y, accent, label_type=1):
    # Jagged rock / barricade / nest
    d.ellipse((x - 42, y - 32, x + 42, y + 32), fill=(accent[0]//4, accent[1]//4, accent[2]//4, 110), outline=(accent[0], accent[1], accent[2], 160), width=2)
    # Cavern opening / jagged stones
    d.arc((x - 22, y - 18, x + 22, y + 16), start=180, end=0, fill=accent, width=3)
    d.polygon([(x - 20, y + 10), (x, y - 8), (x + 20, y + 10)], fill=(15, 15, 18, 240))
    # Spikes / swords marker
    d.line([(x - 14, y + 12), (x - 6, y - 14)], fill=accent, width=2)
    d.line([(x + 14, y + 12), (x + 6, y - 14)], fill=accent, width=2)

def draw_elite_sanctuary(d, x, y, accent):
    # Ornate crypt / temple pedestal / glowing rune circle
    d.ellipse((x - 55, y - 40, x + 55, y + 40), fill=(90, 30, 110, 140), outline=(230, 180, 60, 220), width=3)
    d.ellipse((x - 42, y - 28, x + 42, y + 28), outline=(190, 80, 240, 180), width=2)
    # Stone altar with obelisks
    d.polygon([(x - 28, y + 14), (x, y - 12), (x + 28, y + 14)], fill=(50, 20, 65, 230), outline=(240, 200, 70, 255), width=2)
    # Left Obelisk
    d.polygon([(x - 26, y + 14), (x - 22, y - 24), (x - 18, y + 14)], fill=(200, 160, 60, 240))
    # Right Obelisk
    d.polygon([(x + 18, y + 14), (x + 22, y - 24), (x + 26, y + 14)], fill=(200, 160, 60, 240))
    # Center floating gem
    d.polygon([(x, y - 18), (x + 7, y - 8), (x, y + 2), (x - 7, y - 8)], fill=(240, 80, 255, 255))

def draw_boss_citadel(d, x, y, accent):
    # Grand dread fortress / demonic citadel / dragon horns
    d.ellipse((x - 65, y - 48, x + 65, y + 48), fill=(60, 12, 16, 170), outline=(255, 50, 50, 240), width=4)
    d.ellipse((x - 52, y - 36, x + 52, y + 36), outline=(255, 150, 40, 200), width=2)
    # Fortress Wall
    d.polygon([(x - 42, y + 18), (x - 42, y - 14), (x - 26, y - 14), (x - 26, y - 26),
               (x, y - 36), (x + 26, y - 26), (x + 26, y - 14), (x + 42, y - 14), (x + 42, y + 18)],
              fill=(45, 15, 18, 250), outline=(255, 60, 50, 255), width=3)
    # Boss Gate
    d.arc((x - 14, y - 4, x + 14, y + 18), start=180, end=0, fill=(255, 140, 30, 255), width=3)
    d.rectangle((x - 12, y + 3, x + 12, y + 18), fill=(20, 5, 8, 255))
    # Glowing Horns / Spikes
    d.polygon([(x - 34, y - 14), (x - 44, y - 42), (x - 24, y - 26)], fill=(255, 50, 40, 255))
    d.polygon([(x + 34, y - 14), (x + 44, y - 42), (x + 24, y - 26)], fill=(255, 50, 40, 255))

def generate_floor_map(floor_data):
    W, H = 1920, 1080
    img = Image.new("RGBA", (W, H), floor_data["bg_base"] + (255,))
    d = ImageDraw.Draw(img)

    # 1. Base parchment texture & subtle terrain contours
    random.seed(floor_data["floor"] * 997 + 103)
    for _ in range(80):
        cx = random.randint(100, W - 100)
        cy = random.randint(100, H - 100)
        rx = random.randint(150, 450)
        ry = random.randint(100, 320)
        col = (floor_data["parchment"][0] + random.randint(-5, 5),
               floor_data["parchment"][1] + random.randint(-5, 5),
               floor_data["parchment"][2] + random.randint(-5, 5),
               random.randint(40, 90))
        d.ellipse((cx - rx, cy - ry, cx + rx, cy + ry), fill=col)

    # 2. Organic river / canal / path / leylines
    water_pts = [
        (80, random.randint(400, 600)),
        (350, random.randint(350, 500)),
        (650, random.randint(450, 650)),
        (1000, random.randint(300, 480)),
        (1350, random.randint(400, 620)),
        (1800, random.randint(450, 650))
    ]
    w_col = floor_data["water_color"] + (180,)
    for i in range(len(water_pts) - 1):
        p1 = water_pts[i]
        p2 = water_pts[i+1]
        # Draw broad curve
        d.line([p1, p2], fill=w_col, width=38)
    
    # 3. Topographic contour lines
    acc = floor_data["accent"]
    for i in range(5):
        cy = 180 + i * 160
        contour = []
        for x in range(60, W - 180, 80):
            y_off = int(math.sin((x + i*200) * 0.005) * 45) + random.randint(-8, 8)
            contour.append((x, cy + y_off))
        if len(contour) > 1:
            d.line(contour, fill=(acc[0], acc[1], acc[2], 45), width=2)

    # 4. Illustrated locations mapped to the node positions
    # Shop: (182, 726)
    draw_merchant_camp(d, 182, 726, acc)
    # Skirmish 1: (458, 275)
    draw_skirmish_cavern(d, 458, 275, acc, 1)
    # Skirmish 2: (1058, 441)
    draw_skirmish_cavern(d, 1058, 441, acc, 2)
    # Utility: (1473, 205)
    draw_utility_hearth(d, 1473, 205, acc)
    # Elite Camp: (712, 860)
    draw_elite_sanctuary(d, 712, 860, acc)
    # Boss Citadel: (1660, 720)
    draw_boss_citadel(d, 1660, 720, acc)

    # 5. Cartographic map border with coordinates & ornate corners
    border_col = (acc[0], acc[1], acc[2], 200)
    # Outer double line
    d.rectangle((40, 40, W - 40, H - 40), outline=border_col, width=3)
    d.rectangle((52, 52, W - 52, H - 52), outline=(border_col[0], border_col[1], border_col[2], 120), width=1)
    # Corner ornaments
    for (ox, oy) in [(40, 40), (W - 40, 40), (40, H - 40), (W - 40, H - 40)]:
        d.ellipse((ox - 14, oy - 14, ox + 14, oy + 14), fill=(border_col[0]//2, border_col[1]//2, border_col[2]//2, 220), outline=border_col, width=2)

    # Tick marks on borders
    for x in range(100, W - 100, 100):
        d.line([(x, 40), (x, 48)], fill=border_col, width=2)
        d.line([(x, H - 48), (x, H - 40)], fill=border_col, width=2)
    for y in range(100, H - 100, 100):
        d.line([(40, y), (48, y)], fill=border_col, width=2)
        d.line([(W - 48, y), (W - 40, y)], fill=border_col, width=2)

    # 6. Compass rose in top left corner
    draw_compass_rose(d, 120, 130, radius=48, color=(acc[0], acc[1], acc[2], 210))

    # 7. Floor Title Cartouche banner at bottom left
    cart_x, cart_y = 120, H - 85
    d.rectangle((cart_x, cart_y, cart_x + 360, cart_y + 36), fill=(20, 20, 25, 230), outline=border_col, width=2)
    title_str = f"КАРТА: ЭТАЖ {floor_data['floor']} — {floor_data['name'].upper()}"
    d.text((cart_x + 14, cart_y + 10), title_str, fill=(240, 220, 160, 255))

    # Apply subtle blur to soften parchment
    # Save image
    out_path = os.path.join(BG_DIR, f"floor_{floor_data['floor']}.png")
    img.save(out_path, format="PNG")
    print(f"Saved: {out_path}")

if __name__ == "__main__":
    for fl in FLOOR_THEMES:
        generate_floor_map(fl)
    print("All 10 floor maps generated successfully!")
