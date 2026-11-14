import os
import math
from PIL import Image, ImageDraw

ICONS_DIR = r"assets\icons"
os.makedirs(ICONS_DIR, exist_ok=True)

def create_slot_headgear():
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # Background circle
    d.ellipse((8, 8, 120, 120), fill=(28, 32, 45, 230), outline=(130, 150, 190, 240), width=4)
    # Helmet visor & shape
    d.arc((34, 28, 94, 88), start=180, end=0, fill=(210, 220, 240, 255), width=6)
    d.rectangle((34, 58, 94, 94), fill=(70, 80, 105, 255), outline=(180, 195, 225, 255), width=4)
    # Visor T-slit
    d.rectangle((44, 70, 84, 76), fill=(25, 30, 40, 255))
    d.rectangle((61, 70, 67, 90), fill=(25, 30, 40, 255))
    # Crest
    d.polygon([(64, 18), (56, 34), (72, 34)], fill=(230, 190, 70, 255))
    img.save(os.path.join(ICONS_DIR, "slot_headgear.png"))

def create_slot_costume():
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((8, 8, 120, 120), fill=(28, 32, 45, 230), outline=(130, 150, 190, 240), width=4)
    # Breastplate / Robe armor silhouette
    d.polygon([(40, 36), (64, 48), (88, 36), (98, 70), (84, 104), (44, 104), (30, 70)],
              fill=(75, 85, 115, 255), outline=(190, 205, 235, 255), width=4)
    # Center emblem
    d.polygon([(64, 58), (76, 72), (64, 86), (52, 72)], fill=(225, 180, 60, 255))
    img.save(os.path.join(ICONS_DIR, "slot_costume.png"))

def create_slot_amulet():
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((8, 8, 120, 120), fill=(28, 32, 45, 230), outline=(130, 150, 190, 240), width=4)
    # Chain
    d.arc((36, 26, 92, 78), start=0, end=180, fill=(220, 190, 80, 255), width=4)
    # Gemstone pendant
    d.polygon([(64, 62), (84, 82), (64, 106), (44, 82)], fill=(60, 160, 230, 255), outline=(240, 220, 110, 255), width=3)
    d.polygon([(64, 70), (76, 82), (64, 96), (52, 82)], fill=(140, 220, 255, 255))
    img.save(os.path.join(ICONS_DIR, "slot_amulet.png"))

def create_slot_ring():
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((8, 8, 120, 120), fill=(28, 32, 45, 230), outline=(130, 150, 190, 240), width=4)
    # Ring band
    d.ellipse((38, 48, 90, 100), outline=(225, 185, 65, 255), width=7)
    # Ring jewel at top
    d.polygon([(64, 30), (76, 42), (64, 54), (52, 42)], fill=(230, 60, 90, 255), outline=(255, 240, 140, 255), width=2)
    img.save(os.path.join(ICONS_DIR, "slot_ring.png"))

def create_slot_weapon():
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((8, 8, 120, 120), fill=(28, 32, 45, 230), outline=(130, 150, 190, 240), width=4)
    # Diagonal sword
    d.line([(32, 96), (92, 36)], fill=(220, 230, 250, 255), width=6)
    d.line([(26, 90), (38, 102)], fill=(190, 140, 50, 255), width=6)
    d.ellipse((22, 98, 30, 106), fill=(220, 170, 60, 255))
    d.polygon([(92, 36), (98, 30), (96, 40)], fill=(240, 245, 255, 255))
    img.save(os.path.join(ICONS_DIR, "slot_weapon.png"))

