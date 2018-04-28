#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <time.h>

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
      mapaConPlan[cst.fila][cst.columna] = 1;
    } else if (*it == actTURN_R) {
      cst.orientacion = (cst.orientacion + 1) % 4;
    } else {
      cst.orientacion = (cst.orientacion + 3) % 4;
    }
    it++;
  }
}

list<Action> diferencia_a_accion(const estado &e1, const estado &e2) {
  list<Action> acciones;

  if (e1 == e2)
    return acciones;

  int o1 = e1.orientacion;
  int o2 = e2.orientacion;
  int rot = (o2 - o1);

  if (rot == 2 || rot == -2) {
    acciones.push_back(actTURN_L);
    acciones.push_back(actTURN_L);
  } else {
    if (o1 == 0 && o2 == 3)
      acciones.push_back(actTURN_L);
    else if (o1 == 3 && o2 == 0)
      acciones.push_back(actTURN_R);
    else if (rot < 0)
      acciones.push_back(actTURN_L);
    else if (rot > 0)
      acciones.push_back(actTURN_R);
  }

  acciones.push_back(actFORWARD);

  return acciones;
}

bool ComportamientoJugador::pathFinding(const estado &origen,
                                        const estado &destino,
                                        list<Action> &plan) const {
  list<estado> lista_estados = algoritmo_a_star(origen, destino);

  if (lista_estados.size() < 2)
    return false;

  auto it1 = lista_estados.begin();
  auto it2 = ++lista_estados.begin();

  while (it2 != lista_estados.end()) {
    auto acciones = diferencia_a_accion(*it1, *it2);
    plan.insert(plan.end(), acciones.begin(), acciones.end());
    it1++;
    it2++;
  }

  return true;
}

pair<int, int> ComportamientoJugador::proyectar_vector(int orientacion,
                                                       int pos) const {
  int i, j;

  if (pos == 0)
    return std::make_pair(actual.fila, actual.columna);
  else if (pos <= 3) {
    i = 1;
    j = pos - 2;
  } else if (pos <= 8) {
    i = 2;
    j = pos - 6;
  } else if (pos <= 15) {
    i = 3;
    j = pos - 12;
  }

  if (orientacion == 0)
    return std::make_pair(actual.fila - i, actual.columna + j);
  if (orientacion == 1)
    return std::make_pair(actual.fila + j, actual.columna + i);
  if (orientacion == 2)
    return std::make_pair(actual.fila + j, actual.columna - i);
  else
    return std::make_pair(actual.fila + i, actual.columna - j);
}

void ComportamientoJugador::limpiar_matriz_aldeanos() {
  for (auto bs : mapa_aldeanos)
    bs.reset();
}

estado ComportamientoJugador::aplicar_accion(const estado &e, Action a) const {
  auto resultado = estado(e);

  if (a == actTURN_L)
    resultado.orientacion = (resultado.orientacion - 1 + 4) % 4;
  if (a == actTURN_R)
    resultado.orientacion = (resultado.orientacion + 1) % 4;
  if (a == actFORWARD) {
    auto n_pos = orientacion_a_movimiento(resultado.orientacion, resultado.fila,
                                          resultado.columna);
    resultado.fila = n_pos.first;
    resultado.columna = n_pos.second;
  }

  return resultado;
}

void ComportamientoJugador::reconstruir_terreno(vector<unsigned char> terreno) {
  for (size_t i = 0; i < terreno.size(); i++) {
    auto pos = proyectar_vector(actual.orientacion, i);
    int fil = pos.first;
    int col = pos.second;
    if (celda_valida(fil, col) && mapaResultado[fil][col] == '?')
      mapaResultado[fil][col] = terreno[i];
  }
}

void ComportamientoJugador::proyectar_mapa_temporal(int fila_inicial,
                                                    int columna_inicial,
                                                    int fila_actual,
                                                    int columna_actual) {
  vector<vector<unsigned char>> mapa_temporal(mapaResultado);
  AnularMatriz(mapaResultado);
  int diff_fila = fila_actual - fila_inicial;
  int diff_col = columna_actual - columna_inicial;

  for (int i = 0; i < TAM_MAPA; i++) {
    for (int j = 0; j < TAM_MAPA; j++) {
      if (mapa_temporal[i][j] != '?')
        mapaResultado[i + diff_fila][j + diff_col] = mapa_temporal[i][j];
    }
  }
}

void ComportamientoJugador::procesar_punto_referencia(int fila, int columna,
                                                      int pos_inicial) {
  actual.fila = fila;
  actual.columna = columna;
}

