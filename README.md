# Nine Men's Morris with Monte Carlo Tree Search (MCTS)

To be precise, this is an engine that supports various board games and different algorithms. The main focus was to create a Monte Carlo Tree Search agent for Nine Men's Morris. There is also a UI made in Electron using C++ node modules to view the game and play against the algorithm.

The MCTS used in this game employs root parallelization and a voting system. In fact, with better hardware, it performs very well against Alpha-Beta pruning without changing the algorithm. MCTS proves to be highly scalable, either by "thinking" time or by scaling the hardware. However, to further improve the algorithm, I believe a better strategy would be to use the AlphaZero algorithm or incorporate some heuristic for selecting the next move.

I have also experimented with other parallelization variants such as tree or leaf parallelization, which are more complex. Surprisingly, they performed worse than the much simpler root parallelization. Although there is a variant of tree parallelization with virtual loss and no locks that may yield better results, I believe that the simplicity of root parallelization and exploring a different direction is a better idea.
