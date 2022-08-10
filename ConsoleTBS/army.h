#pragma once
#include <iostream>
#include <vector>
#include <memory>

#include "creature.h"

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

	int get_army_id() const;
	size_t get_army_size() const;

	//std::weak_ptr<std::vector<std::shared_ptr<Creature>>> get_army_weak_ptr(); // currently unused
	std::shared_ptr<std::vector<std::shared_ptr<Creature>>> get_army_shared_ptr();

	void kill_creature(size_t creature_id);

	void clear();

	std::vector<std::shared_ptr<Creature>>::iterator begin();
	std::vector<std::shared_ptr<Creature>>::iterator end();

private:
	friend Army generate_random_army();

private:
	int army_id_;
	std::shared_ptr<std::vector<std::shared_ptr<Creature>>> army_; // owned by Army AND BattleTile
};

Army generate_random_army();