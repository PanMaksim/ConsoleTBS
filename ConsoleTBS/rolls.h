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

	const std::string* creature_database_get_roll_result_naming(RollResult roll_result);
	const double creature_database_get_roll_result_multiplier(RollResult roll_result);

	RollResult compare_rolls(const creature::Creature* action_dealer, creature::StatId action_dealer_stat_to_compare,
		const creature::Creature* target, creature::StatId target_stat_to_compare);

}
