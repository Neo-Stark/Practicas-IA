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

int VictoryRoyale::heuristica(const GameState &state) {
  int score;
  if (jugador == J1)
    score = state.getScore((Player)0) - state.getScore((Player)1);
  else
    score = state.getScore((Player)1) - state.getScore((Player)0);
  return score;
}

int VictoryRoyale::MiniMax(const GameState &state, Move &bestMove, int alpha,
                           int beta, int nivel) {
  if (state.isFinalState() || nivel == 0) {
    return heuristica(state);
  }
  vector<Move> moveList = GenerateMoveList(state);
  int nMoves = moveList.size();
  int value = 1000;
  Move opponentsBestMove;

  for (Move move : moveList) {
    GameState tmp_state = state.simulateMove(move);
    value = MiniMax(tmp_state, opponentsBestMove, alpha, beta, nivel - 1);
    if (state.getCurrentPlayer() == jugador) {
      if (alpha < value) {
        alpha = value;
        bestMove = move;
      }
      if (beta <= alpha) {
        break;
      }
    } else if (state.getCurrentPlayer() == rival) {
      if (beta > value) {
        beta = value;
        bestMove = move;
      }
      if (beta <= alpha) {
        break;
      }
    }
  }
  if (state.getCurrentPlayer() == jugador)
    return alpha;
  else
    return beta;
}

Move VictoryRoyale::nextMove(const vector<Move> &adversary,
                             const GameState &state) {
  Move movimiento = M_NONE;
  jugador = state.getCurrentPlayer();
  rival = jugador == J1 ? J2 : J1;
  int alpha = -1000;
  int beta = 1000;

  // Aleatorio
  vector<Move> list = GenerateMoveList(state);
  srand(time(0));
  int i = rand() % list.size();
  movimiento = list[i];

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
    MiniMax(state, movimiento, alpha, beta, depth);
  }

  return movimiento;
}
