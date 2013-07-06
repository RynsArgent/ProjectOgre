#include "GridGenerator2D.h"

double randRange(double min, double max)
{
	return static_cast<double>(rand()) / RAND_MAX * (max - min) + min;
}

GridPoint2D::GridPoint2D(int x, int y) : x(x), y(y) 
{}

ostream& operator<<(ostream& out, const GridPoint2D & val)
{
	out << "(" << val.x << "," << val.y << ")";
	return out;
}

GridQuad2D::GridQuad2D()
	: p1(), p2(), p3(), p4(), type(QUAD_SQUARE)
{}

GridQuad2D::GridQuad2D(const GridPoint2D & p1, const GridPoint2D & p2, 
	const GridPoint2D & p3, const GridPoint2D & p4, QuadType type)
	: p1(p1), p2(p2), p3(p3), p4(p4), type(type)
{}

int GridQuad2D::width() const
{
	if (type == QUAD_SQUARE)
		return p2.x - p1.x;
	else
		return p3.x - p1.x;
}

int GridQuad2D::height() const
{
	if (type == QUAD_SQUARE)
		return p3.y - p2.y;
	else
		return p4.y - p2.y;
}

GridPoint2D GridQuad2D::midpoint() const
{
	return (p1 + p2 + p3 + p4) >> 2;
}	

ostream& operator<<(ostream& out, const GridQuad2D & val)
{
	out << val.p1 << " " << val.p2 << " " << val.p3 << " " << val.p4;
	return out;
}

GridGenerator2D::GridGenerator2D()
		: n(n), c(0), size(0), 
	minValue(0.0), maxValue(1.0), initialRate(0.0), decayRate(1.0), jumpValue(0),
	type(BREATH_FIRST), map(), visited(), quads(), nquads()
{
}

GridGenerator2D::GridGenerator2D(int n, double initialRate, double decayRate, GenType type)
	: n(n), c(0), size(0), 
	minValue(0.0), maxValue(1.0), initialRate(initialRate), decayRate(decayRate), jumpValue(0),
	type(type), map(), visited(), quads(), nquads()
{
}

// Initialize all variables
void GridGenerator2D::init(int n, double initialRate, double decayRate, GenType type)
{
	this->n = n;
	this->initialRate = initialRate;
	this->decayRate = decayRate;
	this->type = type;
}

void GridGenerator2D::reset()
{
	map.clear();
	quads.clear();
	nquads.clear();
	visited.clear();

	c = 2 * n;

	if (n <= 0)
		size = 1 + 1;
	else
		size = (2 << (n - 1)) + 1;

	map = vector< vector<double> >(size, vector<double>(size, 0));
	visited = vector< vector<bool> >(size, vector<bool>(size, false));

	// Allow for some overflow at first for variety
	jumpValue = initialRate * (maxValue - minValue);

	GridPoint2D tl = GridPoint2D(0, 0);
	GridPoint2D tr = GridPoint2D(size - 1, 0);
	GridPoint2D br = GridPoint2D(size - 1, size - 1);
	GridPoint2D bl = GridPoint2D(0, size - 1);
	initValue(tl);
	initValue(tr);
	initValue(br);
	initValue(bl);

	if (size > 2)
	{
		if (type == BREATH_FIRST)
			quads.push_back(GridQuad2D(tl, tr, br, bl, QUAD_SQUARE));
		else
			nquads.push_back(GridQuad2D(tl, tr, br, bl, QUAD_SQUARE));
	}
}

void GridGenerator2D::initValue(const GridPoint2D & p)
{
	setValue(p, (minValue + maxValue) / 2 + 2 * randRange(-jumpValue, jumpValue));
}

void GridGenerator2D::setValue(const GridPoint2D & p, double value)
{
	map[p.x][p.y] = value;
	visited[p.x][p.y] = true;
	boundValue(p);
}

void GridGenerator2D::addValue(const GridPoint2D & p, double value)
{
	map[p.x][p.y] += value;
	visited[p.x][p.y] = true;
	boundValue(p);
}

double GridGenerator2D::getAverage(const GridQuad2D & quad) const
{
	int count = 0;
	double average = 0.0;
	if (quad.p1.x >= 0 && quad.p1.x < size && quad.p1.y >= 0 && quad.p1.y < size)
	{
		average += getValue(quad.p1);
		++count;
	}
	if (quad.p2.x >= 0 && quad.p2.x < size && quad.p2.y >= 0 && quad.p2.y < size)
	{
		average += getValue(quad.p2);
		++count;
	}
	if (quad.p3.x >= 0 && quad.p3.x < size && quad.p3.y >= 0 && quad.p3.y < size)
	{
		average += getValue(quad.p3);
		++count;
	}
	if (quad.p4.x >= 0 && quad.p4.x < size && quad.p4.y >= 0 && quad.p4.y < size)
	{
		average += getValue(quad.p4);
		++count;
	}
	if (count == 0)
		return 0.0;
	return average / count;
}

