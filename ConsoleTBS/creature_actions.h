#pragma once

#include <memory>
#include <string>

#include "creature.h"
#include "rolls.h"

namespace creature {
	namespace actions {

		//enum class AbilityResult {
		//	kAbilityResultMin,
		//	kGotDead = kAbilityResultMin,
		//	kCriticalFail,
		//	kFailed,
		//	kNoResult,
		//	kSuccess,
		//	kCriticalSuccess,
		//	kKilledTarget,
		//	kAbilityResultMax
		//};

		struct ActionResult {
			roll::RollResult ability_result;
			int damage_dealed;
			int damage_received;
		};

		//const std::string* creature_database_get_ability_result_naming(AbilityResult ability_result);

		void log_action_result(const ActionResult* action_result, const Creature* action_dealer, const Creature* action_target);

		ActionResult ability_default_attack(Creature* attacker, Creature* defender);

	}
}