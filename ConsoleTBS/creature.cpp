
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

using namespace creature;

std::unique_ptr<std::vector<std::string>> creature_first_name_database;
std::unique_ptr<std::vector<std::string>> creature_last_name_database;

std::unique_ptr<std::vector<Creature>> creature_template_database;

void creature::load_database_into_memory(FileDatabaseId database_id) { // open file and copy info into needed array
	if (file_databases_status[static_cast<int>(database_id)] == true) {
#ifdef debug_log
		runtime_logger::log_in_file("ERROR, tried to open already opened database.", true);
#endif
		return;
	}

	std::ifstream txt_database;
	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		txt_database.open("TextDatabases/creature_first_names_database.txt", std::ios::app);
		if (!txt_database) {
#ifdef debug_log
			runtime_logger::log_in_file("ERROR, database not found.", true);
#endif
		}

		creature_first_name_database = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		txt_database.close();

		txt_database.open("TextDatabases/creature_last_names_database.txt", std::ios::app);
		if (!txt_database) {
#ifdef debug_log
			runtime_logger::log_in_file("ERROR, database not found.", true);
#endif
		}
		creature_last_name_database = std::make_unique<std::vector<std::string>>(
			std::istream_iterator<std::string>(txt_database), std::istream_iterator<std::string>());
		break;
	case FileDatabaseId::kCreatureTemplateDatabase:
		txt_database.open("TextDatabases/creature_templates_database.txt", std::ios::app);
		if (!txt_database) {
			std::cerr << "ERROR, database not found.\n";
		}

		{
			creature_template_database = std::make_unique<std::vector<Creature>>();
			std::array<Stat, static_cast<int>(StatId::kStatMax)> creature_stats;
			for (int templates_counter{}, templates_max{ static_cast<int>(CreatureTemplateID::kCreatureTemplateMax) };
				templates_counter != templates_max; ++ templates_counter) { // changed to number of templates from check until file ends because it readed last string twice for unknown reason

				// read Race
				int creature_race{};
				txt_database >> creature_race;

				// read CreatureStats
				creature_stats = std::array<Stat, static_cast<int>(StatId::kStatMax)>();
				for (std::array<Stat, static_cast<int>(StatId::kStatMax)>::iterator
					current_stat_iter{ creature_stats.begin() }, stat_iter_end{ creature_stats.end() };
					current_stat_iter != stat_iter_end; ++current_stat_iter) {

					txt_database >> *current_stat_iter;
				}

				// load Creature into database
				creature_template_database->emplace_back(static_cast<Race>(creature_race), std::move(creature_stats));
			}
		}
		break;
	default:
#ifdef debug_log
		runtime_logger::log_in_file("Error, tried to open unknown database.", true);
#endif
		break;
	}

	file_databases_status[static_cast<int>(database_id)] = true;
}

void creature::unload_database_from_memory(FileDatabaseId database_id) { // release memory
	if (file_databases_status[static_cast<int>(database_id)] == false) {
#ifdef debug_log
		runtime_logger::log_in_file("ERROR, tried to close unopened database.", true);
#endif
		return;
	}

	switch (database_id) {
	case FileDatabaseId::kCreatureNameDatabase:
		creature_first_name_database.reset();
		creature_last_name_database.reset();
		break;
	case FileDatabaseId::kCreatureTemplateDatabase:
		creature_template_database.reset();
		break;
	default:
		runtime_logger::log_in_file("Error, tried to close unknown database.", true);
	}

	file_databases_status[static_cast<int>(database_id)] = false;
}


const Creature* creature::get_ptr_to_creature_template_from_database(CreatureTemplateID creature_template) {
	return &(*creature_template_database)[static_cast<int>(creature_template)];
}

//Creature::Creature(Race creature_race, const std::array<int, static_cast<int>(StatId::kStatMax)>& target_creature_stats) : race_{ creature_race } {
//	std::copy(target_creature_stats.begin(), target_creature_stats.end(), std::back_inserter(stats_));
//	
//	//std::array<Stat, static_cast<int>(StatId::kStatMax)>::pointer
//	//	creature_stat_ptr{ stats_.data() };
//
//	/*for (std::array<int, static_cast<int>(StatId::kStatMax)>::const_pointer
//		target_creature_stat_ptr{ target_creature_stats.data() },
//		target_creature_stat_ptr_end{ target_creature_stats.data() + target_creature_stats.size() };
//		target_creature_stat_ptr != target_creature_stat_ptr_end; ++target_creature_stat_ptr, ++creature_stat_ptr) {
//
//		creature_stat_ptr->current_ = *target_creature_stat_ptr;
//		creature_stat_ptr->max_ = *target_creature_stat_ptr;
//	}*/
//
//	//std::array<Stat, static_cast<int>(StatId::kStatMax)>::pointer
//	//	creature_stat_ptr_end{ stats_.data() + stats_.size() };
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