def create_combat_icons():
    # 1. Normal combat icon: Crossed Iron Swords on Crimson Shield
    img_c = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_c)
    # Shield shape
    d.polygon([(24, 20), (104, 20), (104, 70), (64, 112), (24, 70)],
              fill=(140, 35, 35, 240), outline=(220, 90, 90, 255), width=4)
    # Crossed swords
    d.line([(36, 32), (92, 88)], fill=(230, 235, 245, 255), width=5)
    d.line([(92, 32), (36, 88)], fill=(230, 235, 245, 255), width=5)
    d.line([(32, 84), (40, 92)], fill=(200, 160, 60, 255), width=5)
    d.line([(96, 84), (88, 92)], fill=(200, 160, 60, 255), width=5)
    img_c.save(os.path.join(ICONS_DIR, "combat.png"))

    # 2. Elite combat icon: Royal Purple / Gold Crowned Skull with Crossed Golden Blades
    img_e = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_e)
    # Shield / Crest shape (hexagon / ornate crest)
    d.polygon([(64, 10), (114, 34), (114, 88), (64, 120), (14, 88), (14, 34)],
              fill=(90, 25, 110, 245), outline=(255, 215, 60, 255), width=5)
    # Golden glow outline
    d.polygon([(64, 18), (106, 38), (106, 84), (64, 112), (22, 84), (22, 38)],
              outline=(255, 160, 30, 180), width=2)
    # Crown at top
    d.polygon([(44, 42), (48, 28), (56, 36), (64, 24), (72, 36), (80, 28), (84, 42)],
              fill=(255, 225, 70, 255), outline=(200, 140, 20, 255), width=2)
    # Crossed Golden Blades
    d.line([(34, 48), (94, 98)], fill=(255, 235, 120, 255), width=4)
    d.line([(94, 48), (34, 98)], fill=(255, 235, 120, 255), width=4)
    # Elite purple gemstone in center
    d.polygon([(64, 56), (76, 70), (64, 84), (52, 70)],
              fill=(210, 50, 230, 255), outline=(255, 240, 150, 255), width=2)
    img_e.save(os.path.join(ICONS_DIR, "combat_elite.png"))

    # 3. Boss combat icon: Demonic Fiery Skull with Horns & Crown
    img_b = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img_b)
    # Menacing crimson ring / spiked crest
    d.ellipse((10, 10, 118, 118), fill=(50, 10, 12, 245), outline=(255, 45, 45, 255), width=5)
    # Fiery inner ring
    d.ellipse((18, 18, 110, 110), outline=(255, 130, 20, 220), width=3)
    # Horns
    d.polygon([(36, 44), (18, 22), (42, 32)], fill=(220, 50, 40, 255), outline=(255, 200, 70, 255), width=2)
    d.polygon([(92, 44), (110, 22), (86, 32)], fill=(220, 50, 40, 255), outline=(255, 200, 70, 255), width=2)
    # Skull Head
    d.ellipse((38, 38, 90, 84), fill=(235, 230, 220, 255), outline=(130, 25, 25, 255), width=3)
    # Jaw
    d.rectangle((48, 76, 80, 96), fill=(235, 230, 220, 255), outline=(130, 25, 25, 255), width=2)
    # Fiery Glowing Eyes
    d.ellipse((46, 52, 58, 66), fill=(255, 20, 20, 255))
    d.ellipse((70, 52, 82, 66), fill=(255, 20, 20, 255))
    d.ellipse((50, 56, 54, 62), fill=(255, 230, 100, 255))
    d.ellipse((74, 56, 78, 62), fill=(255, 230, 100, 255))
    # Nose & Teeth
    d.polygon([(64, 68), (61, 74), (67, 74)], fill=(40, 20, 20, 255))
    for t in [52, 60, 68, 76]:
        d.line([(t, 82), (t, 94)], fill=(50, 30, 30, 255), width=2)
    img_b.save(os.path.join(ICONS_DIR, "combat_boss.png"))

