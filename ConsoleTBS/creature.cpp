
#include "creature.h"

#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <memory>

#include <iostream> // only for cerr

#include "logger.h"
#include "file_database.h"
#include "random.h"
#include "creature_actions.h"
#include "creature_stats.h"

std::unique_ptr<std::vector<std::string>> creature_database_first_name;
std::unique_ptr<std::vector<std::string>> creature_database_last_name;

void load_creature_main_database(FileDatabaseId database_id) { // open file and copy info into needed array
	if (file_databases_status[static_cast<int>(database_id)] == true) {
		log_in_file("ERROR, tried to open already opened database.", true);
		return;
	}

	std::ifstream txt_database;
	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		txt_database.open("TextDatabases/creature_first_names_database.txt", std::ios::app);
		if (!txt_database) {
			log_in_file("ERROR, database not found.", true);
		}

		creature_database_first_name = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		txt_database.close();

		txt_database.open("TextDatabases/creature_last_names_database.txt", std::ios::app);
		if (!txt_database) {
			log_in_file("ERROR, database not found.", true);
		}
		creature_database_last_name = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		break;
	default:
		log_in_file("Error, tried to open unknown database.", true);
	}

	file_databases_status[static_cast<int>(database_id)] = true;
}

void unload_creature_main_database(FileDatabaseId database_id) { // release memory
	if (file_databases_status[static_cast<int>(database_id)] == false) {
		log_in_file("ERROR, tried to close unopened database.", true);
		return;
	}

	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		creature_database_first_name.reset();
		creature_database_last_name.reset();
		break;
	default:
		log_in_file("Error, tried to close unknown database.", true);
	}

	file_databases_status[static_cast<int>(database_id)] = false;
}

// Movement speed = 20 for faster testing
const Creature creature_database_templates[static_cast<int>(CreatureTemplate::kCreatureTemplateMax)]{
	{CreatureRace::kHuman, {8, 20, 8, 8, 1, 3} },
	{CreatureRace::kOrc, {10, 20, 10, 8, 1, 4} }
};

const Creature* creature_database_get_templates(CreatureTemplate creature_template) {
	return &creature_database_templates[static_cast<int>(creature_template)];
}

Creature::Creature(CreatureRace creature_race, std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)> target_creature_stats) : race_{ creature_race } {
	std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
		creature_stat_ptr{ creature_stats_.data() };

	for (std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
		target_creature_stat_ptr{ target_creature_stats.data() },
		target_creature_stat_ptr_end{ target_creature_stats.data() + target_creature_stats.size() };
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

	name_ = "No_Name";
}

Creature::Creature(const Creature* creature_ptr) : race_{ creature_ptr->get_race() } {
	std::copy(creature_ptr->creature_stats_.begin(), creature_ptr->creature_stats_.end(), this->creature_stats_.begin());
	name_ = generate_name();
}

CreatureStat Creature::get_certain_stat_current_and_max(CreatureStatId stat_id) const { return creature_stats_[static_cast<int>(stat_id)]; }
int Creature::get_certain_stat_current_value(CreatureStatId stat_id) const { return creature_stats_[static_cast<int>(stat_id)].current; }
CreatureRace Creature::get_race() const { return race_; }
int Creature::get_army_id() const { return army_id_; }
size_t Creature::get_creature_id() const { return creature_id_; }
const std::string* Creature::get_name() const { return &name_; }

void Creature::change_certain_stat_current_value(CreatureStatId stat_id, float change_amount) {
	creature_stats_[static_cast<int>(stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_id)].current) + change_amount); };

void Creature::change_certain_stat_current_value(CreatureStatId stat_id, int change_amount) { creature_stats_[static_cast<int>(stat_id)].current += change_amount; };

void Creature::join_army(int army_id) { army_id_ = army_id; }


int Creature::roll_stat_with_bonus(CreatureStatId stat_id) const {
	return static_cast<int>(
		this->get_certain_stat_current_value(stat_id) * creature_database_get_roll_result_multiplier(
			static_cast<RollResult>(get_random_number(static_cast<int>(RollResult::kRollResultMin), static_cast<int>(RollResult::kRollResultMax)))));
}

const std::string Creature::generate_name() {
	if (creature_database_first_name == nullptr || creature_database_last_name == nullptr) {
		log_in_file("ERROR, tried to generate name when database is not open.", true);
	}
	return (*creature_database_first_name)[get_random_number(0, creature_database_first_name->size() - 1)] + ' ' +
		(*creature_database_last_name)[get_random_number(0, creature_database_last_name->size() - 1)]; // maube should not calculate size every time
}

// should add multipliers for different ATK chances (Critical, Success, Small hit)
int Creature::calculate_received_damage(const Creature* attacker_ptr, double damage_multiplier) {
	return static_cast<int>(attacker_ptr->get_certain_stat_current_value(CreatureStatId::kDAMAGE_ML_PHS) * damage_multiplier
			- this->get_certain_stat_current_value(CreatureStatId::kArmor_PHS));
}

void Creature::apply_stat_multiplier(CreatureStatMultiplier stat_multiplier) {
	creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id)].max) * stat_multiplier.multiplier);
}

void Creature::delete_stat_multiplier(CreatureStatMultiplier stat_multiplier) {
	creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id)].max) / stat_multiplier.multiplier);
}