creature::Creature::Creature(Race creature_race, std::array<Stat, static_cast<int>(StatId::kStatMax)>&& target_creature_stats) : race_{creature_race} {
	std::move(target_creature_stats.begin(), target_creature_stats.end(), stats_.begin());
	name_ = "No_Name";
}

creature::Creature::Creature(const Creature* creature_ptr, ComplexID&& creature_complex_id) : race_{ creature_ptr->get_race() }, complex_id_{ creature_complex_id } {
	std::copy(creature_ptr->stats_.begin(), creature_ptr->stats_.end(), stats_.begin());
	name_ = generate_name();
}

Stat creature::Creature::get_stat_current_and_max(StatId stat_id_) const { return stats_[static_cast<int>(stat_id_)]; }
int creature::Creature::get_stat_current_value(StatId stat_id_) const { return stats_[static_cast<int>(stat_id_)].current_; }
Race creature::Creature::get_race() const { return race_; }
ComplexID creature::Creature::get_complex_id() { return complex_id_; }
int creature::Creature::get_army_id() const { return complex_id_.army_id_; }
size_t creature::Creature::get_creature_id() const { return complex_id_.creature_id_; }
const std::string* creature::Creature::get_name() const { return &name_; }

void creature::Creature::change_stat_current_value(StatId stat_id_, float change_amount) {
	stats_[static_cast<int>(stat_id_)].current_ = static_cast<int>(static_cast<float>(stats_[static_cast<int>(stat_id_)].current_) + change_amount); };

void creature::Creature::change_stat_current_value(StatId stat_id_, int change_amount) {
	stats_[static_cast<int>(stat_id_)].current_ += change_amount; };

void creature::Creature::receive_new_creature_id(int new_creature_id) { complex_id_.creature_id_ = new_creature_id; }

int creature::Creature::roll_stat_with_bonus(StatId stat_id_) const {
	return static_cast<int>(
		this->get_stat_current_value(stat_id_) * creature_database_get_roll_result_multiplier(
			static_cast<roll::RollResult>(random::get_random_number(static_cast<int>(roll::RollResult::kCriticalFail), static_cast<int>(roll::RollResult::kRollResultMax)))));
}

const std::string creature::Creature::generate_name() {
	if (creature_first_name_database == nullptr || creature_last_name_database == nullptr) {
		runtime_logger::log_in_file("ERROR, tried to generate name when database is not open.", true);
	}
	return (*creature_first_name_database)[random::get_random_number(0, creature_first_name_database->size() - 1)] + ' ' +
		(*creature_last_name_database)[random::get_random_number(0, creature_last_name_database->size() - 1)]; // maube should not calculate size every time
}

// should add multipliers for different ATK chances (Critical, Success, Small hit)
int creature::Creature::calculate_received_damage(const Creature* attacker_ptr, double damage_multiplier) {
	return static_cast<int>(attacker_ptr->get_stat_current_value(StatId::kDAMAGE_ML_PHS) * damage_multiplier
			- this->get_stat_current_value(StatId::kArmor_PHS));
}

void creature::Creature::apply_stat_multiplier(StatMultiplier stat_multiplier) {
	stats_[static_cast<int>(stat_multiplier.stat_id_)].current_ = static_cast<int>(static_cast<float>(stats_[static_cast<int>(stat_multiplier.stat_id_)].max_) * stat_multiplier.multiplier_);
}

void creature::Creature::delete_stat_multiplier(StatMultiplier stat_multiplier) {
	stats_[static_cast<int>(stat_multiplier.stat_id_)].current_ = static_cast<int>(static_cast<float>(stats_[static_cast<int>(stat_multiplier.stat_id_)].max_) / stat_multiplier.multiplier_);
}

