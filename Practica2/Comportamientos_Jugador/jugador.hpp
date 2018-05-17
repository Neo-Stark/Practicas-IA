#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <bitset>
#include <list>

struct estado {
  int fila;
  int columna;
  int padre_f, padre_c;
  int orientacion;
  double f, g, h;

  estado(int fil, int col, double f = 0, double g = 0, double h = 0,
         int p_fila = -1, int p_col = -1, int orientacion = 0)
      : fila(fil),
        columna(col),
        padre_f(p_fila),
        padre_c(p_col),
        orientacion(orientacion),
        f(f),
        g(g),
        h(h) {}

  estado()
      : fila(-1),
        columna(-1),
        padre_f(-1),
        padre_c(-1),
        orientacion(0),
        f(0),
        g(0),
        h(0) {}

  bool operator==(const estado &cmp) const {
    return fila == cmp.fila && columna == cmp.columna;
  }
};

class ComportamientoJugador : public Comportamiento {
 public:
  ComportamientoJugador(unsigned int size) : Comportamiento(size) {
    // Inicializar Variables de Estado
    fil = col = 99;
    brujula = 0;  // 0: Norte, 1:Este, 2:Sur, 3:Oeste
    actual.orientacion = 0;
    destino.fila = -1;
    destino.columna = -1;
    destino.orientacion = -1;
    hayPlan = false;
  }

  ComportamientoJugador(vector<vector<unsigned char>> mapaR)
      : Comportamiento(mapaR) {
    // Inicializar Variables de Estado
    fil = col = 99;
    brujula = 0;  // 0: Norte, 1:Este, 2:Sur, 3:Oeste
    actual.orientacion = 0;
    destino.fila = -1;
    destino.columna = -1;
    destino.orientacion = -1;
    hayPlan = false;
  }

  ComportamientoJugador(const ComportamientoJugador &comport)
      : Comportamiento(comport) {}

  ~ComportamientoJugador() {}
  Action think(Sensores sensores);
  int interact(Action accion, int valor);
  void VisualizaPlan(const estado &st, const list<Action> &plan);
  ComportamientoJugador *clone() { return new ComportamientoJugador(*this); }

 private:
  int fil, col, brujula;
  estado destino, actual;
  list<Action> plan;
  Action ultimaAccion;
  bool hayPlan = false;

  void PintaPlan(list<Action> plan);
  bool pathFinding(const estado &origen, const estado &destino,
                   list<Action> &plan) const;

  list<estado> busqueda_a_estrella(const estado &origen,
                                   const estado &destino) const;
  bool celda_valida(int fila, int columna) const;

  bool celda_permitida(int fila, int columna) const;

  bool celda_permitida(char) const;

  pair<int, int> obtener_celda(int orientacion, int pos) const;

  void reconstruir_mapa(vector<unsigned char> terreno);
  void pinta_mapa(vector<unsigned char> terreno);
  void recalcular_plan();
  list<Action> llegar_PK(int pos);
};
#endif
