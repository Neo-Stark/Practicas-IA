/*
 * VictoryRoyale.h
 *
 *  Created on: 09 may. 2018
 *      Author: Fran Gonzalez
 */

#include "Bot.h"

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class VictoryRoyale : Bot {
 public:
  VictoryRoyale();
  ~VictoryRoyale();
  Player jugador, rival;

  void initialize();
  string getName();
  Move nextMove(const vector<Move> &adversary, const GameState &state);
  int MiniMax(const GameState &state, Move &bestMove, int alpha, int beta,
              int nivel);
  int heuristica(const GameState &estado);
 private:
  unsigned int depth;
};

#endif /* MANUPCBOT_H_ */
