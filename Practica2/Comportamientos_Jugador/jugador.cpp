#include "../Comportamientos_Jugador/jugador.hpp"
#include <time.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include "motorlib/util.h"

void ComportamientoJugador::PintaPlan(list<Action> plan) {
  auto it = plan.begin();
  while (it != plan.end()) {
    if (*it == actFORWARD)
      cout << "A ";
    else if (*it == actTURN_R)
      cout << "D ";
    else if (*it == actTURN_L)
      cout << "I ";
    else
      cout << "- ";

    it++;
  }
  cout << endl;
}

void AnularMatriz(vector<vector<unsigned char>> &m) {
  for (size_t i = 0; i < m[0].size(); i++) {
    for (size_t j = 0; j < m.size(); j++) {
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
      if (celda_valida(cst.fila, cst.columna))
        mapaConPlan[cst.fila][cst.columna] = 1;
    } else if (*it == actTURN_R) {
      cst.orientacion = (cst.orientacion + 1) % 4;
    } else {
      cst.orientacion = (cst.orientacion + 3) % 4;
    }
    it++;
  }
}

bool ComportamientoJugador::pathFinding(const estado &origen,
                                        const estado &destino,
                                        list<Action> &plan) const {
  list<estado> camino;
  if (origen.fila != destino.fila or origen.columna != destino.columna) {
    cout << "***********NO SE HA BUSCADO CAMINO************" << endl;
    camino = busqueda_a_estrella(origen, destino);
    cout << "***********ENCONTRADO CAMINO************" << endl;

    auto it1 = camino.begin();
    auto it2 = ++camino.begin();
    while (it2 != camino.end()) {
      int g1 = it1->orientacion;
      int g2 = it2->orientacion;
      int giro = (g2 - g1);

      if (giro == 2 || giro == -2) {
        plan.push_back(actTURN_L);
        plan.push_back(actTURN_L);
      } else {
        if (g1 == 0 && g2 == 3)
          plan.push_back(actTURN_L);
        else if (g1 == 3 && g2 == 0)
          plan.push_back(actTURN_R);
        else if (giro < 0)
          plan.push_back(actTURN_L);
        else if (giro > 0)
          plan.push_back(actTURN_R);
      }

      plan.push_back(actFORWARD);
      it1++;
      it2++;
    }
  }

  return true;
}

pair<int, int> ComportamientoJugador::obtener_celda(int orientacion,
                                                    int pos) const {
  int i, j;
  pair<int, int> celda;
  if (pos == 0) {
    i = 0;
    j = 0;
  } else if (pos <= 3) {
    i = 1;
    j = pos - 2;
  } else if (pos <= 8) {
    i = 2;
    j = pos - 6;
  } else if (pos <= 15) {
    i = 3;
    j = pos - 12;
  }

  switch (orientacion) {
    case 0:
      celda = std::make_pair(actual.fila - i, actual.columna + j);
      break;
    case 1:
      celda = std::make_pair(actual.fila + j, actual.columna + i);
      break;
    case 2:
      celda = std::make_pair(actual.fila + i, actual.columna - j);
      break;
    case 3:
      celda = std::make_pair(actual.fila - j, actual.columna - i);
      break;
  }

  return celda;
}

void ComportamientoJugador::reconstruir_mapa(vector<unsigned char> terreno) {
  for (size_t i = 0; i < terreno.size(); i++) {
    auto pos = obtener_celda(actual.orientacion, i);
    int fil = pos.first;
    int col = pos.second;
    if (celda_valida(fil, col) && mapaResultado[fil][col] == '?')
      mapaResultado[fil][col] = terreno[i];
  }
}

