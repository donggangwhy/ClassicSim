#ifndef HUNTER_H
#define HUNTER_H

#include "Character.h"
#include "CombatRoll.h"

class Hunter: public Character {
    Q_OBJECT
public:
    Hunter(Race* race, Engine* engine, Equipment* _eq, CombatRoll* _roll, QObject *parent = 0) :
        Character(race, engine, _eq, _roll, parent) {}

    virtual ~Hunter() {}

    QString get_name() const override;
    QString get_class_color() const override;
    int get_strength_modifier() const override;
    int get_agility_modifier() const override;
    int get_stamina_modifier() const override;
    int get_intellect_modifier() const override;
    int get_spirit_modifier() const override;
    void rotation() override;
    void mh_auto_attack(const int) override;
    void oh_auto_attack(const int) override;
    float global_cooldown() const override;

    int get_ap_per_strength() const override;
    int get_ap_per_agi() const override;

    void reset_spells() const override;

protected:
private:
    CombatRoll* roll;

    void initialize_talents() override;
};

#endif // HUNTER_H
