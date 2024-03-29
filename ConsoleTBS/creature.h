#pragma once

#include <array>
#include <algorithm>
#include <string>
#include <utility>

#include "file_database.h"
#include "creature_stats.h"

namespace creature {

	enum class CreatureTemplateID {
		NoTemplate,
		kHumanSpearman,
		kOrcAxeman,
		kCreatureTemplateMax
	};

	class Creature {
	public:
		//Creature(CreatureRace creature_race, const std::array<int, static_cast<int>(StatId::kStatMax)>& target_creature_stats); // default creature_id = -1 because 0 can be used for first element
		Creature(creature::stat::Race creature_race, std::array<creature::stat::Stat, static_cast<int>(creature::stat::StatId::kStatMax)>&& target_creature_stats); // used to load database from file, !!! no ID !!!
		Creature(const Creature* creature_ptr, creature::stat::ComplexID&& creature_complex_id); // used to construct from database, receives only rhs ID because it must be unique
		//Creature(CreatureTemplateID creature_template) : Creature(*database_create_creature_by_template(creature_template)) {}

		~Creature() = default;

		//bool operator==(const Creature& another_creature) {
		//	return creature_id_ == another_creature.get_creature_id();
		//}



		// can be realized by pointer to first element
		//std::array<int, static_cast<int>(StatId::CreatureStatMax)>* get_all_stats() { return &stats_; }
		creature::stat::Stat get_stat_current_and_max(creature::stat::StatId stat_id_) const;
		int get_stat_current_value(creature::stat::StatId stat_id_) const;
		creature::stat::Race get_race() const;
		creature::stat::ComplexID get_complex_id();
		int get_army_id() const;
		size_t get_creature_id() const;
		const std::string* get_name() const;

		void change_stat_current_value(creature::stat::StatId stat_id_, float change_amount);
		void change_stat_current_value(creature::stat::StatId stat_id_, int change_amount);

		void receive_new_creature_id(int new_creature_id);
		int roll_stat_with_bonus(creature::stat::StatId stat_id_) const; // returns stat after bonus damage_multiplier
		int calculate_received_damage(const Creature* attacker_ptr, double damage_multiplier);

		// in future should be created new class for effects and shoving active effects on screen
		void apply_stat_multiplier(creature::stat::StatMultiplier stat_multiplier);
		void delete_stat_multiplier(creature::stat::StatMultiplier stat_multiplier);

	private:
		creature::stat::ComplexID complex_id_{};

		short level_{};
		std::string name_{}; // name selection can be modified by system with character modifiers (something like "likes jokes", "proud for family tree", etc)
		creature::stat::Race race_;
		// in future should add sex(male/female) too, and differency in names for them
		std::array<creature::stat::Stat, static_cast<int>(creature::stat::StatId::kStatMax)> stats_;
		// std::vector<Effect> active_effects;

		const std::string generate_name(); // in future should use creature_race and culture
	};
	

	void load_database_into_memory(file_database::ID database_id);
	void unload_database_from_memory(file_database::ID database_id);

	const creature::Creature* get_ptr_to_creature_template_from_database(CreatureTemplateID creature_template);

}