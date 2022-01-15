# Nine men's morris with Monte Carlo tree search (MCTS)

To be precise this is an engine that supports various board games and different algorithms. Main focus was to make a Monte Carlo tree search agent for Nine men's morris. There is also a UI made in electron using C++ node modules to view the game and play against the algorithm.  

The MCTS used in this game uses root parallelization and uses voting system. In fact without changing the algorithm and just with better hardware it performs very well against Alpha Beta pruning. MCTS proves to be very scalable by either "thinking" time or by scaling the hardware. However, to improve the algorithm further I think better strategy would be to use Alpha Zero algorithm or to use some heuristic for picking next move. 

Also for parallelization I have tried other variants like tree or leaf parallelization which are a lot more complicated. Surprisingly, they performed worse than a much simpler root parallelization. Although, there is a variant of tree parallelization with virtual loss and no locks that may perform better. But I think the simplicity of root parallelization and pursuing a different direction is a better idea. 