void GridGenerator2D::boundValue(const GridPoint2D & p)
{
	if (map[p.x][p.y] < minValue)
		map[p.x][p.y] = minValue;
	if (map[p.x][p.y] > maxValue)
		map[p.x][p.y] = maxValue;
}

double GridGenerator2D::getValue(const GridPoint2D & p) const
{
	return map[p.x][p.y];
}

bool GridGenerator2D::isSet(GridPoint2D & p) const
{
	return map[p.x][p.y] > 0;
}

bool GridGenerator2D::pushOn(const GridQuad2D & quad)
{
	GridPoint2D p = quad.midpoint();
	if (!isSet(p))
	{
		nquads.push_back(quad);
		return true;
	}
	return false;
}

int GridGenerator2D::processQuad(const GridQuad2D & quad)
{
	GridPoint2D p = quad.midpoint();
	if (isSet(p))
		return -1;

	setValue(p, getAverage(quad) + randRange(-jumpValue, jumpValue));

	int count = 0;
	if (quad.type == QUAD_SQUARE)
	{
		// To diamond
		count += pushOn(GridQuad2D(quad.p1, GridPoint2D(p.x, p.y - quad.height()), quad.p2, p, QUAD_DIAMOND));
		count += pushOn(GridQuad2D(p, quad.p2, GridPoint2D(p.x + quad.width(), p.y), quad.p3, QUAD_DIAMOND));
		count += pushOn(GridQuad2D(quad.p4, p, quad.p3, GridPoint2D(p.x, p.y + quad.height()), QUAD_DIAMOND));
		count += pushOn(GridQuad2D(GridPoint2D(p.x - quad.width(), p.y), quad.p1, p, quad.p4, QUAD_DIAMOND));
	}
	else //if (quad.type == QUAD_DIAMOND)
	{
		// To square
		if (quad.p1.x >= 0 && quad.p1.x < size && quad.p1.y >= 0 && quad.p1.y < size &&
			quad.p2.x >= 0 && quad.p2.x < size && quad.p2.y >= 0 && quad.p2.y < size)
		{
			GridQuad2D nquad = GridQuad2D(GridPoint2D(quad.p1.x, quad.p2.y), quad.p2, p, quad.p1, QUAD_SQUARE);
			if (nquad.width() > 1 && nquad.height() > 1)
			{
				count += pushOn(nquad);
			}
		}
		if (quad.p2.x >= 0 && quad.p2.x < size && quad.p2.y >= 0 && quad.p2.y < size &&
			quad.p3.x >= 0 && quad.p3.x < size && quad.p3.y >= 0 && quad.p3.y < size)
		{
			GridQuad2D nquad = GridQuad2D(quad.p2, GridPoint2D(quad.p3.x, quad.p2.y), quad.p3, p, QUAD_SQUARE);
			if (nquad.width() > 1 && nquad.height() > 1)
			{
				count += pushOn(nquad);
			}
		}
		if (quad.p3.x >= 0 && quad.p3.x < size && quad.p3.y >= 0 && quad.p3.y < size &&
			quad.p4.x >= 0 && quad.p4.x < size && quad.p4.y >= 0 && quad.p4.y < size)
		{
			GridQuad2D nquad = GridQuad2D(p, quad.p3, GridPoint2D(quad.p3.x, quad.p4.y), quad.p4, QUAD_SQUARE);
			if (nquad.width() > 1 && nquad.height() > 1)
			{
				count += pushOn(nquad);
			}
		}
		if (quad.p4.x >= 0 && quad.p4.x < size && quad.p4.y >= 0 && quad.p4.y < size &&
			quad.p1.x >= 0 && quad.p1.x < size && quad.p1.y >= 0 && quad.p1.y < size)
		{
			GridQuad2D nquad = GridQuad2D(quad.p1, p, quad.p4, GridPoint2D(quad.p1.x, quad.p4.y), QUAD_SQUARE);
			if (nquad.width() > 1 && nquad.height() > 1)
			{
				count += pushOn(nquad);
			}
		}
	}
	return count;
}

bool GridGenerator2D::processPhase()
{
	if (type == BREATH_FIRST)
	{
		if (c > 0)
		{
			nquads.clear();
			for (list<GridQuad2D>::iterator it = quads.begin(); it != quads.end(); ++it)
				processQuad(*it);
			quads = nquads;
			jumpValue *= (1 - decayRate);
			--c;
			return true;
		}
		return false;
	}
	else //(type == DEPTH_FIRST)
	{
		if (!nquads.empty())
		{
			GridQuad2D cur = nquads.back();
			nquads.pop_back();
			int count = processQuad(cur);
			// *** TODO: Multiple divisions and multiplications too slow
			if (count == 0)
				jumpValue /= (1 - decayRate);
			else if (count < 0)
				jumpValue *= (1 - decayRate);

			return true;
		}
		return false;
	}
}

void GridGenerator2D::process()
{
	while (processPhase()) {}
	return;
}

vector< vector< double > > GridGenerator2D::retrieveMap() const
{
	return map;
}