list<Action> ComportamientoJugador::llegar_PK(int pos) {
  list<Action> camino;
  if (pos <= 15 and pos >= 9) {
    if (pos < 12) {
      camino.push_back(actTURN_L);
    } else if (pos > 12) {
      camino.push_back(actTURN_R);
    }
  } else if (pos <= 8 and pos >= 4) {
    if (pos < 6) {
      camino.push_back(actTURN_L);
    } else if (pos > 6) {
      camino.push_back(actTURN_R);
    }
  } else if (pos == 1)
    camino.push_back(actTURN_L);
  else if (pos == 3)
    camino.push_back(actTURN_R);

  if (pos == 13 or pos == 3 or pos == 7) {
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_L);
  } else if (pos == 11 or pos == 5 or pos == 1) {
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_R);
  } else if (pos == 10 or pos == 4) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_R);
  } else if (pos == 14 or pos == 8) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_L);
  } else if (pos == 9) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_R);
  } else if (pos == 15) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actTURN_L);
  }

  if (pos >= 9) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
  } else if (pos <= 8 and pos >= 4) {
    camino.push_back(actFORWARD);
    camino.push_back(actFORWARD);
  } else if (pos <= 3)
    camino.push_back(actFORWARD);

  return camino;
}

void ComportamientoJugador::recalcular_plan() {
  plan.clear();
  // cout << "***********SIN PLAN***********" << endl;
  // VisualizaPlan(actual, plan);
  hayPlan = pathFinding(actual, destino, plan);
  // cout << "***********CON PLAN***********" << endl;
  // VisualizaPlan(actual, plan);
}

Action girar_aleatorio() {
  int accion = rand() % 2;
  if (accion)
    return actTURN_L;
  else
    return actTURN_R;
}

Action ComportamientoJugador::think(Sensores sensores) {
  static bool actual_valida = false;
  static bool localizadoPK = false;
  Action sigAccion = actIDLE;
  bool obstaculo = false;
  static int espera = 0;
  const int MAX_ESPERA = 10;
  cout << "*************SENSORES***********\n"
       << sensores.mensajeF << '\t' << sensores.mensajeC << endl;
  cout << "*************ACTUAL***********\n"
       << actual.fila << '\t' << actual.columna << endl;
  cout << "*************DESTINO-SENSOR***********\n"
       << sensores.destinoF << '\t' << sensores.destinoC << endl;
  cout << "*************DESTINO***********\n"
       << destino.fila << '\t' << destino.columna << endl;

  auto terreno = sensores.terreno;
  auto superficie = sensores.superficie;

  // Inicializar estado actual.
  if (sensores.mensajeF != -1) {
    actual.fila = sensores.mensajeF;
    actual.columna = sensores.mensajeC;
    actual_valida = true;
  } else if (!actual_valida) {  // No tenemos una posicion de inicio valida -->
                                // Buscamos PK
    int posPK;
    if (!localizadoPK) {
      for (int i = 0; i < terreno.size(); i++) {
        localizadoPK = terreno[i] == 'K';
        if (localizadoPK) posPK = i;
      }
    }
    if (terreno[0] == 'K') {
      localizadoPK = false;
      actual_valida = true;
      actual.fila = sensores.mensajeF;
      actual.columna = sensores.mensajeC;
    } else if (localizadoPK and plan.size() == 0) {
      plan = llegar_PK(posPK);
    } else {
      srand(time(NULL));
      if (celda_permitida(terreno[2]))
        sigAccion = actFORWARD;
      else {
        sigAccion = girar_aleatorio();
      }
    }
  }

  actual_valida = celda_valida(actual.fila, actual.columna);
  // Si se cambia el objetivo y hay punto de referencia, crear un nuevo
  // plan.
  if (actual_valida and (destino.fila != sensores.destinoF or
                         destino.columna != sensores.destinoC)) {
    destino.fila = sensores.destinoF;
    destino.columna = sensores.destinoC;

    cout << "Nuevo destino: " << destino.fila << ' ' << destino.columna << endl;
    plan.clear();
    // Muestra el mapa
    // for(auto i =0; i<mapaResultado.size();i++){
    //   for(auto j =0; j<mapaResultado.size();j++){
    //       char contenido = mapaResultado[i][j];
    //       cout << contenido  << " ";
    //   }
    //   cout << endl;
    // }
    cout << "*************DESTINO***********\n"
         << destino.fila << '\t' << destino.columna << endl;
    cout << "*************ACTUAL***********\n"
         << actual.fila << '\t' << actual.columna << endl;

    hayPlan = pathFinding(actual, destino, plan);
  }
  // Posicion actual del personaje
  cout << "Posicion: " << actual.fila << ' ' << actual.columna << ' '
       << actual.orientacion << endl;

  if (hayPlan and actual_valida) reconstruir_mapa(terreno);
  // Si nos encontramos con un aldeano nos quedamos quietos hasta que se
  // vaya
  if (superficie[2] == 'a') {
    sigAccion = actIDLE;
    obstaculo = true;
    if (espera < MAX_ESPERA)
      espera++;
    else {
      sigAccion = girar_aleatorio();
      espera = 0;
    }
  }
  if (!celda_permitida(terreno[2]) and plan.front() == actFORWARD and
      actual_valida and
      (actual.fila != destino.fila or actual.columna != destino.columna)) {
    cout << "*****RECALCULAR*********" << endl;
    hayPlan = false;
    recalcular_plan();
    PintaPlan(plan);
  }

  if (!celda_permitida(terreno[2]) and localizadoPK) {
    localizadoPK = false;
    plan.clear();
  }

  // Ejecutar el plan
  if (hayPlan and plan.size() > 0 and !obstaculo) {
    VisualizaPlan(actual, plan);
    sigAccion = plan.front();
    plan.erase(plan.begin());
    PintaPlan(plan);
    cout << "PLAN VISTO" << endl;
  }

  // Ejecuta el plan para llegar a PK
  if (localizadoPK and plan.size() > 0 and !obstaculo) {
    sigAccion = plan.front();
    plan.erase(plan.begin());
  }

  ultimaAccion = sigAccion;
  // Actualizar el efecto de la ultima accion
  switch (ultimaAccion) {
    case actIDLE:
      break;
    case actTURN_R:
      actual.orientacion = (actual.orientacion + 1) % 4;
      break;
    case actTURN_L:
      actual.orientacion = (actual.orientacion + 3) % 4;
      break;
    case actFORWARD:
      switch (actual.orientacion) {
        case 0:
          actual.fila--;
          break;
        case 1:
          actual.columna++;
          break;
        case 2:
          actual.fila++;
          break;
        case 3:
          actual.columna--;
          break;
      }
  }
  return sigAccion;
}

