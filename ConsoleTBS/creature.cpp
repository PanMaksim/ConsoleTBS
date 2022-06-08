
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

void Creature::apply_stat_multiplier(CreatureStatId effect_targeted_stat, float effect_size) {
	creature_stats_[static_cast<int>(effect_targeted_stat)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(effect_targeted_stat)].current) * effect_size);
}

void Creature::delete_stat_multiplier(CreatureStatId effect_targeted_stat, float effect_size) {
	creature_stats_[static_cast<int>(effect_targeted_stat)].current = static_cast<int>(static_cast<float>(creature_stats_[static_cast<int>(effect_targeted_stat)].current) / effect_size);
}
