#include <iostream>
#include <cassert>
#include "../include/Database.hpp"
#include "../include/PlayerData.hpp"

int main() {
    std::cout << "Testing GameDatabase initialization..." << std::endl;
    auto& db = GameDatabase::getInstance();
    db.init();

    // Verify 4 artifact sets are registered
    assert(db.artifactSets.find("set_guardian") != db.artifactSets.end());
    assert(db.artifactSets.find("set_ash") != db.artifactSets.end());
    assert(db.artifactSets.find("set_berserker") != db.artifactSets.end());
    assert(db.artifactSets.find("set_rift") != db.artifactSets.end());

    std::cout << "Verified 4 artifact sets registered." << std::endl;

    // Verify pieces
    assert(db.artifacts.find("guardian_helm") != db.artifacts.end());
    assert(db.artifacts["guardian_helm"]->getArtifactType() == ArtifactType::Headgear);
    assert(db.artifacts.find("guardian_armor") != db.artifacts.end());
    assert(db.artifacts["guardian_armor"]->getArtifactType() == ArtifactType::Costume);
    assert(db.artifacts.find("guardian_amulet") != db.artifacts.end());
    assert(db.artifacts["guardian_amulet"]->getArtifactType() == ArtifactType::Amulet);
    assert(db.artifacts.find("guardian_ring") != db.artifacts.end());
    assert(db.artifacts["guardian_ring"]->getArtifactType() == ArtifactType::Ring);

    std::cout << "Verified artifact piece types." << std::endl;

    PlayerInventory inventory;
    inventory.addItem("rusty_blade", 1);
    inventory.addItem("guardian_helm", 1);
    inventory.addItem("guardian_armor", 1);
    inventory.addItem("guardian_amulet", 1);
    inventory.addItem("guardian_ring", 1);
    inventory.addItem("ash_hood", 1);

    PlayerCharacter hero("adeir");
    StatArray baseStats = hero.getTotalStats();

    // Test 1: Equip weapon
    bool eqW = hero.equipWeapon("rusty_blade", inventory);
    assert(eqW == true);
    assert(inventory.getItemCount("rusty_blade") == 0);
    assert(hero.getEquippedWeapon() == "rusty_blade");
    assert(hero.getTotalStats()[StatIndex::Attack] > baseStats[StatIndex::Attack]);
    std::cout << "Test 1: Weapon equipped successfully. Attack increased from "
              << baseStats[StatIndex::Attack] << " to " << hero.getTotalStats()[StatIndex::Attack] << std::endl;

    // Test 2: Strict slot validation - trying to equip amulet to weapon slot or headgear slot
    bool fail1 = hero.equipArtifact(ArtifactType::Headgear, "guardian_amulet", inventory);
    assert(fail1 == false);
    bool fail2 = hero.equipArtifact(ArtifactType::Ring, "guardian_armor", inventory);
    assert(fail2 == false);
    std::cout << "Test 2: Strict cross-slot validation passed! Cannot equip wrong artifact types." << std::endl;

    // Test 3: Equip 1 piece of Guardian Set (no set bonus yet)
    bool eqH = hero.equipArtifact(ArtifactType::Headgear, "guardian_helm", inventory);
    assert(eqH == true);
    assert(hero.getActiveSetCounts()["set_guardian"] == 1);

    // Test 4: Equip 2nd piece of Guardian Set (triggers 2-piece set bonus: +60 HP, +15 Def)
    StatArray stats1Piece = hero.getTotalStats();
    bool eqC = hero.equipArtifact(ArtifactType::Costume, "guardian_armor", inventory);
    assert(eqC == true);
    assert(hero.getActiveSetCounts()["set_guardian"] == 2);
    StatArray stats2Piece = hero.getTotalStats();
    // 2-piece bonus (+60 HP) + armor bonus (+80 HP) = +140 HP difference
    assert(stats2Piece[StatIndex::HP] - stats1Piece[StatIndex::HP] >= 140.0);
    std::cout << "Test 3 & 4: 2-Piece set bonus triggered! HP: " << stats2Piece[StatIndex::HP] << std::endl;

    // Test 5: Equip 3rd and 4th pieces (triggers 4-piece set bonus: +150 HP, +35 Def)
    bool eqA = hero.equipArtifact(ArtifactType::Amulet, "guardian_amulet", inventory);
    bool eqR = hero.equipArtifact(ArtifactType::Ring, "guardian_ring", inventory);
    assert(eqA && eqR);
    assert(hero.getActiveSetCounts()["set_guardian"] == 4);
    StatArray stats4Piece = hero.getTotalStats();
    std::cout << "Test 5: Full 4-Piece set bonus triggered! Total HP: " << stats4Piece[StatIndex::HP]
              << ", Defense: " << stats4Piece[StatIndex::Defense] << std::endl;

    // Test 6: Unequipping costume breaks 4-piece bonus, restores item to inventory
    bool unC = hero.unequipArtifact(ArtifactType::Costume, inventory);
    assert(unC == true);
    assert(inventory.getItemCount("guardian_armor") == 1);
    assert(hero.getActiveSetCounts()["set_guardian"] == 3);
    StatArray stats3Piece = hero.getTotalStats();
    assert(stats3Piece[StatIndex::HP] < stats4Piece[StatIndex::HP]);
    std::cout << "Test 6: Unequip succeeded, 4-piece bonus deactivated, item returned to inventory." << std::endl;

    std::cout << "\nALL LOGIC TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