void ComportamientoJugador::pinta_mapa(vector<unsigned char> terreno) {
  int fil = actual.fila;
  int col = actual.columna;
  int pos_v = 0;
  int di, dj;

  switch (actual.orientacion) {
    case 0:
      di = -1;
      dj = 1;
      break;
    case 1:
      di = -1;
      dj = 1;
      break;
    case 2:
      di = 1;
      dj = -1;
      break;
    case 3:
      di = 1;
      dj = -1;
      break;
  }
  if (actual.orientacion == 0 or actual.orientacion == 2) {
    for (int i = 0; i <= 3; i++) {
      if (i >= 1) {
        mapaResultado[fil + i * di][col + i * dj] = terreno[pos_v];
        pos_v++;
      }
      if (i >= 3) {
        mapaResultado[fil + i * di][col + 2 * dj] = terreno[pos_v];
        pos_v++;
      }
      if (i >= 2) {
        mapaResultado[fil + i * di][col + 1 * dj] = terreno[pos_v];
        pos_v++;
      }
      for (int j = 0; j <= i; j++) {
        mapaResultado[fil + i * dj][col + j * dj] = terreno[pos_v];
        pos_v++;
      }
    }
  } else {
    for (int i = 0; i <= 3; i++) {
      cout << pos_v << endl;
      if (i >= 1) {
        mapaResultado[fil + i * di][col + i * dj] = terreno[pos_v];
        pos_v++;
      }
      if (i >= 3) {
        mapaResultado[fil + 2 * di][col + i * dj] = terreno[pos_v];
        pos_v++;
      }
      if (i >= 2) {
        mapaResultado[fil + 1 * di][col + i * dj] = terreno[pos_v];
        pos_v++;
      }
      for (int j = 0; j <= i; j++) {
        mapaResultado[fil + j * dj][col + i * dj] = terreno[pos_v];
        pos_v++;
      }
    }
  }
}

int ComportamientoJugador::interact(Action accion, int valor) { return false; }

bool ComportamientoJugador::celda_permitida(char contenidoCasilla) const {
  return contenidoCasilla == 'S' or contenidoCasilla == 'T' or
         contenidoCasilla == 'K' or contenidoCasilla == '?';
}

