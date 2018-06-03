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
  double MiniMax(const GameState &state, Move &bestMove, double alpha,
                 double beta, int nivel);
  double heuristica(const GameState &);
  int movimientosPosibles(const GameState &, Player);
  int piedrasTotal(const GameState &, Player);
  int casillasLibres(const GameState &, Player);

 private:
  unsigned int depth;
};

#endif /* FRANPCBOT_H_ */
