#pragma once

#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
using namespace std;

double randRange(double min, double max);

enum QuadType { QUAD_SQUARE, QUAD_DIAMOND };
enum GenType { DEPTH_FIRST, BREATH_FIRST };

struct GridPoint2D
{
	int x;
	int y;

	GridPoint2D(int x = 0, int y = 0);
	
	GridPoint2D operator+(const GridPoint2D & rhs) const 
	{ return GridPoint2D(x + rhs.x, y + rhs.y); }
	GridPoint2D operator-(const GridPoint2D & rhs) const 
	{ return GridPoint2D(x - rhs.x, y - rhs.y); }
	GridPoint2D operator*(int val) const 
	{ return GridPoint2D(x * val, y * val); }
	GridPoint2D operator/(int val) const 
	{ return GridPoint2D(x / val, y / val); }
	GridPoint2D operator>>(int val) const 
	{ return GridPoint2D(x >> val, y >> val); }
	GridPoint2D operator<<(int val) const 
	{ return GridPoint2D(x << val, y << val); }
	GridPoint2D operator+=(const GridPoint2D & rhs) 
	{ x += rhs.x; y += rhs.y; return *this; }
	GridPoint2D operator-=(const GridPoint2D & rhs) 
	{ x -= rhs.x; y -= rhs.y; return *this; }
	GridPoint2D operator*=(int val)
	{ x *= val; y *= val; return *this; }
	GridPoint2D operator/=(int val)
	{ x /= val; y /= val; return *this; }
	bool operator==(const GridPoint2D & rhs) const
	{ return x == rhs.x && y == rhs.y; }
	bool operator!=(const GridPoint2D & rhs) const
	{ return x != rhs.x || y != rhs.y; }

	friend ostream& operator<<(ostream& out, const GridPoint2D & val);
};
ostream& operator<<(ostream& out, const GridPoint2D & val);

struct GridQuad2D
{
	GridPoint2D p1;
	GridPoint2D p2;
	GridPoint2D p3; 
	GridPoint2D p4; 
	QuadType type;

	GridQuad2D();
	GridQuad2D(const GridPoint2D & p1, const GridPoint2D & p2, 
		const GridPoint2D & p3, const GridPoint2D & p4, QuadType type = QUAD_SQUARE);

	int width() const;
	int height() const;
	GridPoint2D midpoint() const;
	
	friend ostream& operator<<(ostream& out, const GridQuad2D & val);
};
ostream& operator<<(ostream& out, const GridQuad2D & val);

class GridGenerator2D
{
private:
	int n; // Determines width and height in powers of 2
	int c; // Tracks n in powers of 2
	int size;
	
	double minValue;
	double maxValue;
	double initialRate;
	double decayRate;
	double jumpValue;

	GenType type;
	vector< vector< double > > map;
	vector< vector< bool > > visited;
	list< GridQuad2D > quads;
	list< GridQuad2D > nquads;
public:
	// Parameters:
	// n - the size of the map, the size will be 2^n + 1
	// initialRate - Determines the initial random movement jump rate where 1.00 will lead to a full range jump
	//		the higher this value is, the higher the decayRate needs to be for smoother generations.
	//		Higher values tend for more initial diversity.
	// decayRate - The speed at which the jump rate decays to no movement
	//		the higher this value is, the higher the initialRate needs to be for smoother generations.
	//		
	GridGenerator2D();
	GridGenerator2D(int n, double initialRate, double decayRate, GenType type = BREATH_FIRST);

	double getMinValue() const { return minValue; }
	double getMaxValue() const { return maxValue; }
	
	// Initialize all variables
	void init(int n, double initialRate, double decayRate, GenType type = BREATH_FIRST);
	// Reset all variables
	void reset();
	// Randomly initialize the value at point p
	void initValue(const GridPoint2D & p);
	// Sets the value at point p
	void setValue(const GridPoint2D & p, double value);
	// Adds more value at point p
	void addValue(const GridPoint2D & p, double value);
	// Retrieves the average from a provided quadrilateral
	double getAverage(const GridQuad2D & quad) const;
	// Restricts the value at point p between minValue and maxValue
	void boundValue(const GridPoint2D & p);
	// Retrieves the value at point p
	double getValue(const GridPoint2D & p) const;
	// Determines whether point p is visited already
	bool isSet(GridPoint2D & p) const;
	// Adds the quad to the list to visit if the midpoint is not visited yet
	bool pushOn(const GridQuad2D & quad);
	// Process the quad setting the value at the midpoint and generating new ones
	int processQuad(const GridQuad2D & quad);
	// Process one phase of the diamond-square algorithm
	bool processPhase();
	// Runs the entire diamond-square algorithm
	void process();
	// Returns the map populated with values
	vector< vector< double > > retrieveMap() const;
};