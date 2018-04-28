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
  ComportamientoJugador(unsigned int size)
      : Comportamiento(size), TAM_MAPA(size) {
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
      : Comportamiento(mapaR), TAM_MAPA(mapaR.size()) {
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
      : Comportamiento(comport), TAM_MAPA(comport.TAM_MAPA) {}

  ~ComportamientoJugador() {}
  Action think(Sensores sensores);
  int interact(Action accion, int valor);
  void VisualizaPlan(const estado &st, const list<Action> &plan);
  ComportamientoJugador *clone() { return new ComportamientoJugador(*this); }

 private:
  int fil, col, brujula;
  estado destino, actual;
  list<Action> plan;
  const int TAM_MAPA;
  Action ultimaAccion;
  bool hayPlan =false;

  void PintaPlan(list<Action> plan);
  bool pathFinding(const estado &origen, const estado &destino,
                   list<Action> &plan) const;

  /*
   * Devuelve la sucesion optima de estados (celdas) desde el origen hasta el
   * destino
   */
  list<estado> busqueda_a_estrella(const estado &origen,
                                const estado &destino) const;
  /*
   * Devuelve true si la celda se encuentra dentro de los limites del mapa
   */
  bool celda_valida(int fila, int columna) const;

  /*
   * Devuelve true si la celda no es transitable.
   */
  bool celda_permitida(int fila, int columna) const;

  /*
   * Devuelve true si la celda no es transitable (por contenido).
   */
  bool celda_permitida(char contenido) const;

  /*
   * Devuelve la posicion resultante si se avanzase una casilla
   * dependiendo de la orientacion de la entidad
   */
  pair<int, int> orientacion_a_movimiento(int orientacion, int fila,
                                          int columna) const;

  /*
   * Devuelve el estado resultado de aplicar la accion a e.
   */
  estado aplicar_accion(const estado &e, Action a) const;

  /*
   * Devuelve la posicion real de una celda de
   * los sensores del jugador.
   */
  pair<int, int> proyectar_vector(int orientacion, int pos) const;

  /*
   * Resetea la matriz de aldeanos
   */
  void limpiar_matriz_aldeanos();

  /*
   * Translada el contenido del mapa para ajustarlo a la posicion real
   */
  void proyectar_mapa_temporal(int fila_inicial, int columna_inicial,
                               int fila_actual, int columna_actual);

  /*
   * Añade los datos del sensor al mapa
   */
  void reconstruir_terreno(vector<unsigned char> terreno);
  void pinta_mapa(vector<unsigned char> terreno);

  /*
   * Genera un plan cuyo origen es el del jugador actual y un destino
   * aleatorio.
   */
  list<Action> generar_plan_aleatorio() const;
  void recalcular_plan();
};
#endif
