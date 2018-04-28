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

bool ComportamientoJugador::pathFinding(const estado &origen,
                                        const estado &destino,
                                        list<Action> &plan) const {
  list<estado> camino = busqueda_a_estrella(origen, destino);

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

list<Action> ComportamientoJugador::generar_plan_aleatorio() const {
  estado d;
  srand(time(NULL));

  while(!celda_valida(d.fila, d.columna) || !celda_permitida(d.fila, d.columna)) {
    d.fila = rand() % TAM_MAPA;
    d.columna = rand() % TAM_MAPA;
  }

  list<Action> p;
  pathFinding(actual, d, p);
  return p;
}

void ComportamientoJugador::recalcular_plan() {
   plan.clear();
   hayPlan = pathFinding(actual, destino, plan);
}

Action ComportamientoJugador::think(Sensores sensores) {
  // static const int POS_POR_DEFECTO = TAM_MAPA / 2;
  static bool pk_encontrado = false;
  static bool alcanzando_pk = false;
  static bool actual_valida = false;
  Action sigAccion = actIDLE;
  bool obstaculo = false;
  cout << "*************SENSORES***********\n" 
  << sensores.mensajeF << '\t' << sensores.mensajeC << endl;
  cout << "*************DESTINO-SENSOR***********\n" 
  << sensores.destinoF << '\t' << sensores.destinoC << endl;
  cout << "*************DESTINO***********\n" 
  << destino.fila << '\t' << destino.columna << endl;
  cout << "*************ACTUAL***********\n"
  << actual.fila << '\t' << actual.columna << endl; 
  
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

      auto terreno = sensores.terreno;
      auto superficie = sensores.superficie;

      // reconstruir_terreno(terreno);

    // Inicializar estado actual.
      if (sensores.mensajeF != -1) {
        actual.fila = sensores.mensajeF;
        actual.columna = sensores.mensajeC;
        actual_valida = true;
      }else if(!pk_encontrado && !actual_valida){    //No tenemos una posicion de inicio valida --> Buscamos PK
        if (terreno[0] == 'K') {
          actual_valida = true;
          pk_encontrado = true;
          alcanzando_pk = false;
          actual.fila = sensores.mensajeF;
          actual.columna = sensores.mensajeC;
        } else{
          srand(time(NULL));
          if(celda_permitida(terreno[2])) sigAccion = actFORWARD;
          else {
            int accion = rand() % 2;
            if (accion) sigAccion = actTURN_L;
            else sigAccion = actTURN_R;
          }
        }
      }

      actual_valida = celda_valida(actual.fila, actual.columna);
    // if (celda_valida(sensores.mensajeF, sensores.mensajeC) &&
      //     actual_valida == false) {
      //   actual.fila = sensores.mensajeF;
      //   actual.columna = sensores.mensajeC;
      //   actual_valida = true;
      //   pk_encontrado = true;
      // } 
      // //MAL no se puede poner asignar una posicion aleatoria
      // else if (actual_valida == false) {
      //   actual.fila = POS_POR_DEFECTO;
      //   actual.columna = POS_POR_DEFECTO;
      // }

      // Busca el Punto de Referencia
      // if (pk_encontrado == false) {
      //   if (terreno[0] == 'K') {
      //     actual_valida = true;
      //     pk_encontrado = true;
      //     alcanzando_pk = false;
      //     actual.fila = sensores.mensajeF;
      //     actual.columna = sensores.mensajeC;
      //     proyectar_mapa_temporal(POS_POR_DEFECTO, POS_POR_DEFECTO, actual.fila,
      //                             actual.columna);
      //     cout << "Punto de referencia encontrado: " << sensores.mensajeF << " "
      //          << sensores.mensajeC << endl;
      //   } else {
      //     auto pk = std::find(terreno.begin(), terreno.end(), 'K');
      //     if (!alcanzando_pk && pk != terreno.end()) {
      //       int pk_index = pk - terreno.begin();
      //       auto pk_pos = proyectar_vector(actual.orientacion, pk_index);
      //       destino.fila = pk_pos.first;
      //       destino.columna = pk_pos.second;
      //       plan.clear();
      //       pathFinding(actual, destino, plan);
      //       cout << "Generando camino hasta PK: " << pk_index << ' '
      //            << destino.fila << ' ' << destino.columna << ' ' << plan.size()
      //            << endl;
      //       alcanzando_pk = true;
      //     } else if (plan.empty())
      //       plan = generar_plan_aleatorio();
      //   }
      // }

    // Si se cambia el objetivo y hay punto de referencia, crear un nuevo
      // plan.
      if ((pk_encontrado or actual_valida) and (destino.fila != sensores.destinoF or
                            destino.columna != sensores.destinoC)) {
        destino.fila = sensores.destinoF;
        destino.columna = sensores.destinoC;

        cout << "Nuevo destino: " << destino.fila << ' ' << destino.columna
             << endl;
          plan.clear();
          for(auto i =0; i<mapaResultado.size();i++){
            for(auto j =0; j<mapaResultado.size();j++){
                char contenido = mapaResultado[i][j];
                cout << contenido  << " ";
            }
            cout << endl;
          }
  cout << "*************DESTINO***********\n" 
  << destino.fila << '\t' << destino.columna << endl;
  cout << "*************ACTUAL***********\n"
  << actual.fila << '\t' << actual.columna << endl; 
          hayPlan = pathFinding(actual, destino, plan);
      }
    //Posicion actual del personaje
      cout << "Posicion: " << actual.fila << ' ' << actual.columna << ' '
           << actual.orientacion << endl;

    // Si nos encontramos con un aldeano nos quedamos quietos hasta que se
    // vaya
      if (superficie[2] == 'a') {
        sigAccion = actIDLE;
        obstaculo = true;
      }

    // Ejecutar el plan
      if (hayPlan and plan.size() > 0 and !obstaculo) {
        PintaPlan(plan);
        VisualizaPlan(actual, plan);
        sigAccion = plan.front();
        plan.erase(plan.begin());
      }

      ultimaAccion = sigAccion;
      return sigAccion;
}

int ComportamientoJugador::interact(Action accion, int valor) { return false; }

bool ComportamientoJugador::celda_permitida(char contenidoCasilla) const {
  return contenidoCasilla == 'S' or contenidoCasilla == 'T' or
         contenidoCasilla == 'K' or contenidoCasilla == '?';
}

bool ComportamientoJugador::celda_permitida(int fila, int columna) const {
  char contenido = mapaResultado[fila][columna];
  return celda_valida(fila,columna) and celda_permitida(contenido);
}

bool ComportamientoJugador::celda_valida(int fila, int columna) const {
  int max = mapaResultado.size();
  return (fila > 0 && fila < max) && (columna > 0 && columna < max);
}

estado mejorNodo(list<estado> abiertos){
  auto it = abiertos.begin();
  estado elegido = *it;
  while (++it != abiertos.end())
    if (it->f < elegido.f)
      elegido = *it;
  return elegido;
}

list<estado>
ComportamientoJugador::busqueda_a_estrella(const estado &origen,
                                        const estado &destino) const {
  bool encontrado = false;

  int TAM = mapaResultado.size();
  vector<vector<bool>> cerrados(TAM, vector<bool>(TAM, false));
  vector<vector<estado>> estados_celda(TAM, vector<estado>(TAM));
  list<estado> abiertos;
  int df[4] = {-1, 0, 1, 0};  // incremento en coordenada x
  int dc[4] = {0, 1, 0, -1};  // incremento en coordanada y

  for (auto i = 0; i < TAM; i++) {
    for (auto j = 0; j < TAM; j++) {
      estados_celda[i][j].f = INFINITY;
      estados_celda[i][j].fila = i;
      estados_celda[i][j].columna = j;
    }
  }

  // Inicializa el estado origen en la matriz.
  estados_celda[origen.fila][origen.columna].f = 0;
  estados_celda[origen.fila][origen.columna].h = 0;
  estados_celda[origen.fila][origen.columna].g = 0;
  estados_celda[origen.fila][origen.columna].orientacion = origen.orientacion;

  // Añadir el origen a la lista de abiertos.
  abiertos.push_back(origen);

  while (!abiertos.empty() && !encontrado) {
    estado q = mejorNodo(abiertos);

    abiertos.remove(q);

    auto q_obj = estados_celda[q.fila][q.columna];

    for (int i = 0; i < 4; i++) {
      int n_fil = df[i] + q_obj.fila;
      int n_col = dc[i] + q_obj.columna;

      // Si es el destino, dejar de buscar.
      if (n_fil == destino.fila && n_col == destino.columna) {
        cout << "Camino encontrado" << endl;
        // Establecer informacion estado final.
        estados_celda[n_fil][n_col] = estado(n_fil, n_col, 0, 0, 0, q_obj.fila, q_obj.columna, i);
        encontrado = true;
        break;
      }

      if (!cerrados[n_fil][n_col] && celda_permitida(n_fil, n_col)) {
        int n_g = q_obj.g + 1;
        int n_h = abs(q_obj.fila - n_fil) +  abs(q_obj.columna - n_col);
        int n_f = n_g + n_h;

        //  añadir a la lista de abiertos y actualizar valor de f si tiene un coste menor 
        if (estados_celda[n_fil][n_col].f > n_f) {
          estado hijo(n_fil, n_col, n_f, n_g, n_h, q_obj.fila,
                              q_obj.columna, i);
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
