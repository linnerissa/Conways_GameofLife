#include <iostream>
#include <stdint.h>
#include <assert.h>
#include <boost/functional/hash_fwd.hpp> 
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <math.h>
#include <limits.h>

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
		std::unordered_set<Point> neighborPoints = neighbors(p);
		for (const Point& newPoint: neighborPoints) {
			int64_t newX = newPoint.x;
			int64_t newY = newPoint.y;
			
			if (neighborCount.find(newPoint) != neighborCount.end()){
				//Remove from changelist and or add to changelist as off:
				if (neighborCount.at(newPoint) == MAGICNUMBER){

					if (changelist.find(newPoint) != changelist.end()){
						changelist.erase(newPoint);
					} else if (currentlyOn.find(newPoint) != currentlyOn.end()){
						changelist[newPoint] = OFF;
					} else {
						//Should not have a case where its neither on changelist or on currentlyon
						cout<<"ERROR\n";
					}

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

	void nextState(){
		std::unordered_map<Point, int> localCopy = changelist;
		changelist.clear();

		for (const auto& change: changelist){
			Point point = change.first;
			int action = change.second;
			toggleBit(point, action);
		}
		for (const Point& on: currentlyOn){
			if (neighborCount[on] != 3){
				changelist[on] = OFF;
			}
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
	Gameboard * testGameboard = new Gameboard();
	testGameboard->toggleBit(Point(1, 2), ON);
	testGameboard->toggleBit(Point(2, 2), ON);
	testGameboard->toggleBit(Point(3, 2), ON);
	// testGameboard->toggleBit(Point(3, 2), OFF);
	// testGameboard->toggleBit(Point(2, 2), OFF);
	// testGameboard->toggleBit(Point(1, 2), OFF);

	// testGameboard->toggleBit(Point(LLONG_MAX, LLONG_MAX), ON);
	// testGameboard->toggleBit(Point(LLONG_MIN, LLONG_MIN), ON);
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

	testGameboard->nextState();
}

