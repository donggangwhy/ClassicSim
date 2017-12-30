
#include "Mage.h"

std::string Mage::get_name(void) const {
    return "Mage";
}

int Mage::get_strength_modifier(void) const {
    return 0;
}

int Mage::get_agility_modifier(void) const {
    return 0;
}

int Mage::get_stamina_modifier(void) const {
    return 0;
}

int Mage::get_intellect_modifier(void) const {
    return 3;
}

int Mage::get_spirit_modifier(void) const {
    return 2;
}

void Mage::rotation() {
    return;
}

void Mage::auto_attack() {
    return;
}

float Mage::global_cooldown() const {
    return 1.5;
}