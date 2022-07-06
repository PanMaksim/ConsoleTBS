
#include "creature.h"

#include <string>
#include <string_view>
#include <vector>

#include "random.h"
#include "creature_actions.h"
#include "creature_stats.h"

// Movement speed = 20 for faster testing
const Creature creature_database_templates[static_cast<int>(CreatureTemplate::kCreatureTemplateMax)]{
	{CreatureRace::kHuman, {8, 20, 8, 8, 1, 3} },
	{CreatureRace::kOrc, {10, 20, 10, 8, 1, 4} }
};

const Creature* creature_database_get_templates(CreatureTemplate creature_template) {
	return &creature_database_templates[static_cast<int>(creature_template)];
}

// possibly not const array, but only const strings inside array, must be checked
const std::string_view creature_database_first_name[]{
	"Alfons",
	"Adner",
	"Maksim",
	"Taras",
	"Jonny",
	"Lourence",
	"Claus",
	"Bernard",
	"Benny",
	"Ranald"
};

const std::string_view creature_database_second_name[]{
	"White",
	"Black",
	"Folknir",
	"Hastner",
	"Norman",
	"Festner",
	"Wulfheart",
	"Lionheart",
	"Benner",
	"Pestilens",
};

Creature::Creature(CreatureRace race, std::array<int, static_cast<int>(CreatureStatId::kCreatureStatMax)> target_creature_stats) : race_{ race } {
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

	name_ = generate_name();
}

Creature::Creature(const Creature* creature) : race_{ creature->get_race() } {
	std::copy(creature->creature_stats_.begin(), creature->creature_stats_.end(), this->creature_stats_.begin());
	name_ = generate_name();
}

CreatureStat Creature::get_certain_stat_current_and_max(CreatureStatId creature_stat) const { return creature_stats_[static_cast<int>(creature_stat)]; }
int Creature::get_certain_stat_current_value(CreatureStatId creature_stat) const { return creature_stats_[static_cast<int>(creature_stat)].current; }
CreatureRace Creature::get_race() const { return race_; }
int Creature::get_army_id() const { return army_id_; }
size_t Creature::get_creature_id() const { return creature_id_; }
const std::string* Creature::get_name() const { return &name_; }

void Creature::change_certain_stat_current_value(CreatureStatId stat_id, float change_amount) {
	creature_stats_[static_cast<int>(stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_id)].current) + change_amount); };

void Creature::change_certain_stat_current_value(CreatureStatId stat_id, int change_amount) { creature_stats_[static_cast<int>(stat_id)].current += change_amount; };

void Creature::join_army(int army_id) { army_id_ = army_id; }


int Creature::roll_stat_with_bonus(CreatureStatId creature_stat) const {
	return static_cast<int>(
		this->get_certain_stat_current_value(creature_stat) * creature_database_get_roll_result_multiplier(
			static_cast<RollResult>(get_random_number(static_cast<int>(RollResult::kRollResultMin), static_cast<int>(RollResult::kRollResultMax)))));
}

const std::string Creature::generate_name() {
	return static_cast<std::string>(creature_database_first_name[get_random_number(0, static_cast<int>(creature_database_first_name->size()))]) + ' ' +
		creature_database_second_name[get_random_number(0, static_cast<int>(creature_database_second_name->size()))].data(); // maube should not calculate size every time
}

// should add multipliers for different ATK chances (Critical, Success, Small hit)
int Creature::calculate_received_damage(const Creature* attacker, double multiplier) {
	return static_cast<int>(attacker->get_certain_stat_current_value(CreatureStatId::kDAMAGE_ML_PHS) * multiplier
			- this->get_certain_stat_current_value(CreatureStatId::kArmor_PHS));
}

void Creature::apply_stat_multiplier(CreatureStatMultiplier stat_multiplier) {
	creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current) * stat_multiplier.multiplier);
}

void Creature::delete_stat_multiplier(CreatureStatMultiplier stat_multiplier) {
	creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(stat_multiplier.stat_id)].current) / stat_multiplier.multiplier);
}

