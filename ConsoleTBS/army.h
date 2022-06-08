#pragma once
#include <iostream>
#include <vector>

#include "creature.h"

enum class BattleStartStatus {
	kBattleStartStatusMin,
	kAttaking = kBattleStartStatusMin,
	kDefending,
	kBattleStartStatusMax
};

class Army {
public:

	Army() {
		army_.reserve(army_size_max_);

		static int army_id_counter_{};
		army_id_ = army_id_counter_++;
	}
	~Army() = default;

	int get_army_id() const { return army_id_; }
	std::vector<Creature>* get_army_ptr() { return &army_; }

	void generate_random_army();
	void clear();

public:
	static constexpr int army_size_max_{ 14 }; // changed from 18 to 14 for laptop

private:
	int army_id_;
	std::vector<Creature> army_;
};