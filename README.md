# Conway's Game of Life

Simple game - Rules are as follows:
 - Cells with three alive neighbors spring to life
 - Cells with two or three alive neighbors may stay alive if already alive
 - Cells with more than three alive neighbors die
 - Cells with less than two alive neighbors also die
 
Implemented in C++

##Fuctionality:
Reliant on CLI with the first argument passed in being some positive number of cycles or -1 if infinite
An arbitrary number of points may be passed in following the first argument, using the following format:
` x1,y1 x2,y2 etc.`

An example call from command line with 10 iterations:
` ./gameboard 10 1,1 1,2 1,3`

Currently, it prints the points that are on at each stage, kind of hard to tell the state of the gameboard unless purposely drawn out. Seems to defeat the purpose of the game if you can't watch it. Would like to get around to some implementation of graphics

##To-do:
- Implement it graphically to visually examine the game (implement in toggle bits where it also does the work of coloring squares)
- Replace alive neighbors with a picture of minions
- Implement a graphical method to input IE select squares, these will light up as your starting grid and initialize the grid

##Design Idea:
We're creating the game on a massive gameboard where any 64 bit int is game with wrap around on the edges and such. To be able to accomodate for this, theres almost little way in which typical implementations iterating through each square of the board will produce an efficiently runable program unless you would like to sit around and watch paint dry. Noticing that the gameboard matrix is 2^64 * 2^64, majority of the gameboard is going to be dark the whole time, giving us a sparse matrix. We also maintain state between stages of the game. This leads to the idea the storing some in hashmaps and hashsets seem to be efficient and a good idea.

We hash the currently on bits. Accessing them in a hashmap is O(1) given a good hash (I hope I made a good hash :D) and O(n) if you have a dumb hash. We keep a state of grid to # of alive neighbors, as well as a list of the changes that need to be made in the next iteration.

Changelist was particularily annoying because the toggle function updates a bit at a time but changelist would alter with every bit that was being toggled. Also, it made order important in some of the comparisons as evident in my earlier code. Realized this was horrible and made the game break and act all weird so I had to keep track of like, the set of toggling that was occuring.
