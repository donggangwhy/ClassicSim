#include "TestAttackTables.h"

#include <cassert>

#include "CharacterStats.h"
#include "CombatRoll.h"
#include "Equipment.h"
#include "MagicAttackTable.h"
#include "Mechanics.h"
#include "MeleeSpecialTable.h"
#include "Orc.h"
#include "Race.h"
#include "SimSettings.h"
#include "Target.h"
#include "Warrior.h"
#include "WhiteHitTable.h"

void TestAttackTables::test_all() {
    test_basic_properties();

    test_white_hit_table();
    test_white_hit_table_update();
    test_special_hit_table();
    test_magic_attack_table();
}

void TestAttackTables::test_basic_properties() {
}

void TestAttackTables::test_white_hit_table() {
    auto* random = new Random(0, 9999);
    auto* table = new WhiteHitTable(random, 300, 0.0, 0.0, 0.0, 0.0, 0.0);
    assert(table->get_outcome(0, 0.0) == PhysicalAttackResult::HIT);
    assert(table->get_outcome(9999, 0.0) == PhysicalAttackResult::HIT);
    delete table;

    table = new WhiteHitTable(random, 300, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001);
    assert(table->get_outcome(0, 0.0) == PhysicalAttackResult::MISS);
    assert(table->get_outcome(1, 0.0) == PhysicalAttackResult::DODGE);
    assert(table->get_outcome(2, 0.0) == PhysicalAttackResult::PARRY);
    assert(table->get_outcome(3, 0.0) == PhysicalAttackResult::GLANCING);
    assert(table->get_outcome(4, 0.0) == PhysicalAttackResult::BLOCK);
    assert(table->get_outcome(5, 0.0001) == PhysicalAttackResult::CRITICAL);
    assert(table->get_outcome(6, 0.0001) == PhysicalAttackResult::HIT);
    assert(table->get_outcome(9999, 0.0) == PhysicalAttackResult::HIT);
    assert(table->get_outcome(6, 1.0) == PhysicalAttackResult::CRITICAL);
    delete random;
    delete table;
}

void TestAttackTables::test_white_hit_table_update() {
    Race* race = new Orc();
    auto* sim_settings = new SimSettings();
    auto* pchar = new Warrior(race, equipment_db, sim_settings);
    pchar->get_equipment()->set_mainhand("Frostbite");
    pchar->get_equipment()->set_offhand("Vis'kag the Bloodletter");

    WhiteHitTable* table = pchar->get_combat_roll()->get_white_hit_table(300);

    assert(table->get_outcome(0, 0.0001) == PhysicalAttackResult::MISS);
    assert(table->get_outcome(2799, 0.0001) == PhysicalAttackResult::MISS);
    assert(table->get_outcome(2800, 0.0001) == PhysicalAttackResult::DODGE);
    assert(table->get_outcome(3299, 0.0001) == PhysicalAttackResult::DODGE);
    assert(table->get_outcome(3300, 0.0001) == PhysicalAttackResult::GLANCING);
    assert(table->get_outcome(7299, 0.0001) == PhysicalAttackResult::GLANCING);
    assert(table->get_outcome(7300, 0.0001) == PhysicalAttackResult::CRITICAL);
    // Note: This will fail when changing base agility or agi needed per crit.
    assert(table->get_outcome(7984, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::CRITICAL);
    assert(table->get_outcome(7985, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::HIT);

    pchar->get_stats()->increase_crit(0.0001);
    assert(table->get_outcome(7985, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::CRITICAL);
    assert(table->get_outcome(7986, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::HIT);

    pchar->get_stats()->decrease_crit(pchar->get_stats()->get_mh_crit_chance());
    pchar->get_stats()->increase_crit(0.9999);
    assert(table->get_outcome(7986, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::CRITICAL);
    assert(table->get_outcome(9999, pchar->get_stats()->get_mh_crit_chance()) == PhysicalAttackResult::CRITICAL);

    delete sim_settings;
    delete race;
    delete pchar;
}

void TestAttackTables::test_special_hit_table() {
    auto* random = new Random(0, 9999);
    auto* table = new MeleeSpecialTable(random, 300, 0.0, 0.0, 0.0, 0.0);
    assert(table->get_outcome(0, 0.0) == PhysicalAttackResult::HIT);
    delete table;

    table = new MeleeSpecialTable(random, 300, 0.0001, 0.0001, 0.0001, 0.0001);
    assert(table->get_outcome(0, 1.0) == PhysicalAttackResult::MISS);
    assert(table->get_outcome(1, 1.0) == PhysicalAttackResult::DODGE);
    assert(table->get_outcome(2, 1.0) == PhysicalAttackResult::PARRY);
    assert(table->get_outcome(3, 1.0) == PhysicalAttackResult::BLOCK_CRITICAL);
    assert(table->get_outcome(4, 1.0) == PhysicalAttackResult::CRITICAL);
    assert(table->get_outcome(9999, 1.0) == PhysicalAttackResult::CRITICAL);
    delete table;
    delete random;
}

void TestAttackTables::test_magic_attack_table() {
    auto* random = new Random(0, 9999);
    auto* target = new Target(63);
    auto* mechanics = new Mechanics(target);
    auto* table = new MagicAttackTable(mechanics, random, 60, 0.0, target->get_resistance(MagicSchool::Frost));

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(1699, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(1700, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(62);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(599, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(600, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(61);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(499, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(500, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(60);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(399, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(400, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(59);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(299, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(300, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(58);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(199, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(200, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(57);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(99, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(100, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    target->set_lvl(56);
    table->update_miss_chance(60, 0.0);

    assert(table->get_hit_outcome(0, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(99, 0.0) == MagicAttackResult::MISS);
    assert(table->get_hit_outcome(100, 0.0) == MagicAttackResult::HIT);
    assert(table->get_hit_outcome(9999, 0.0) == MagicAttackResult::HIT);

    delete random;
    delete target;
    delete mechanics;
    delete table;
}