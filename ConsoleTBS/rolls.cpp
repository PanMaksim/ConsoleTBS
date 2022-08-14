#include "rolls.h"

#include <string>

#include "turn_based_game_global.h"
#include "creature.h"
#include "creature_stats.h"

const std::string creature_database_roll_result_naming[static_cast<int>(RollResult::kRollResultMax)]{
	"Critical Fail",
	"Fail",
	"Small Fail",
	"No Result",
	"Small Success",
	"Success",
	"Critical Success"
};

const std::string* creature_database_get_roll_result_naming(RollResult roll_result) {
	return &creature_database_roll_result_naming[static_cast<int>(roll_result)];
}

const double creature_database_roll_result_multipliers[static_cast<int>(RollResult::kRollResultMax)]{
	0.5,
	0.75,
	1,
	0,
	1,
	1.25,
	1.5
};

const double creature_database_get_roll_result_multiplier(RollResult roll_result) {
	return creature_database_roll_result_multipliers[static_cast<int>(roll_result)];
}

RollResult compare_rolls(const Creature* action_dealer, creature::StatId action_dealer_stat_to_compare,
	const Creature* target, creature::StatId target_stat_to_compare) {

	RollResult roll_result;

	int creature_roll{ action_dealer->roll_stat_with_bonus(action_dealer_stat_to_compare) },
		target_roll{ target->roll_stat_with_bonus(target_stat_to_compare) };

	add_string_to_ui_log(*(action_dealer->get_name()) + " rolled: " + std::to_string(creature_roll));
	add_string_to_ui_log(*(target->get_name()) + " rolled: " + std::to_string(target_roll));

	if (creature_roll > target_roll) { // action success
		int roll_difference{ creature_roll - target_roll };
		if (roll_difference < creature_roll * 0.25) { // no bonus
			roll_result = RollResult::kSmallSuccess;
		}
		else if (roll_difference < creature_roll * 0.5) { // success
			roll_result = RollResult::kSuccess;
		}
		else { // critical success
			roll_result = RollResult::kCriticalSuccess;
		}
	}
	else if (creature_roll < target_roll) { // fail
		int roll_difference{ target_roll - creature_roll };
		if (roll_difference < target_roll * 0.25) { // no bonus
			roll_result = RollResult::kSmallFail;
		}
		else if (roll_difference < target_roll * 0.5) { // success
			roll_result = RollResult::kFail;
		}
		else { // critical success
			roll_result = RollResult::kCriticalFail;
		}
	}
	else {
		roll_result = RollResult::kNoResult;
	}

	//game_ptr->add_string_to_ui_log( "Roll result: " + *creature_database_get_roll_result_naming(roll_result));
	return roll_result;
}
