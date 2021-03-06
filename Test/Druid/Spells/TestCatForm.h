#pragma once

#include "TestSpellDruid.h"

class TestCatForm : public TestSpellDruid {
public:
    TestCatForm(EquipmentDb* equipment_db);

    void test_all();

private:
    void test_name_correct() override;
    void test_spell_cooldown() override;
    void test_whether_spell_causes_global_cooldown() override;
    void test_how_spell_observes_global_cooldown() override;
    void test_resource_cost() override;
    void test_is_ready_conditions() override;
    void test_resource_cost_with_3_of_3_natural_shapeshifter();
    void test_claws_equipped_upon_entering_cat_form();
    void test_proc_enchants_on_weapon_disabled_in_cat_form();
    void test_attack_speed_in_and_out_of_cat_form();
    void test_hit_dmg();
    void test_crit_dmg();
    void test_glancing_dmg();
    void test_leader_of_the_pack_gives_crit_to_party_members();
    void test_0_of_5_furor_gives_0_energy_on_entering_cat_form();
    void test_5_of_5_furor_gives_40_energy_on_entering_cat_form();

    void when_cat_form_is_performed();
};