def create_artifact_piece_icons():
    # 1. Guardian Set
    # Helm
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(20, 30, 45, 240), outline=(100, 160, 240, 255), width=4)
    d.arc((36, 32, 92, 88), 180, 0, fill=(200, 225, 255, 255), width=6)
    d.rectangle((36, 60, 92, 92), fill=(60, 90, 130, 255), outline=(170, 210, 255, 255), width=4)
    d.rectangle((46, 72, 82, 78), fill=(20, 40, 65, 255))
    d.polygon([(64, 20), (56, 36), (72, 36)], fill=(120, 200, 255, 255))
    img.save(os.path.join(ICONS_DIR, "guardian_helm.png"))

    # Armor
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(20, 30, 45, 240), outline=(100, 160, 240, 255), width=4)
    d.polygon([(40, 36), (64, 46), (88, 36), (98, 72), (84, 102), (44, 102), (30, 72)],
              fill=(55, 85, 125, 255), outline=(170, 210, 255, 255), width=4)
    d.polygon([(64, 56), (76, 72), (64, 88), (52, 72)], fill=(110, 190, 255, 255))
    img.save(os.path.join(ICONS_DIR, "guardian_armor.png"))

    # Amulet
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(20, 30, 45, 240), outline=(100, 160, 240, 255), width=4)
    d.arc((36, 26, 92, 78), 0, 180, fill=(180, 210, 250, 255), width=4)
    d.polygon([(64, 62), (84, 82), (64, 104), (44, 82)], fill=(60, 140, 240, 255), outline=(210, 235, 255, 255), width=3)
    img.save(os.path.join(ICONS_DIR, "guardian_amulet.png"))

    # Ring
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(20, 30, 45, 240), outline=(100, 160, 240, 255), width=4)
    d.ellipse((38, 48, 90, 100), outline=(150, 200, 255, 255), width=7)
    d.polygon([(64, 32), (76, 44), (64, 54), (52, 44)], fill=(90, 170, 255, 255), outline=(230, 245, 255, 255), width=2)
    img.save(os.path.join(ICONS_DIR, "guardian_ring.png"))

    # 2. Ash Set (Fire / Orange-Red theme)
    # Hood
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(40, 18, 16, 240), outline=(240, 110, 40, 255), width=4)
    d.polygon([(64, 24), (32, 90), (52, 90), (64, 76), (76, 90), (96, 90)],
              fill=(160, 45, 30, 255), outline=(255, 160, 70, 255), width=4)
    d.polygon([(64, 48), (56, 68), (72, 68)], fill=(255, 210, 60, 255))
    img.save(os.path.join(ICONS_DIR, "ash_hood.png"))

    # Robe
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(40, 18, 16, 240), outline=(240, 110, 40, 255), width=4)
    d.polygon([(42, 34), (64, 46), (86, 34), (96, 102), (32, 102)],
              fill=(145, 40, 25, 255), outline=(255, 140, 50, 255), width=4)
    d.line([(64, 48), (64, 102)], fill=(255, 200, 70, 255), width=4)
    img.save(os.path.join(ICONS_DIR, "ash_robe.png"))

    # Amulet
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(40, 18, 16, 240), outline=(240, 110, 40, 255), width=4)
    d.arc((36, 26, 92, 78), 0, 180, fill=(240, 160, 60, 255), width=4)
    d.polygon([(64, 60), (84, 82), (64, 106), (44, 82)], fill=(230, 70, 30, 255), outline=(255, 220, 100, 255), width=3)
    img.save(os.path.join(ICONS_DIR, "ash_amulet.png"))

    # Ring
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(40, 18, 16, 240), outline=(240, 110, 40, 255), width=4)
    d.ellipse((38, 48, 90, 100), outline=(240, 140, 50, 255), width=7)
    d.polygon([(64, 30), (76, 42), (64, 54), (52, 42)], fill=(245, 75, 30, 255), outline=(255, 225, 90, 255), width=2)
    img.save(os.path.join(ICONS_DIR, "ash_ring.png"))

    # 3. Berserker Set (Blood / Crimson theme)
    # Mask
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(38, 12, 16, 240), outline=(220, 40, 60, 255), width=4)
    d.polygon([(36, 38), (64, 28), (92, 38), (86, 86), (64, 102), (42, 86)],
              fill=(130, 25, 35, 255), outline=(240, 80, 100, 255), width=4)
    d.line([(48, 54), (58, 62)], fill=(255, 230, 100, 255), width=4)
    d.line([(80, 54), (70, 62)], fill=(255, 230, 100, 255), width=4)
    img.save(os.path.join(ICONS_DIR, "berserker_mask.png"))

    # Harness
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(38, 12, 16, 240), outline=(220, 40, 60, 255), width=4)
    d.polygon([(38, 38), (64, 52), (90, 38), (96, 92), (32, 92)],
              fill=(100, 25, 35, 255), outline=(230, 70, 85, 255), width=4)
    d.line([(38, 38), (90, 92)], fill=(220, 150, 60, 255), width=5)
    d.line([(90, 38), (38, 92)], fill=(220, 150, 60, 255), width=5)
    img.save(os.path.join(ICONS_DIR, "berserker_harness.png"))

    # Amulet
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(38, 12, 16, 240), outline=(220, 40, 60, 255), width=4)
    d.arc((36, 26, 92, 78), 0, 180, fill=(190, 40, 50, 255), width=4)
    d.polygon([(64, 60), (84, 82), (64, 108), (44, 82)], fill=(210, 20, 45, 255), outline=(255, 120, 140, 255), width=3)
    img.save(os.path.join(ICONS_DIR, "berserker_amulet.png"))

    # Ring
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(38, 12, 16, 240), outline=(220, 40, 60, 255), width=4)
    d.ellipse((38, 48, 90, 100), outline=(190, 40, 50, 255), width=7)
    d.polygon([(64, 30), (76, 42), (64, 54), (52, 42)], fill=(220, 25, 45, 255), outline=(255, 140, 160, 255), width=2)
    img.save(os.path.join(ICONS_DIR, "berserker_ring.png"))

    # 4. Rift Shadow Set (Void / Purple-Cyan theme)
    # Cowl
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(25, 16, 38, 240), outline=(170, 70, 230, 255), width=4)
    d.polygon([(64, 22), (32, 88), (96, 88)], fill=(85, 30, 120, 255), outline=(200, 110, 255, 255), width=4)
    d.ellipse((52, 58, 62, 66), fill=(60, 240, 230, 255))
    d.ellipse((66, 58, 76, 66), fill=(60, 240, 230, 255))
    img.save(os.path.join(ICONS_DIR, "rift_cowl.png"))

    # Vestment
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(25, 16, 38, 240), outline=(170, 70, 230, 255), width=4)
    d.polygon([(40, 36), (64, 48), (88, 36), (98, 102), (30, 102)],
              fill=(75, 25, 110, 255), outline=(190, 100, 255, 255), width=4)
    d.polygon([(64, 60), (74, 74), (64, 88), (54, 74)], fill=(60, 240, 230, 255))
    img.save(os.path.join(ICONS_DIR, "rift_vestment.png"))

    # Talisman
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(25, 16, 38, 240), outline=(170, 70, 230, 255), width=4)
    d.arc((36, 26, 92, 78), 0, 180, fill=(180, 100, 240, 255), width=4)
    d.polygon([(64, 60), (84, 82), (64, 106), (44, 82)], fill=(130, 40, 190, 255), outline=(80, 240, 230, 255), width=3)
    img.save(os.path.join(ICONS_DIR, "rift_talisman.png"))

    # Ring
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((10, 10, 118, 118), fill=(25, 16, 38, 240), outline=(170, 70, 230, 255), width=4)
    d.ellipse((38, 48, 90, 100), outline=(160, 80, 230, 255), width=7)
    d.polygon([(64, 30), (76, 42), (64, 54), (52, 42)], fill=(60, 240, 230, 255), outline=(220, 150, 255, 255), width=2)
    img.save(os.path.join(ICONS_DIR, "rift_ring.png"))

if __name__ == "__main__":
    create_slot_headgear()
    create_slot_costume()
    create_slot_amulet()
    create_slot_ring()
    create_slot_weapon()
    create_combat_icons()
    create_artifact_piece_icons()
    print("All icons successfully generated!")
