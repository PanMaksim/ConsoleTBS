#pragma once

#include <array>
#include <algorithm>
#include <string>
#include <utility>

#include "creature_stats.h"

enum class CreatureTemplate {
	kCreatureTemplateMin,
	kHumanSpearman = kCreatureTemplateMin,
	kOrcAxeman,
	kCreatureTemplateMax
};

class Creature {
public:
	Creature(CreatureRace race, std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)> target_creature_stats) : race_{ race } {
		std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
			creature_stat_ptr{ creature_stats_.data() };

		for (std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer 
			target_creature_stat_ptr{ target_creature_stats.data() }, 
			target_creature_stat_ptr_end{ target_creature_stats.data() + target_creature_stats.size()};
			target_creature_stat_ptr != target_creature_stat_ptr_end; ++target_creature_stat_ptr, ++creature_stat_ptr) {

			creature_stat_ptr->current = *target_creature_stat_ptr;
			creature_stat_ptr->max = *target_creature_stat_ptr;
		}

		std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
			creature_stat_ptr_end{ creature_stats_.data() + creature_stats_.size() };

		if (creature_stat_ptr != creature_stat_ptr_end) {
			for (; creature_stat_ptr != creature_stat_ptr_end; ++creature_stat_ptr) {

				creature_stat_ptr->current = 0;
				creature_stat_ptr->max = 0;
			}
		}

		name_ = generate_name();
	}
	Creature(const Creature* creature) : race_{ creature->get_race() } {
		std::copy(creature->creature_stats_.begin(), creature->creature_stats_.end(), this->creature_stats_.begin());
		name_ = generate_name();
	}
	//Creature(CreatureTemplate creature_template) : Creature(*database_create_creature_by_template(creature_template)) {}
	
	// can be realized by pointer to first element
	//std::array<int, static_cast<int>(CreatureStatId::CreatureStatMax)>* get_all_stats() { return &creature_stats_; }
	CreatureStat get_certain_stat_current_and_max(CreatureStatId creature_stat) const { return creature_stats_[static_cast<int>(creature_stat)]; }
	int get_certain_stat_current_value(CreatureStatId creature_stat) const { return creature_stats_[static_cast<int>(creature_stat)].current; }
	CreatureRace get_race() const { return race_; }
	int get_army_id() const { return army_id_; }
	const std::string* get_name() const { return &name_; }

	void change_certain_stat_current_value(CreatureStatId stat_id, float change_amount) { creature_stats_[static_cast<int>(stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_id)].current) + change_amount); };
	void change_certain_stat_current_value(CreatureStatId stat_id, int change_amount) { creature_stats_[static_cast<int>(stat_id)].current += change_amount; };

	void join_army(int army_id) { army_id_ = army_id; }
	int roll_stat_with_bonus(CreatureStatId creature_stat) const; // returns stat after bonus multiplier
	int calculate_received_damage(const Creature* attacker, double multiplier);
	
	// in future should be created new class for effects and shoving active effects on screen
	void apply_stat_multiplier(CreatureStatMultiplier stat_multiplier);
	void delete_stat_multiplier(CreatureStatMultiplier stat_multiplier);

private:
	int army_id_{ 0 };
	int level_{ 0 };
	std::string name_{ "No_Name" }; // name selection can be modified by system with character modifiers (something like "likes jokes", "proud for family tree", etc)
	CreatureRace race_;
	// in future should add sex(male/female) too, and differency in names for them
	std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)> creature_stats_{}; // pair for current/max stats

	// std::vector<Effect> active_effects;

	const std::string generate_name(); // in future should use race and culture
};



class creature
{
};
const Creature* creature_database_get_templates(CreatureTemplate creature_template);