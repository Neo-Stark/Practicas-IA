
#include <iostream>
#include "Bot.h"
#include "SimulatorLink.h"

#include "VictoryRoyale.h"

using namespace std;

int main() {
  SimulatorLink sim;  // Enlace con el simulador
  Bot *bot = 0;       // Bot que se ejecutará en el simulador

  VictoryRoyale *rb = new VictoryRoyale();

  bot = (Bot *)rb;

  // Se establece el bot del alumno como bot del simulador
  sim.setBot(bot);

  // Ejecutamos la simulación
  bool salida = sim.run();

  delete rb;

  // Ejecutamos la simulación
  if (!salida) {
    cerr << "\n\n\tSIMULACION ABORTADA.\n\n";
  } else {
    cerr << "\n\n\tFIN DE LA PARTIDA CON EXITO.\n\n";
  }

  return 0;
}
