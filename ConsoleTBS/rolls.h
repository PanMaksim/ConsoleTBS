#pragma once

#include <string>

#include "creature.h"
#include "creature_stats.h"

namespace roll {

	enum class RollResult {
		kCriticalFail,
		kFail,
		kSmallFail, // no bonus
		kNoResult,
		kSmallSuccess, // no bonus
		kSuccess,
		kCriticalSuccess,
		kRollResultMax
	};

	const std::string* get_roll_result_naming_from_database(RollResult roll_result);
	const double get_roll_result_multiplier_from_database(RollResult roll_result);

	RollResult compare_rolls(const creature::Creature* action_dealer, creature::stat::StatId action_dealer_stat_to_compare,
		const creature::Creature* target, creature::stat::StatId target_stat_to_compare);

}
