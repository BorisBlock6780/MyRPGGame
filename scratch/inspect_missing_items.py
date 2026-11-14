import re

with open(r'include/Database.hpp', 'r', encoding='utf-8') as f:
    text = f.read()

missing_files = ['assets/icons/frozen_cream.png', 'assets/icons/ether_spice.png', 'assets/icons/neon_extract.png', 'assets/icons/chem_concentrate.png', 'assets/icons/food_powder.png', 'assets/icons/predator_nectar.png', 'assets/icons/vitamin_powder.png', 'assets/icons/crystal_juice.png', 'assets/icons/missing.png', 'assets/icons/ice_apple.png', 'assets/icons/sky_nectar.png', 'assets/icons/saline.png', 'assets/icons/lightbird_breast.png', 'assets/icons/sacred_vine_leaf.png', 'assets/icons/chem_sweetener.png', 'assets/icons/taste_matrix.png', 'assets/icons/synth_base.png', 'assets/icons/live_grub.png', 'assets/icons/antitox_root.png', 'assets/icons/magic_ice_crumb.png', 'assets/icons/lizard_meat.png', 'assets/icons/protein_mass.png', 'assets/icons/plastic_protein.png', 'assets/icons/golden_wheat.png', 'assets/icons/synth_water.png', 'assets/icons/giant_fruit.png', 'assets/icons/holy_water.png', 'assets/icons/blessed_seed.png', 'assets/icons/infected_mycelium.png', 'assets/icons/frost_truffle.png', 'assets/icons/snow_wolf_meat.png']

for mf in missing_files:
    m = re.search(r'registerItem\(\s*"([^"]+)",\s*"([^"]+)",\s*ItemCategory::(\w+),\s*"' + re.escape(mf) + r'"', text)
    if m:
        print(f'{mf}: id={m.group(1)}, name={m.group(2)}, cat={m.group(3)}')
    else:
        print(f'{mf}: not matched by regex')
