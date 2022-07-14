#pragma once
#include <iostream>
#include <vector>

#include "creature.h"

enum class BattleStartStatus {
	kAttaking,
	kDefending,
	kBattleStartStatusMax
};

class Army {
public:
	static constexpr int army_size_max_{ 14 }; // changed from 18 to 14 for laptop

public:

	Army();
	~Army() = default;

	int get_army_id() const;
	size_t get_army_size() const;
	std::vector<Creature>* get_army_ptr();

	void kill_creature(size_t creature_id);

	void generate_random_army();
	void clear();
private:

private:
	int army_id_;
	std::vector<Creature> army_;
};