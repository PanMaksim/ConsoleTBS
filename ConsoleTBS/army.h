#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <memory>

#include "creature.h"

namespace army {

	extern const int army_size_max_;

	enum class BattleStartStatus {
		kAttaking,
		kDefending,
		kBattleStartStatusMax
	};

	class Army {
	public:

		Army();
		// default copy constructor is being used in generate_random_army
		~Army() = default;

		short get_army_id() const;
		int get_army_size() const; // not size_t because there can't be so much creatures, max creature_id_ is limited to max int value.

		//std::weak_ptr<std::vector<std::shared_ptr<Creature>>> get_army_weak_ptr(); // currently unused
		std::shared_ptr<std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>> get_army_shared_ptr();

		creature::stat::ComplexID generate_creature_complex_id();
		void kill_creature(std::shared_ptr<creature::Creature> creature);

		void clear();

		std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>::iterator begin();
		std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>::iterator end();

		friend Army generate_random_army();
	private:

	private:
		short army_id_;
		std::shared_ptr<std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>> army_; // creatures owned by Army AND BattleTile, they can't live only in army or only in battle_tile

		int creature_id_counter_{};
	};

	Army generate_random_army();

}