bool ComportamientoJugador::celda_permitida(int fila, int columna) const {
  char contenido = mapaResultado[fila][columna];
  // cout << "*****CELDA PERMITIDA***CONTENIDO: " << contenido << endl;
  return celda_valida(fila, columna) and celda_permitida(contenido);
}

bool ComportamientoJugador::celda_valida(int fila, int columna) const {
  int max = mapaResultado.size();
  return (fila > 0 && fila < max - 3) && (columna > 0 && columna < max - 3);
}

estado mejorNodo(list<estado> abiertos) {
  auto it = abiertos.begin();
  estado elegido = *it;
  while (++it != abiertos.end())
    if (it->f < elegido.f) elegido = *it;
  return elegido;
}

list<estado> ComportamientoJugador::busqueda_a_estrella(
    const estado &origen, const estado &destino) const {
  bool encontrado = false;

  int TAM = mapaResultado.size();
  cout << "##########TAM MAPA#########" << TAM << endl;
  vector<vector<bool>> cerrados(TAM, vector<bool>(TAM, false));
  vector<vector<estado>> estados_celda(TAM, vector<estado>(TAM));
  list<estado> abiertos;
  int df[4] = {-1, 0, 1, 0};  // incremento de filas
  int dc[4] = {0, 1, 0, -1};  // incremento de columnas

  // Inicializar matriz de estados con el coste de f(g+h) a infinito y la
  // posicion de las celdas
  for (auto i = 0; i < TAM; i++) {
    for (auto j = 0; j < TAM; j++) {
      estados_celda[i][j].f = INFINITY;
      estados_celda[i][j].fila = i;
      estados_celda[i][j].columna = j;
    }
  }

  // Inicializar el estado origen en la matriz.
  estados_celda[origen.fila][origen.columna].f = 0;
  estados_celda[origen.fila][origen.columna].h = 0;
  estados_celda[origen.fila][origen.columna].g = 0;
  estados_celda[origen.fila][origen.columna].orientacion = origen.orientacion;

  // Añadir origen a abiertos.
  abiertos.push_back(origen);
  cout << "********ORIGEN ANIADIDO**********" << endl;

  while (!abiertos.empty() && !encontrado) {
    estado q = mejorNodo(abiertos);

    abiertos.remove(q);

    auto q_obj = estados_celda[q.fila][q.columna];

    for (int i = 0; i < 4; i++) {
      // Obtener los nodos adyacentes
      int n_fil = df[i] + q_obj.fila;
      int n_col = dc[i] + q_obj.columna;

      if (!celda_valida(n_fil, n_col)) break;

      if (n_fil == destino.fila && n_col == destino.columna) {
        cout << "Camino encontrado" << endl;
        // Establecer informacion estado final.
        estados_celda[n_fil][n_col] =
            estado(n_fil, n_col, 0, 0, 0, q_obj.fila, q_obj.columna, i);
        encontrado = true;
        break;
      }

      if (!cerrados[n_fil][n_col] && celda_permitida(n_fil, n_col)) {
        int n_g = q_obj.g + 1;
        int n_h = abs(q_obj.fila - n_fil) + abs(q_obj.columna - n_col);
        int n_f = n_g + n_h;

        //  añadir a la lista de abiertos y actualizar valor de f si tiene un
        //  coste menor
        if (estados_celda[n_fil][n_col].f > n_f) {
          estado hijo(n_fil, n_col, n_f, n_g, n_h, q_obj.fila, q_obj.columna,
                      i);
          estados_celda[n_fil][n_col] = hijo;
          abiertos.push_back(hijo);
        }
      }
    }
    // Añadir nodo actual a la lista de cerrados.
    cerrados[q_obj.fila][q_obj.columna] = true;
  }

  list<estado> lista;

  if (encontrado) {
    estado e = estados_celda[destino.fila][destino.columna];
    while (celda_valida(e.padre_f, e.padre_c)) {
      lista.push_front(e);
      e = estados_celda[e.padre_f][e.padre_c];
    }
    lista.push_front(origen);
  }

  return lista;
}
