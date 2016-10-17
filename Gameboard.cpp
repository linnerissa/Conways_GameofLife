#include <iostream>
#include <stdint.h>
#include <assert.h>
#include <boost/functional/hash_fwd.hpp> 
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <math.h>
#include <limits.h>
#include <sstream>


using namespace std;

#define MAGICNUMBER 3
#define ON 1
#define OFF -1

class Point{
	public:
  		std::int64_t x;
  		std::int64_t y;

		Point(): x(0), y(0) {}
		Point(int64_t x, int64_t y) : x(x), y(y) {}

	bool operator==(Point const& other) const{ 
		return (x == other.x && y == other.y);
	}
};

namespace std {

  	template <>
  	struct hash<Point>{
	  	std::size_t operator()(const Point& p) const{
	    	std::size_t seed = std::hash<long long>{}(p.x);
	      	return std::hash<long long>{}(p.y) + 0x9e3779b9 + (seed <<6) + (seed >>2);
	    }
	};
};	

class Gameboard{
	//All coordinates must be listed as int64_t

	/* Need following state variables:
			CurrentlyOnList: {(x1, y1), (x2, y2), (x3, y3) etc. }
				*Needs to be easy to look through and iterate: hashset?
			NeighborStateList: {(x1,y1): 3 ; (x2,y2): 2 etc.}
				*Mapping from coordinate to the number of neighbors on
				*Needs to have REALLY quick access time
			Changelist: {(x1, y1): 1(on); (x2, y2): -1(off)}
				*Holds the next state's changes that need to be made
				*1 is on, -1 is off
	*/
	public:
	Gameboard(std::unordered_set<Point> startingPoints){
		for (const Point& point: startingPoints) {
			changelist[point] = ON;
		}
	}

	private:
	std::unordered_set<Point> currentlyOn;
	std::unordered_map<Point, int> neighborCount;
	std::unordered_map<Point, int> changelist;

	std::unordered_set<Point> deltaNeighbors = {
		Point(-1, -1), Point(0, -1), Point(1, -1),
		Point(-1, 0), Point(1, 0),
		Point(-1, 1), Point(0, 1), Point(1, 1)};


	/*Function ToggleBit(Coord, Off/On):
		takes a coordnate and turns its neighbors on or off
			add any "Changed state" neighbors into the changelist
		add it to currently on coordinate list(or remove it)
	*/
	private:
	int toggleBit(Point p, int action){
		//Remeber to do boundary checking on the x and y's for overflow
		std::unordered_set<Point> neighborPoints = neighbors(p);
		for (const Point& newPoint: neighborPoints) {
			int64_t newX = newPoint.x;
			int64_t newY = newPoint.y;
			
			if (neighborCount.find(newPoint) != neighborCount.end()){
				//Remove from changelist and or add to changelist as off:
				if (neighborCount.at(newPoint) == MAGICNUMBER){

					if (changelist.find(newPoint) != changelist.end()){
						changelist.erase(newPoint);
					} 

					//else {
						//This is the case where the changelist is empty because we are toggling bits to match the new state, 
						//If we toggle off a bit, we cant just remove the bits that are neighboring and need to be toggled on
						//So we can't remove it from changelist, its also not a bit that's currentlyOn since we are toggling it off
						//Taken care of near end of this function
					//}

					neighborCount.at(newPoint) += action;

				} else {
					neighborCount.at(newPoint) += action;
					if (neighborCount.at(newPoint) == 3){
						
						if (currentlyOn.find(newPoint) != currentlyOn.end()){
							//IS CURRENTLY ON
							if (changelist.at(newPoint) == OFF){
								changelist.erase(newPoint);
							}
						}

						if (currentlyOn.find(newPoint) == currentlyOn.end()){
							//not already on, need to make changes to it
							changelist[newPoint] =  ON;
						}

					} else if (neighborCount.at(newPoint) == 0){
						neighborCount.erase(newPoint);
					}
				}

			} else {
				//Doesnt exist in neighborCount yet:
				assert(action != OFF);
				neighborCount[newPoint] = 1;
			}
		}

		if (action == ON){
			assert(currentlyOn.find(p) == currentlyOn.end());
			currentlyOn.insert(p);
		} else {
			assert(action == OFF);
			assert(currentlyOn.find(p) != currentlyOn.end());
			currentlyOn.erase(p);
		}
	}

