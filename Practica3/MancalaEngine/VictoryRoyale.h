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
  int MaxMove(const GameState &state, Move &bestMove, int nivel, int alpha,
              int beta);
  int MinMove(const GameState &state, Move &bestMove, int nivel, int alpha,
              int beta);
  Move MiniMax(const GameState &state, int alpha, int beta);
};

#endif /* MANUPCBOT_H_ */
