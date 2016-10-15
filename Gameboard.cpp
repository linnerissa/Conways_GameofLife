#include <stdint.h>

using namespace std;

class Gameboard{
	//All coordinates must be listed as int64_t

	/* Need following state variables:
			CurrentlyOnList: {(x1, y1), (x2, y2), (x3, y3) etc. }
				*Needs to be easy to look through and iterate: hashset?
			NeighborStateList: {(x1,y1): 3 ; (x2,y2): 2 etc.}
				*Mapping from coordinate to the number of neighbors on
				*Needs to have REALLY quick access time
			Changelist: {(x1, y1): 1(on); (x2, y2): 0(off)}
				*Holds the next state's changes that need to be made
	*/

	/*Function ToggleBit(Coord, Off/On):
		takes a coordnate and turns its neighbors on or off
			add any "Changed state" neighbors into the changelist
		add it to currently on coordinate list(or remove it)
	*/

	/*Function Neighbors(Coord):
		return all of a coordinates neighbors -> Take care of the wrapping
		Ex(should I need to round the corner, neighbors will return 
		the coord of the box at the otherside)
	*/
	
	/*Functon NextState()
		Should take all the changelist items, copy it into a local copy to iterate through
		set the class variable to a new empty changelist
		for each item in changelist:
			update as necessary(ie delete from "on list" and decrement neighbors or turn on neighbors
	*/


};

int main(int argc, char *argv[]){
	/* flow of main should look like:
		take input pixels that are on
			iterate the list and toggle bit for each as on
			-> produces a changelist for next state and an accurate neighbor state list and current on list
		nextstate()
			iterate through next state lists and turn the off pixels off and on pixels on
			should produce the next changelist etc.
			can insert graphics stage here to print all the currently on pixels
	*/

   return 0;
}