list<Action> ComportamientoJugador::generar_plan_aleatorio() const {
  estado d;
  srand(time(NULL));

  while(!celda_valida(d.fila, d.columna) || celda_bloqueada(d.fila, d.columna)) {
    d.fila = rand() % TAM_MAPA;
    d.columna = rand() % TAM_MAPA;
  }

  list<Action> p;
  pathFinding(actual, d, p);
  return p;
}

void ComportamientoJugador::recalcular_plan() {
   plan.clear();
   pathFinding(actual, destino, plan);
}

Action ComportamientoJugador::think(Sensores sensores) {
  static int esperando = 0;
  static const int MAX_ESPERA = 15;
  static const int POS_POR_DEFECTO = TAM_MAPA / 2;
  static bool pk_encontrado = false;
  static bool alcanzando_pk = false;

  bool actual_valida = celda_valida(actual.fila, actual.columna);
  auto terreno = sensores.terreno;
  auto superficie = sensores.superficie;

  reconstruir_terreno(terreno);

  // Inicializar estado actual.
  if (celda_valida(sensores.mensajeF, sensores.mensajeC) &&
      actual_valida == false) {
    actual.fila = sensores.mensajeF;
    actual.columna = sensores.mensajeC;
    actual_valida = true;
    pk_encontrado = true;
  } else if (actual_valida == false) {
    actual.fila = POS_POR_DEFECTO;
    actual.columna = POS_POR_DEFECTO;
  }

  // Busca el Punto de Referencia
  if (pk_encontrado == false) {
    if (terreno[0] == 'K') {
      actual_valida = true;
      pk_encontrado = true;
      alcanzando_pk = false;
      actual.fila = sensores.mensajeF;
      actual.columna = sensores.mensajeC;
      proyectar_mapa_temporal(POS_POR_DEFECTO, POS_POR_DEFECTO, actual.fila,
                              actual.columna);
      cout << "Punto de referencia encontrado: " << sensores.mensajeF << " "
           << sensores.mensajeC << endl;
    } else {
      auto pk = std::find(terreno.begin(), terreno.end(), 'K');
      if (!alcanzando_pk && pk != terreno.end()) {
        int pk_index = pk - terreno.begin();
        auto pk_pos = proyectar_vector(actual.orientacion, pk_index);
        destino.fila = pk_pos.first;
        destino.columna = pk_pos.second;
        plan.clear();
        pathFinding(actual, destino, plan);
        cout << "Generando camino hasta PK: " << pk_index << ' '
             << destino.fila << ' ' << destino.columna << ' ' << plan.size()
             << endl;
        alcanzando_pk = true;
      } else if (plan.empty())
        plan = generar_plan_aleatorio();
    }
  }

  // Si se cambia el objetivo y hay punto de referencia, crear un nuevo plan.
  if (pk_encontrado && (destino.fila != sensores.destinoF ||
                        destino.columna != sensores.destinoC)) {
    destino.fila = sensores.destinoF;
    destino.columna = sensores.destinoC;

    cout << "Asigado destino: " << destino.fila << ' ' << destino.columna
         << endl;

    if (actual_valida) {
      plan.clear();
      pathFinding(actual, destino, plan);
      VisualizaPlan(actual, plan);
    }
  }

  cout << "Actual: " << actual.fila << ' ' << actual.columna << ' '
       << actual.orientacion << endl;

  // Si se encuentra un aldeano.
  bool aldeano = superficie[2] == 'a';
  bool posible_colision = !plan.empty() && plan.front() == actFORWARD &&
                          (aldeano || celda_bloqueada(terreno[2]));

  if (posible_colision) {
    cout << "Evitando colision" << endl;
    if ((pk_encontrado || alcanzando_pk) && aldeano) {
      auto celda_aldeano = proyectar_vector(actual.orientacion, 2);
      cout << "Aldeano encontrado: " << celda_aldeano.first << ' '
        << celda_aldeano.second << endl;
      mapa_aldeanos[celda_aldeano.first][celda_aldeano.second] = true;
      list<Action> plan_alternativo;
      pathFinding(actual, destino, plan_alternativo);
      int diferencia = plan_alternativo.size() - plan.size();
      // Usar nuevo plan
      if (!plan_alternativo.empty() &&
          (diferencia < MAX_ESPERA || esperando >= MAX_ESPERA)) {
        cout << "Usando plan alternativo" << endl;
        esperando = 0;
        plan = plan_alternativo;
        VisualizaPlan(actual, plan);
      } else {
        esperando++;
        cout << "Esperando" << endl;
        return actIDLE;
      }
    } else if(!plan.empty())
      recalcular_plan();
  }

  // Aplicar acciones del plan actual.
  if (!plan.empty()) {
    auto accion = plan.front();
    plan.pop_front();
    actual = aplicar_accion(actual, accion);
    return accion;
  } else
    return actIDLE;
}

int ComportamientoJugador::interact(Action accion, int valor) { return false; }