	/*Function Neighbors(Coord):
		return all of a coordinates neighbors -> Take care of the wrapping
		Ex(should I need to round the corner, neighbors will return 
		the coord of the box at the otherside)
	*/
	private:
	std::unordered_set<Point> neighbors(Point p){

		std::unordered_set<Point>neighbors;
		int64_t newX, newY;

		for (const Point& delta: deltaNeighbors){
			if (p.x == LLONG_MAX && delta.x == 1){
				newX = LLONG_MIN;
			} else if (p.x == LLONG_MIN && delta.x == -1){
				newX = LLONG_MAX;
			} else {
				newX = p.x + delta.x;
			}

			if (p.y == LLONG_MAX && delta.y == 1){
				newY = LLONG_MIN;
			} else if (p.y == LLONG_MIN && delta.y == -1){
				newY = LLONG_MAX;
			} else {
				newY = p.y + delta.y;
			}
			bool result = neighbors.insert(Point(newX, newY)).second;
			assert(result);
		}
		return neighbors;
	}
	
	/*Functon NextState()
		Should take all the changelist items, copy it into a local copy to iterate through
		set the class variable to a new empty changelist
		for each item in changelist:
			update as necessary(ie delete from "on list" and decrement neighbors or turn on neighbors
	*/
	public:
	void nextState(){
		for (const Point& on: currentlyOn){
			if (neighborCount[on] != 3 && neighborCount[on] != 2){
				changelist[on] = OFF;
			}
		}
		std::unordered_map<Point, int> localCopy = changelist;
		changelist.clear();

		for (const auto& change: localCopy){
			Point point = change.first;
			int action = change.second;
			toggleBit(point, action);
		}
			

		for (const Point& on: currentlyOn){
			cout << on.x << "," << on.y << endl;
		}
	}
};



int main(int argc, char *argv[]){
	/* flow of main should look like:
		take input pixels that are on
			iterate the list and toggle bit for each as on
			-> produces a changelist for next state and an accurate neighbor state list and current on list
		nextstate()
			iterate through next state lists and turn the off pixels off and on pixels on
			should produce the next changelist etc.
	*/

	if (argc < 2){
		cout<<"Not enough arguments, require an integer, then following with points x,y that are starting points" <<endl;
		return 0;
	}
	int n;
	try{
		std::string str = argv[1];
		if (str.find_first_not_of("0123456789-") != std::string::npos){
			cout<< "Second argument must be number of iterations or -1 if infinite" << endl;
			return 0;
		}

		n = stoi(str);
		if (n != -1 && n < 0){
			cout<< "Second argument must be number of iterations or -1 if infinite" << endl;
			return 0;
		}
	} catch (int e) {
		cout<< "Second argument must be an integer (number of stages) or -1 for infinite" <<endl;
		return 0;
	}

	std::unordered_set<Point> input; 
	for (int i = 2; i < argc; i++){
		std::string s = argv[i];
		std::string delimiter = ",";

		size_t pos = 0;
		std::string token;
		int64_t x, y;
		while((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			x = stoll(token);
			s.erase(0, pos + delimiter.length());
		}
		y = stoll(s);

		Point newPoint = Point(x, y);
		input.emplace(newPoint);
	}
	
	Gameboard* game = new Gameboard(input);

	if (n == -1){
		while (1){
			cout<<"Next State" <<endl;
			game->nextState();
		}

	} else {
		for (int i = 0; i < n; i++){
			cout<<"State " << i + 1<<endl;
			game->nextState();
		}
	}
}

