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

VictoryRoyale::VictoryRoyale() {
  // Inicializar las variables necesarias para ejecutar la partida
}

VictoryRoyale::~VictoryRoyale() {
  // Liberar los recursos reservados (memoria, ficheros, etc.)
}

void VictoryRoyale::initialize() {
  // Inicializar el bot antes de jugar una partida
}

string VictoryRoyale::getName() { return "VictoryRoyale"; }

vector<Move> GenerateMoveList(const GameState &state) {
  vector<Move> moves;
  for (int i = 1; i <= 6; i++)
    if (state.getSeedsAt(state.getCurrentPlayer(), (Position)i) > 0)
      moves.push_back((Move)i);

  return moves;
}

set<int> listaS;
int VictoryRoyale::MaxMove(const GameState &state, Move &bestMove, int nivel,
                           int alpha, int beta) {
  if (state.isFinalState() || nivel == 0) {
    int semillas = state.getScore(jugador);
    return semillas;
  }

  vector<Move> moveList = GenerateMoveList(state);
  int nMoves = moveList.size();
  int v = -1000;

  for (int i = 0; i < nMoves; i++) {
    Move move = moveList[i];
    GameState tmp_state = state.simulateMove(move);
    Move opponentsBestMove;
    int alpha = MinMove(tmp_state, opponentsBestMove, nivel - 1, alpha, beta);
    if (alpha > v) {
      v = alpha;
      bestMove = move;
    }
    if (beta >= alpha) break;
  }
  return v;
}
int VictoryRoyale::MinMove(const GameState &state, Move &bestMove, int nivel,
                           int alpha, int beta) {
  if (state.isFinalState() || nivel == 0) {
    int semillas = state.getScore(jugador);
    return semillas;
  }

  vector<Move> moveList = GenerateMoveList(state);
  int nMoves = moveList.size();
  int v = 1000;

  for (int i = 0; i < nMoves; i++) {
    Move move = moveList[i];
    GameState tmp_state = state.simulateMove(move);
    Move opponentsBestMove;
    alpha = MaxMove(tmp_state, opponentsBestMove, nivel - 1, alpha, beta);
    if (alpha < v) {
      v = alpha;
      bestMove = move;
    }
    if (beta >= alpha) break;
  }
  return v;
}

Move VictoryRoyale::MiniMax(const GameState &state, int alpha, int beta) {
  // Movimiento aleatorio
  srand(time(0));
  vector<Move> list = GenerateMoveList(state);
  int i = rand() % list.size();
  Move bestMove = list[i];

  MaxMove(state, bestMove, 9, alpha, beta);

  return bestMove;
}

Move VictoryRoyale::nextMove(const vector<Move> &adversary,
                             const GameState &state) {
  Move movimiento = M_NONE;
  jugador = state.getCurrentPlayer();
  rival = jugador == J1 ? J2 : J1;
  int alpha = -1000;
  int beta = 1000;

  // Si es el primer movimiento de la partida siempre sembramos la primera
  // casilla
  static bool primerTurno = true;
  if (rival == J2 and primerTurno)
    for (int i = 1; i < 7; i++)
      if (state.getSeedsAt(rival, (Position)i) != 4)
        primerTurno = false;
      else if (rival == J1)
        primerTurno = false;
  if (primerTurno) {
    movimiento = (Move)1;
    primerTurno = false;
  } else {
    movimiento = MiniMax(state, alpha, beta);
  }

  return movimiento;
}
