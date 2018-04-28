#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <cmath>
#include <iostream>
#include <queue>
#include <vector>
#include <stack>

void ComportamientoJugador::PintaPlan(list<Action> plan) {
  auto it = plan.begin();
  while (it != plan.end()) {
    if (*it == actFORWARD) {
      cout << "A ";
    } else if (*it == actTURN_R) {
      cout << "D ";
    } else if (*it == actTURN_L) {
      cout << "I ";
    } else {
      cout << "- ";
    }
    it++;
  }
  cout << endl;
}

// bool ComportamientoJugador::creaPlan(){}

bool ComportamientoJugador::pathFinding(const estado &origen,
                                        const estado &destino,
                                        list<Action> &plan) {
  // Borro la lista
  plan.clear();

  estado st = origen;
  int TAM = mapaResultado.size();

  int df[4] = {-1, 0, 1, 0};  // incremento en coordenada x
  int dc[4] = {0, 1, 0, -1};  // incremento en coordanada y
  queue<estado> Q;
  vector<vector<bool>> visitado(TAM, vector<bool>(TAM, false));
  vector<vector<estado>> padre(TAM, vector<estado>(TAM, estado(-1, -1, -1)));
  Q.push(origen);

  while (!Q.empty()) {
    estado actual = Q.front();
    Q.pop();
    if (actual.fila == destino.fila and actual.columna == destino.columna) {
		 cout << "***********************OBJETIVO ENCONTRADO************************\n";
      stack<estado> camino;
      int i = actual.fila, j = actual.columna;
      while (padre[i][j].orientacion != -1) {
        camino.push(padre[i][j]);
		  int x = i, y = j;
		  cout << "\nPADRE " << i << "  " << j;
		  i = padre[x][y].fila;
		  j = padre[x][y].columna;
      }
      while (!camino.empty()) {
        estado paso = camino.top();
		  cout << "\nCAMINO  " << paso.fila << " " << paso.columna << " " << paso.orientacion; 
        camino.pop();
        estado sig;
        if (!camino.empty()) sig = camino.top();
        if (paso.orientacion != sig.orientacion) {
          bool direccion = false;  // izquierda = true / derecha = false
          switch (paso.orientacion) {
            case 0:
              if (sig.orientacion == 3) direccion = true;
              break;
            case 1:
              if (sig.orientacion == 0) direccion = true;
              break;
            case 2:
              if (sig.orientacion == 1) direccion = true;
              break;
            case 3:
              if (sig.orientacion == 2) direccion = true;
              break;
          }
          if (direccion)
            plan.push_back(actTURN_L);
          else
            plan.push_back(actTURN_R);
        }
        plan.push_back(actFORWARD);
      }

      VisualizaPlan(origen, plan);
      return true;
    }
    visitado[actual.fila][actual.columna] = true;
    for (int i = 0; i < 4; i++) {
      int fil = df[i] + actual.fila;
      int col = dc[i] + actual.columna;
      unsigned char contenidoCasilla = mapaResultado[fil][col];
      if ((contenidoCasilla == 'S' or contenidoCasilla == 'T' or
           contenidoCasilla == 'K') &&
          !visitado[fil][col]) {
        padre[fil][col] = actual;
        cout << "fil: " << actual.fila << "  col: " << actual.columna
             << " Or: " << actual.orientacion << endl;
        cout << "\tfil: " << fil << "  col: " << col << " Or: " << i << endl;
        estado adyacente(fil, col, i);
        Q.push(adyacente);
      }
    }
  }

  while (st.fila != destino.fila or st.columna != destino.columna) {
  }
}

Action ComportamientoJugador::think(Sensores sensores) {
  if (sensores.mensajeF != -1) {
    fil = sensores.mensajeF;
    col = sensores.mensajeC;
  }

  // Actualizar el efecto de la ultima accion
  switch (ultimaAccion) {
    case actTURN_R:
      brujula = (brujula + 1) % 4;
      break;
    case actTURN_L:
      brujula = (brujula + 3) % 4;
      break;
    case actFORWARD:
      switch (brujula) {
        case 0:
          fil--;
          break;
        case 1:
          col++;
          break;
        case 2:
          fil++;
          break;
        case 3:
          col--;
          break;
      }
      cout << "fil: " << fil << "  col: " << col << " Or: " << brujula << endl;
  }

  // Determinar si ha cambiado el destino desde la ultima planificacion
  if (hayPlan and (sensores.destinoF != destino.fila or
                   sensores.destinoC != destino.columna)) {
    cout << "El destino ha cambiado\n";
    hayPlan = false;
  }

  // Determinar si tengo que construir un plan
  if (!hayPlan) {
    estado origen(fil, col, brujula);

    destino.fila = sensores.destinoF;
    destino.columna = sensores.destinoC;

    hayPlan = pathFinding(origen, destino, plan);
  }

  // Ejecutar el plan
  Action sigAccion;
  if (hayPlan and plan.size() > 0) {
    sigAccion = plan.front();
    plan.erase(plan.begin());
  } else {
    sigAccion = actIDLE;
  }

  ultimaAccion = sigAccion;
  return sigAccion;
}

void AnularMatriz(vector<vector<unsigned char>> &m) {
  for (int i = 0; i < m[0].size(); i++) {
    for (int j = 0; j < m.size(); j++) {
      m[i][j] = 0;
    }
  }
}

void ComportamientoJugador::VisualizaPlan(const estado &st,
                                          const list<Action> &plan) {
  AnularMatriz(mapaConPlan);
  estado cst = st;

  auto it = plan.begin();
  while (it != plan.end()) {
    if (*it == actFORWARD) {
      switch (cst.orientacion) {
        case 0:
          cst.fila--;
          break;
        case 1:
          cst.columna++;
          break;
        case 2:
          cst.fila++;
          break;
        case 3:
          cst.columna--;
          break;
      }
      mapaConPlan[cst.fila][cst.columna] = 1;
    } else if (*it == actTURN_R) {
      cst.orientacion = (cst.orientacion + 1) % 4;
    } else {
      cst.orientacion = (cst.orientacion + 3) % 4;
    }
    it++;
  }
}

int ComportamientoJugador::interact(Action accion, int valor) { return false; }
