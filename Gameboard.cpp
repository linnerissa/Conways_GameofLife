#include <iostream>
#include <stdint.h>
#include <assert.h>
#include <boost/functional/hash_fwd.hpp> 
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

#define MAGICNUMBER 3
#define ON 1
#define OFF -1

class Point{
	public:
  		std::int64_t x;
  		std::int64_t y;

		Point(): x(0), y(0) {}
		Point(int x, int y) : x(x), y(y) {}

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
	std::unordered_set<Point> currentlyOn;
	std::unordered_map<Point, int> neighborCount;
	std::unordered_map<Point, int> changelist;

	std::unordered_set<Point> deltaNeighbors = {
		Point(-1, -1), Point(0, -1), Point(1, -1),
		Point(-1, 0), Point(1, 0),
		Point(-1, 1), Point(0, 1), Point(1, 1)};

	int currentlyOnSetter(std::unordered_set<Point> x){
		currentlyOn = x;
		return 0;
	}

	int neighborCountSetter(std::unordered_map<Point, int> x){
		neighborCount = x;
		return 0;
	}

	int changelistSetter(std::unordered_map<Point, int> x){
		changelist = x;
		return 0;
	}


	/*Function ToggleBit(Coord, Off/On):
		takes a coordnate and turns its neighbors on or off
			add any "Changed state" neighbors into the changelist
		add it to currently on coordinate list(or remove it)
	*/
	int toggleBit(Point p, int action){
		//Remeber to do boundary checking on the x and y's for overflow
		for (const Point& delta: deltaNeighbors) {
			int64_t newX = p.x + delta.x;
			int64_t newY = p.y + delta.y;
			Point newP = Point(newX, newY);
			
			if (neighborCount.find(newP) != neighborCount.end()){
				//Remove from changelist and or add to changelist as off:
				if (neighborCount.at(newP) == MAGICNUMBER){

					changelist.emplace(newP, OFF);
					neighborCount.at(newP) += action;

				} else {
					neighborCount.at(newP) += action;
					if (neighborCount.at(newP) == 3){
						
						if (changelist.find(newP) != changelist.end()){
							if (changelist.at(newP) == OFF){
								changelist.erase(newP);
							}
						}

						if (currentlyOn.find(newP) == currentlyOn.end()){
							//not already on, need to make changes to it
							changelist.emplace(newP, ON);
						}

					} else if (neighborCount.at(newP) == 0){
						neighborCount.erase(newP);
					}
				}

			} else {
				//Doesnt exist in neighborCount yet:
				assert(action != OFF);
				neighborCount.emplace(newP, 1);
			}
		}
		currentlyOn.emplace(p);
	}

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
	*/
	Gameboard * testGameboard = new Gameboard();
	testGameboard->toggleBit(Point(1, 2), ON);
	testGameboard->toggleBit(Point(2, 2), ON);
	testGameboard->toggleBit(Point(3, 2), ON);
	std::cout << "My neighbors look like: \n";
	for(const auto& neighbor: testGameboard->neighborCount){
		Point point = neighbor.first;
		int count = neighbor.second;
		std::cout << point.x;
		std::cout << ",";
		std::cout << point.y;
		std::cout << "     ";
		std::cout << count;
		std::cout <<"\n";
	}

	std::cout << "Currently on includes: \n";
	for(const auto& on: testGameboard->currentlyOn){
		std::cout<<on.x << "," << on.y <<"\n";
	}

	std::cout<< "Changelist looks like: \n";
	for(const auto& change: testGameboard->changelist){
		Point point = change.first;
		int action = change.second;
		std::cout << point.x <<"," << point.y << "      " << action<<"\n";
	}
}

