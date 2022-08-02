#pragma once
#include <iostream>
#include <vector>

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
	std::vector<Creature>* get_army_ptr();

	void kill_creature(size_t creature_id);

	void clear();
private:

private:
	int army_id_;
	std::vector<Creature> army_;
};

Army generate_random_army();