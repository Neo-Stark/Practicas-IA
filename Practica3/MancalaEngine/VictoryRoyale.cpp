/*
 * VictoryRoyale.cpp
 *
 *  Created on: 09 may. 2018
 *      Author: Fran Gonzalez
 */

#include "VictoryRoyale.h"

#include <cstdlib>
#include <iostream>
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

int MaxMove(const GameState &state, Move &bestMove, int nivel) {
  if (state.isFinalState()) {
    return state.getScore(state.getCurrentPlayer());
  }
  vector<Move> moveList = GenerateMoveList(state);
  int nMoves = moveList.size();
  int v = -1000;
  for (int i = 0; i < nMoves; i++) {
    Move move = moveList[i];
    state.simulateMove(move);
    Move opponentsBestMove;
    int Rating = MinMove(state, opponentsBestMove, ++nivel);
    if (Rating > v) {
      v = Rating;
      bestMove = move;
    }
  }
  return v;
}
int MinMove(const GameState &state, Move &bestMove, int nivel) {
  if (state.isFinalState() || nivel == 6) {
    return state.getScore(state.getCurrentPlayer());
  }
  vector<Move> moveList = GenerateMoveList(state);
  int nMoves = moveList.size();
  int v = 1000;
  for (int i = 0; i < nMoves; i++) {
    Move move = moveList[i];
    state.simulateMove(move);
    Move opponentsBestMove;
    int Rating = MaxMove(state, opponentsBestMove, ++nivel);
    if (Rating < v) {
      v = Rating;
      bestMove = move;
    }
  }
  return v;
}

Move MiniMax(const GameState &state) {
  Move bestMove;
  int i = 0;
  if (state.getCurrentPlayer() == (Player)1) {
    i = MaxMove(state, bestMove, 0);
  } else {
    i = MinMove(state, bestMove, 0);
  }
  return bestMove;
}

Move VictoryRoyale::nextMove(const vector<Move> &adversary,
                             const GameState &state) {
  Move movimiento = M_NONE;
  Player turno = state.getCurrentPlayer();
  Player rival = turno == J1 ? J2 : J1;

  // Si es el primer movimiento de la partida siempre sembramos la primera
  // casilla
  static bool primerTurno = true;
  if (rival == J2 and primerTurno)
    for (int i = 1; i < 7; i++)
      if (state.getSeedsAt(rival, (Position)i) != 4) primerTurno = false;

  int puntos = -10000;

	
	
  if (primerTurno) movimiento = (Move)1;

  return movimiento;
}
