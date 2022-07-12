#pragma once

#include <array>
#include <algorithm>
#include <string>
#include <utility>

#include "file_database.h"
#include "creature_stats.h"

void load_creature_main_database(FileDatabaseId database_id);
void unload_creature_main_database(FileDatabaseId database_id);

enum class CreatureTemplate {
	kCreatureTemplateMin,
	kHumanSpearman = kCreatureTemplateMin,
	kOrcAxeman,
	kCreatureTemplateMax
};

static size_t creature_id_counter{};

class Creature {
public:
	Creature(CreatureRace creature_race, std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)> target_creature_stats);
	Creature(const Creature* creature_ptr);
	//Creature(CreatureTemplate creature_template) : Creature(*database_create_creature_by_template(creature_template)) {}
	
	//bool operator==(const Creature& another_creature) {
	//	return creature_id_ == another_creature.get_creature_id();
	//}


	// can be realized by pointer to first element
	//std::array<int, static_cast<int>(CreatureStatId::CreatureStatMax)>* get_all_stats() { return &creature_stats_; }
	CreatureStat get_certain_stat_current_and_max(CreatureStatId stat_id) const;
	int get_certain_stat_current_value(CreatureStatId stat_id) const;
	CreatureRace get_race() const;
	int get_army_id() const;
	size_t get_creature_id() const;
	const std::string* get_name() const;

	void change_certain_stat_current_value(CreatureStatId stat_id, float change_amount);
	void change_certain_stat_current_value(CreatureStatId stat_id, int change_amount);

	void join_army(int army_id);
	int roll_stat_with_bonus(CreatureStatId stat_id) const; // returns stat after bonus damage_multiplier
	int calculate_received_damage(const Creature* attacker_ptr, double damage_multiplier);
	
	// in future should be created new class for effects and shoving active effects on screen
	void apply_stat_multiplier(CreatureStatMultiplier stat_multiplier);
	void delete_stat_multiplier(CreatureStatMultiplier stat_multiplier);

private:
	int army_id_{};
	size_t creature_id_{ creature_id_counter++ };

	int level_{};
	std::string name_{}; // name selection can be modified by system with character modifiers (something like "likes jokes", "proud for family tree", etc)
	CreatureRace race_;
	// in future should add sex(male/female) too, and differency in names for them
	std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)> creature_stats_{}; // pair for current/max stats

	// std::vector<Effect> active_effects;

	const std::string generate_name(); // in future should use creature_race and culture
};

const Creature* creature_database_get_templates(CreatureTemplate creature_template);