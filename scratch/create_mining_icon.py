import os
from PIL import Image, ImageDraw

ICONS_DIR = r"assets\icons"
img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
# Frame
d.rounded_rectangle((6, 6, 122, 122), radius=16, fill=(35, 30, 24, 245), outline=(230, 180, 60, 255), width=3)
# Crossed Pickaxe and Chisel
d.line([(32, 98), (96, 34)], fill=(150, 100, 50, 255), width=6) # wooden handle
d.arc((64, 18, 114, 68), start=180, end=330, fill=(210, 225, 245, 255), width=7) # pick head
# Glowing gem in center
d.polygon([(64, 54), (76, 68), (64, 82), (52, 68)], fill=(60, 200, 255, 255), outline=(255, 240, 120, 255), width=2)
img.save(os.path.join(ICONS_DIR, "mining.png"), format="PNG")
print("Mining icon saved!")
