/*
 * VictoryRoyale.cpp
 *
 *  Created on: 09 may. 2018
 *      Author: Fran Gonzalez
 */

#include "VictoryRoyale.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
using namespace std;

VictoryRoyale::VictoryRoyale() : depth(10) {}

VictoryRoyale::~VictoryRoyale() {}

void VictoryRoyale::initialize() {
  jugador = this->getPlayer();
  rival = jugador == J1 ? J2 : J1;
}

string VictoryRoyale::getName() { return "VictoryRoyale"; }

vector<Move> GenerateMoveList(const GameState &state, Player turno) {
  vector<Move> moves;
  for (int i = 1; i <= 6; i++)
    if (state.getSeedsAt(turno, (Position)i) > 0) moves.push_back((Move)i);

  return moves;
}

int VictoryRoyale::movimientosPosibles(const GameState &state, Player turno) {
  return GenerateMoveList(state, turno).size() * 10 / 6;
}

int VictoryRoyale::casillasLibres(const GameState &state, Player turno) {
  int score = 0;
  for (int i = 1; i <= 2; i++)
    if (state.getSeedsAt(turno, (Position)i) == 0) score += 7 - i;
  return score - 1;
}

int VictoryRoyale::piedrasTotal(const GameState &state, Player turno) {
  int total = 0;
  for (int i = 1; i <= 6; i++) total += state.getSeedsAt(turno, (Position)i);

  return total * 10 / 40;
}

double VictoryRoyale::heuristica(const GameState &state) {
  double score;
  score = (state.getScore(jugador) - state.getScore(rival) * 0.5) * 10 / 48;
  score += movimientosPosibles(state, rival) * 0.4;
  score += piedrasTotal(state, jugador) * 0.45;
  score += casillasLibres(state, jugador) * 0.2;
  return score;
}

double VictoryRoyale::MiniMax(const GameState &state, Move &bestMove,
                              double alpha, double beta, int nivel) {
  if (state.isFinalState() || nivel == 0) {
    return heuristica(state);
  }

  vector<Move> moveList = GenerateMoveList(state, state.getCurrentPlayer());
  int nMoves = moveList.size();
  double value = 0;
  Move opponentsBestMove;

  for (Move move : moveList) {
    GameState tmp_state = state.simulateMove(move);
    value = MiniMax(tmp_state, opponentsBestMove, alpha, beta, nivel - 1);
    if (state.getCurrentPlayer() == jugador) {
      if (alpha < value) {
        alpha = value;
        if (nivel == depth) bestMove = move;
      }
    } else if (state.getCurrentPlayer() == rival) {
      if (beta > value) {
        beta = value;
      }
    }
    if (beta <= alpha) {
      break;
    }
  }
  if (state.getCurrentPlayer() == jugador)
    return alpha;
  else
    return beta;
}

Move VictoryRoyale::nextMove(const vector<Move> &adversary,
                             const GameState &state) {
  static unsigned contador = 0;
  Move movimiento = M_NONE;
  static bool inicio = true;
  if (inicio) {
    initialize();
    inicio = false;
  }
  double alpha = -1000;
  double beta = 1000;

  MiniMax(state, movimiento, alpha, beta, depth);
  cerr << ++contador << endl;
  return movimiento;
}
