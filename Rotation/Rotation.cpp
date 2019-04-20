#include "Rotation.h"

#include <QDebug>
#include <utility>

#include "Character.h"
#include "CharacterSpells.h"
#include "ConditionBuff.h"
#include "ConditionResource.h"
#include "ConditionSpell.h"
#include "ConditionVariableAssign.h"
#include "ConditionVariableBuiltin.h"
#include "EnabledBuffs.h"
#include "RotationExecutor.h"
#include "Spell.h"
#include "SpellCastingTime.h"
#include "Utils/Check.h"

Rotation::Rotation(QString class_name) :
    pchar(nullptr),
    class_name(std::move(class_name)),
    attack_mode(AttackMode::MeleeAttack)
{}

Rotation::~Rotation() {
    for (auto & executor : all_executors) {
        delete executor;
    }
}

void Rotation::run_precombat_actions() {
    for (auto & spell : precombat_spells) {
        if (spell->is_available())
            spell->perform();
    }
}

void Rotation::perform_rotation() const {
    for (auto & executor : active_executors)
        executor->attempt_cast();
}

void Rotation::link_spells(Character* pchar) {
    this->pchar = pchar;

    active_executors.clear();

    for (auto & executor : all_executors) {
        Spell* spell = pchar->get_spells()->get_spell_by_name(executor->get_spell_name());

        if (spell == nullptr || !spell->is_enabled())
            continue;

        executor->set_spell(spell);

        if (this->add_conditionals(executor))
            active_executors.append(executor);
    }

    link_precast_spell();
    link_precombat_spells();
}

void Rotation::link_precast_spell() {
    SpellCastingTime* spell = dynamic_cast<SpellCastingTime*>(pchar->get_spells()->get_spell_by_name(precast_spell_name));

    if (spell == nullptr)
        return;

    precast_spell = spell;
}

void Rotation::link_precombat_spells() {
    precombat_spells.clear();

    for (auto & spell_name : precombat_spell_names) {
        Spell* spell = pchar->get_spells()->get_spell_by_name(spell_name);

        if (spell == nullptr || !spell->is_enabled())
            continue;

        precombat_spells.append(spell);
    }
}

bool Rotation::add_conditionals(RotationExecutor * executor) {
    for (auto & group : executor->condition_groups)
        for (auto & condition : group)
            delete condition;
    executor->condition_groups.clear();

    QVector<Condition*> condition_group_to_add;

    for (auto & sentence : executor->sentences) {
        Condition* condition = nullptr;

        if (sentence->logical_connective == LogicalConnectives::OR) {
            check((condition_group_to_add.empty() == false), "Cannot use OR conditional at start");
            executor->add_condition(condition_group_to_add);
            condition_group_to_add.clear();
        }

        switch (sentence->condition_type) {
        case ConditionTypes::BuffCondition: {
            Buff* buff = pchar->get_enabled_buffs()->get_buff_by_name(sentence->type_value);
            if (buff == nullptr) {
                qDebug() << "could not find buff for condition:" << sentence->type_value;
                return false;
            }
            condition = new ConditionBuff(buff,
                                          sentence->mathematical_symbol,
                                          sentence->compared_value.toDouble());
            break;
        }
        case ConditionTypes::SpellCondition:
            condition = new ConditionSpell(pchar->get_spells()->get_spell_by_name(sentence->type_value),
                                           sentence->mathematical_symbol,
                                           sentence->compared_value.toDouble());
            break;
        case ConditionTypes::ResourceCondition:
            condition = new ConditionResource(this->pchar,
                                              sentence->mathematical_symbol,
                                              get_resource_from_string(sentence->type_value),
                                              sentence->compared_value.toDouble());
            break;
        case ConditionTypes::VariableBuiltinCondition:
            if (ConditionVariableBuiltin::get_builtin_variable(sentence->type_value) == BuiltinVariables::Undefined)
                break;
            condition = new ConditionVariableBuiltin(this->pchar,
                                                     ConditionVariableBuiltin::get_builtin_variable(sentence->type_value),
                                                     sentence->mathematical_symbol,
                                                     sentence->compared_value.toDouble());
            break;
        default:
            qDebug() << "condition type not supported:" << sentence->condition_type;
            sentence->dump();
            return false;
        }

        condition_group_to_add.append(condition);
    }

    // Add last condition manually
    if (!condition_group_to_add.empty())
        executor->add_condition(condition_group_to_add);

    return true;
}

double Rotation::get_time_required_to_run_precombat() {
    return precast_spell ? precast_spell->get_cast_time() : pchar->global_cooldown();
}

void Rotation::set_name(const QString& name) {
    this->name = name;
}

void Rotation::set_description(const QString& desc) {
    this->description = desc;
}

bool Rotation::try_set_attack_mode(const QString& value) {
    if (value == "melee")
        attack_mode = AttackMode::MeleeAttack;
    else if (value == "ranged")
        attack_mode = AttackMode::RangedAttack;
    else if (value == "magic")
        attack_mode = AttackMode::MagicAttack;
    else
        return false;

    return true;
}

void Rotation::add_variable(const QString& var, const QString& value) {
    this->defined_variables.insert(var, value);
}

void Rotation::add_prerequisite(const QString& key, const QString& value) {
    this->prerequisites.insert(key, value);
}

void Rotation::add_precombat_spell(const QString &spell_name) {
    this->precombat_spell_names.append(spell_name);
}

void Rotation::add_precast_spell(const QString &spell_name) {
    this->precast_spell_name = spell_name;
}

void Rotation::add_executor(RotationExecutor* executor) {
    this->all_executors.append(executor);
}

QString Rotation::get_class() const {
    return this->class_name;
}

QString Rotation::get_name() const {
    return this->name;
}

QString Rotation::get_description() const {
    return this->description;
}

AttackMode Rotation::get_attack_mode() const {
    return this->attack_mode;
}

ResourceType Rotation::get_resource_from_string(const QString& resource) const {
    if (resource == "Mana")
        return ResourceType::Mana;
    if (resource == "Rage")
        return ResourceType::Rage;
    if (resource == "Energy")
        return ResourceType::Energy;

    qDebug() << "Failed to find resource for" << resource;
    check(false, QString("Failed to find resource for '%1'").arg(resource).toStdString());
    return ResourceType::Rage;
}

void Rotation::dump() {
    qDebug() << "class" << class_name;
    qDebug() << "name" << name;
    qDebug() << "desc" << description;
    qDebug() << "defined_variables" << defined_variables;
    qDebug() << "prerequisites" << prerequisites;
    qDebug() << "executors:";
    for (auto & executor : all_executors) {
        executor->dump();
    }
}
