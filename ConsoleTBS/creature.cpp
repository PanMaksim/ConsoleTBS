
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

std::unique_ptr<std::vector<Creature>> creature_database_templates;

void load_creature_main_database(FileDatabaseId database_id) { // open file and copy info into needed array
	if (file_databases_status[static_cast<int>(database_id)] == true) {
#ifdef debug_log
		log_in_file("ERROR, tried to open already opened database.", true);
#endif
		return;
	}

	std::ifstream txt_database;
	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		txt_database.open("TextDatabases/creature_first_names_database.txt", std::ios::app);
		if (!txt_database) {
#ifdef debug_log
			log_in_file("ERROR, database not found.", true);
#endif
		}

		creature_database_first_name = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		txt_database.close();

		txt_database.open("TextDatabases/creature_last_names_database.txt", std::ios::app);
		if (!txt_database) {
#ifdef debug_log
			log_in_file("ERROR, database not found.", true);
#endif
		}
		creature_database_last_name = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		break;
	case FileDatabaseId::kCreatureTemplateDatabase:
		txt_database.open("TextDatabases/creature_templates_database.txt", std::ios::app);
		if (!txt_database) {
			std::cerr << "ERROR, database not found.\n";
		}

		{
			creature_database_templates = std::make_unique<std::vector<Creature>>();
			std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)> creature_stats;
			for (int templates_counter{}, templates_max{ static_cast<int>(CreatureTemplate::kCreatureTemplateMax) };
				templates_counter != templates_max; ++ templates_counter) { // changed to number of templates from check until file ends because it readed last string twice for unknown reason

				// read CreatureRace
				int creature_race{};
				txt_database >> creature_race;

				// read CreatureStats
				creature_stats = std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>();
				for (std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::iterator
					current_stat_iter{ creature_stats.begin() }, stat_iter_end{ creature_stats.end() };
					current_stat_iter != stat_iter_end; ++current_stat_iter) {

					txt_database >> *current_stat_iter;
				}

				// load Creature into database
				creature_database_templates->emplace_back(static_cast<CreatureRace>(creature_race), std::move(creature_stats));
			}
		}
		break;
	default:
#ifdef debug_log
		log_in_file("Error, tried to open unknown database.", true);
#endif
		break;
	}

	file_databases_status[static_cast<int>(database_id)] = true;
}

void unload_creature_main_database(FileDatabaseId database_id) { // release memory
	if (file_databases_status[static_cast<int>(database_id)] == false) {
#ifdef debug_log
		log_in_file("ERROR, tried to close unopened database.", true);
#endif
		return;
	}

	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		creature_database_first_name.reset();
		creature_database_last_name.reset();
		break;
	case FileDatabaseId::kCreatureTemplateDatabase:
		creature_database_templates.reset();
		break;
	default:
		log_in_file("Error, tried to close unknown database.", true);
	}

	file_databases_status[static_cast<int>(database_id)] = false;
}


const Creature* creature_database_get_templates(CreatureTemplate creature_template) {
	return &(*creature_database_templates)[static_cast<int>(creature_template)];
}

//Creature::Creature(CreatureRace creature_race, const std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)>& target_creature_stats) : race_{ creature_race } {
//	std::copy(target_creature_stats.begin(), target_creature_stats.end(), std::back_inserter(creature_stats_));
//	
//	//std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
//	//	creature_stat_ptr{ creature_stats_.data() };
//
//	/*for (std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)>::const_pointer
//		target_creature_stat_ptr{ target_creature_stats.data() },
//		target_creature_stat_ptr_end{ target_creature_stats.data() + target_creature_stats.size() };
//		target_creature_stat_ptr != target_creature_stat_ptr_end; ++target_creature_stat_ptr, ++creature_stat_ptr) {
//
//		creature_stat_ptr->current_ = *target_creature_stat_ptr;
//		creature_stat_ptr->max_ = *target_creature_stat_ptr;
//	}*/
//
//	//std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>::pointer
//	//	creature_stat_ptr_end{ creature_stats_.data() + creature_stats_.size() };
//
//	//if (creature_stat_ptr != creature_stat_ptr_end) {
//	//	for (; creature_stat_ptr != creature_stat_ptr_end; ++creature_stat_ptr) {
//
//	//		creature_stat_ptr->current_ = 0;
//	//		creature_stat_ptr->max_ = 0;
//	//	}
//	//}
//
//	name_ = "No_Name";
//	creature_id_ = creature_id_counter++;
//}

Creature::Creature(CreatureRace creature_race, std::array<CreatureStat, static_cast<int>(CreatureStatId::kCreatureStatMax)>&& target_creature_stats) : race_{creature_race} {
	std::move(target_creature_stats.begin(), target_creature_stats.end(), creature_stats_.begin());
	name_ = "No_Name";
}

Creature::Creature(const Creature* creature_ptr, CreatureComplexID&& creature_complex_id) : race_{ creature_ptr->get_race() }, creature_complex_id_{ creature_complex_id } {
	std::copy(creature_ptr->creature_stats_.begin(), creature_ptr->creature_stats_.end(), creature_stats_.begin());
	name_ = generate_name();
}

CreatureStat Creature::get_certain_stat_current_and_max(CreatureStatId stat_id_) const { return creature_stats_[static_cast<int>(stat_id_)]; }
int Creature::get_certain_stat_current_value(CreatureStatId stat_id_) const { return creature_stats_[static_cast<int>(stat_id_)].current_; }
CreatureRace Creature::get_race() const { return race_; }
CreatureComplexID Creature::get_creature_complex_id() { return creature_complex_id_; }
int Creature::get_army_id() const { return creature_complex_id_.army_id_; }
size_t Creature::get_creature_id() const { return creature_complex_id_.creature_id_; }
const std::string* Creature::get_name() const { return &name_; }

void Creature::change_certain_stat_current_value(CreatureStatId stat_id_, float change_amount) {
	creature_stats_[static_cast<int>(stat_id_)].current_ = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_id_)].current_) + change_amount); };

void Creature::change_certain_stat_current_value(CreatureStatId stat_id_, int change_amount) { 
	creature_stats_[static_cast<int>(stat_id_)].current_ += change_amount; };

void Creature::receive_new_creature_simple_id(int new_creature_id) { creature_complex_id_.creature_id_ = new_creature_id; }

int Creature::roll_stat_with_bonus(CreatureStatId stat_id_) const {
	return static_cast<int>(
		this->get_certain_stat_current_value(stat_id_) * creature_database_get_roll_result_multiplier(
			static_cast<RollResult>(get_random_number(static_cast<int>(RollResult::kCriticalFail), static_cast<int>(RollResult::kRollResultMax)))));
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
	creature_stats_[static_cast<int>(stat_multiplier.stat_id_)].current_ = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id_)].max_) * stat_multiplier.multiplier_);
}

void Creature::delete_stat_multiplier(CreatureStatMultiplier stat_multiplier) {
	creature_stats_[static_cast<int>(stat_multiplier.stat_id_)].current_ = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id_)].max_) / stat_multiplier.multiplier_);
}

