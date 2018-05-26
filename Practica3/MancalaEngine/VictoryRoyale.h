/*
 * VictoryRoyale.h
 *
 *  Created on: 09 may. 2018
 *      Author: Fran Gonzalez
 */

#include "Bot.h"

#ifndef FRANPCBOT_H_
#define FRANPCBOT_H_

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

#endif /* FRANPCBOT_H_ */