// Funcion heuristica.
int distancia(int f1, int c1, int f2, int c2) {
  return abs(f1 - f2) + abs(c1 - c2);
}

bool ComportamientoJugador::celda_bloqueada(char contenido) const {
  return contenido == 'M' || contenido == 'A' || contenido == 'P' ||
         contenido == 'B';
}

bool ComportamientoJugador::celda_bloqueada(int fila, int columna) const {
  char contenido = mapaResultado[fila][columna];
  return mapa_aldeanos[fila][columna] == true || celda_bloqueada(contenido);
}

bool ComportamientoJugador::celda_valida(int fila, int columna) const {
  int max = mapaResultado.size();
  return (fila > 0 && fila < max) && (columna > 0 && columna < max);
}

pair<int, int>
ComportamientoJugador::orientacion_a_movimiento(int orientacion, int fila,
                                                int columna) const {
  if (orientacion == 0)
    fila--;
  if (orientacion == 1)
    columna++;
  if (orientacion == 2)
    fila++;
  if (orientacion == 3)
    columna--;

  return make_pair(fila, columna);
}

list<estado>
ComportamientoJugador::algoritmo_a_star(const estado &origen,
                                        const estado &destino) const {
  if (origen == destino || !celda_valida(destino.fila, destino.columna))
    return list<estado>();

  bool encontrado = false;

  vector<bitset<100>> cerrados(TAM_MAPA);
  vector<vector<estado>> matriz_estados(TAM_MAPA);
  list<estado> abiertos;

  // Incicializa la matriz de estados
  for (auto i = 0; i < TAM_MAPA; i++) {
    matriz_estados[i].resize(TAM_MAPA);
    for (auto j = 0; j < TAM_MAPA; j++) {
      matriz_estados[i][j].f = INFINITY;
      matriz_estados[i][j].fila = i;
      matriz_estados[i][j].columna = j;
    }
  }

  // Inicializa el estado origen en la matriz.
  matriz_estados[origen.fila][origen.columna].f = 0;
  matriz_estados[origen.fila][origen.columna].g = 0;
  matriz_estados[origen.fila][origen.columna].orientacion = origen.orientacion;

  // Añadir el origen a la lista de abiertos.
  abiertos.push_back(origen);

  while (!abiertos.empty() && !encontrado) {
    auto q = std::min_element(
        abiertos.begin(), abiertos.end(),
        [](const estado &e1, const estado &e2) { return e1.f < e2.f; });

    abiertos.erase(q);

    auto q_obj = matriz_estados[q->fila][q->columna];

    // Generar estados descencientes. 4, uno para cada dirección.
    for (int orientacion = 0; orientacion < 4; orientacion++) {
      auto n_pos =
          orientacion_a_movimiento(orientacion, q_obj.fila, q_obj.columna);
      int n_fil = n_pos.first;
      int n_col = n_pos.second;

      if (!celda_valida(n_fil, n_col))
        break;

      // Si es el destino, dejar de buscar.
      if (n_fil == destino.fila && n_col == destino.columna) {
        cout << "Camino encontrado" << endl;
        // Establecer la orientacion y el padre del estado final (destino).
        estado descendiente(n_fil, n_col, 0, 0, q_obj.fila, q_obj.columna,
                            orientacion);
        matriz_estados[n_fil][n_col] = descendiente;
        encontrado = true;
        break;
      }

      if (!cerrados[n_fil][n_col] && !celda_bloqueada(n_fil, n_col)) {
        int nuevo_g = q_obj.g + 1;
        int nuevo_h = distancia(q_obj.fila, q_obj.columna, n_fil, n_col);
        int nuevo_f = nuevo_g + nuevo_h;

        // Si el nuevo estado tiene un costo menor que el anterior estado de la
        // misma posicion, añadir a la lista de abiertos y sustituir en la
        // matriz.
        if (matriz_estados[n_fil][n_col].f > nuevo_f) {
          estado descendiente(n_fil, n_col, nuevo_f, nuevo_g, q_obj.fila,
                              q_obj.columna, orientacion);
          matriz_estados[n_fil][n_col] = descendiente;
          abiertos.push_back(descendiente);
        }
      }
    }
    // Añadir el padre a la lista de cerrados.
    cerrados[q_obj.fila][q_obj.columna] = true;
  }

  list<estado> lista;

  // Generar la lista de estados a partir del estado final.
  if (encontrado) {
    auto e = &matriz_estados[destino.fila][destino.columna];
    while (celda_valida(e->padre_f, e->padre_c)) {
      lista.push_back(*e);
      e = &matriz_estados[e->padre_f][e->padre_c];
    }
    lista.push_back(origen);
    lista.reverse();
  }

  return lista;
}
