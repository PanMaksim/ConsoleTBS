#pragma once
#include <iostream>
#include <vector>
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
		~Army() = default;

		short get_army_id() const;
		int get_army_size() const; // not size_t because there can't be so much creatures, max creature_id_ is limited to max int value.

		//std::weak_ptr<std::vector<std::shared_ptr<Creature>>> get_army_weak_ptr(); // currently unused
		std::shared_ptr<std::vector<std::shared_ptr<creature::Creature>>> get_army_shared_ptr();

		creature::stat::ComplexID generate_creature_complex_id();
		void kill_creature(size_t creature_id_);

		void clear();

		std::vector<std::shared_ptr<creature::Creature>>::iterator begin();
		std::vector<std::shared_ptr<creature::Creature>>::iterator end();

		friend Army generate_random_army();
	private:

	private:
		short army_id_;
		std::shared_ptr<std::vector<std::shared_ptr<creature::Creature>>> army_; // owned by Army AND BattleTile

		int creature_id_counter_{};
	};

	Army generate_random_army();

}