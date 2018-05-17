/*
 * VictoryRoyale.h
 *
 *  Created on: 09 may. 2018
 *      Author: Fran Gonzalez
 */

#include "Bot.h"

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class VictoryRoyale:Bot {
public:
	VictoryRoyale();
	~VictoryRoyale();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
};

#endif /* MANUPCBOT_H_ */
