# MEMORIA

## Heuristica 1

  Mejor puntuacion para el jugador actual

## Heuristica 2

  Diferencia de puntuacion entre nuestro jugador y rival

~~~ c++
int VictoryRoyale::heuristica(const GameState &state) {
  int score;
  if (jugador == J1)
    score = state.getScore((Player)0) - state.getScore((Player)1);
  else
    score = state.getScore((Player)1) - state.getScore((Player)0);
  return score;
}
~~~

### Mejor contra GreedyBot

= **Heuristica 2**