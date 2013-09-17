#include "object.h"

#include "pch.h"

const double PI = 3.14159265;
const double E = 2.7182818;
const double EPSILON = pow(2.0, -3);

void Object2D::render() const
{
	glColor4d(color.r, color.g, color.b, color.a);
}

void Object2D::render(const Color & col) const
{
	glColor4d(col.r, col.g, col.b, col.a);
}

double Point2D::magnitude() const
{
	return sqrt(x * x + y * y);
}

double Point2D::magnitude2() const
{
	return x * x + y * y;
}

Point2D Point2D::normalized() const
{
	return (*this) / magnitude();
}

void Point2D::set(const Point2D & pos)
{
	x = pos.x;
	y = pos.y;
}

void Point2D::move(double dx, double dy) 
{
	x += dx;
	y += dy;
}

void Point2D::render() const
{
	Object2D::render();
	glBegin(GL_POINTS);
	glVertex2d(x, y);
	glEnd();
}

void Point2D::render(const Color & col) const
{
	Object2D::render(col);
	glBegin(GL_POINTS);
	glVertex2d(x, y);
	glEnd();
}

bool Line2D::isOnLine(const Point2D & o) const
{
	double cross = (o.y - p1.y) * (p2.x - p1.x) - (o.x - p1.x) * (p2.y - p1.y);
	if (abs(cross) > EPSILON) return false;

	double dot = (o.x - p1.x) * (p2.x - p1.x) + (o.y - p1.y) * (p2.y - p1.y);
	if (dot < 0) return false;

	if (dot > magnitude2()) return false;
	return true;
}

double Line2D::magnitude() const
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

double Line2D::magnitude2() const
{
	return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2);
}

double Line2D::angle() const
{
	return atan2(p2.y - p1.y, p2.x - p1.x);
}

double Line2D::dot() const
{
	return p1.x * p2.x + p1.y * p2.y;
}

double Line2D::cross() const
{
	return p1.x * p2.y - p2.x * p1.y;
}

Point2D Line2D::delta() const
{
	return p2 - p1;
}

Point2D Line2D::midpoint() const
{
	return (p1 + p2) / 2.0;
}

//Returns the slope in the form (m, b) for the equation y = mx + b
Point2D Line2D::slopeformFX() const
{
	//*** no check for v.x = 0
	Point2D v = delta();
	double slope = v.y / v.x;
	double yintercept = p1.y - slope * p1.x;
	return Point2D(slope, yintercept);
}

Point2D Line2D::slopeformFY() const
{
	//*** no check for v.y = 0
	Point2D v = delta();
	double slope = v.x / v.y;
	double xintercept = p1.x - slope * p1.y;
	return Point2D(slope, xintercept);
}

//Returns the point closest to the imaginary infinite line of this
Point2D Line2D::projectedPoint(const Point2D & o) const
{
	double a1 = p2.y - p1.y;
	double b1 = p1.x - p2.x;
	double c1 = a1 * p1.x + b1 * p1.y;
	double c2 = -b1 * o.x + a1 * o.y;
	double det = a1 * a1 + b1 * b1;
	Point2D ret;
	if (det != 0.0)
	{
		ret.x = (a1 * c1 - b1 * c2) / det;
		ret.y = (a1 * c2 + b1 * c1) / det;
	}
	else
	{
		ret.x = o.x;
		ret.y = o.y;
	}
	return ret;
}

double Line2D::angleDifference(const Line2D & l2) const
{
	//Get angle difference between the two lines
	double angularDist = abs(angle() - l2.angle());
	double normDist = angularDist;
	if (angularDist > PI / 2) 
		normDist = PI - angularDist;
	return normDist;
}

//Swaps the points such the furthest leftmost then bottommost point is p1.
void Line2D::reorderPoints()
{
	if (p2.x < p1.x)
	{
		Point2D t = p1;
		p1 = p2;
		p2 = t;
	}
	else if (p2.x == p1.x && p2.y < p1.y)
	{
		Point2D t = p1;
		p1 = p2;
		p2 = t;
	}
}

void Line2D::set(const Point2D & pos) 
{
	Point2D moveBy = pos - midpoint();
	p1 += moveBy;
	p2 += moveBy;
}

void Line2D::move(double dx, double dy)
{
	p1.move(dx, dy);
	p2.move(dx, dy);
}

void Line2D::render() const
{
	Object2D::render();
	glBegin(GL_LINES);
	glVertex2d(p1.x, p1.y);
	glVertex2d(p2.x, p2.y);
	glEnd();
}

void Line2D::render(const Color & col) const
{
	Object2D::render(col);
	glBegin(GL_LINES);
	glVertex2d(p1.x, p1.y);
	glVertex2d(p2.x, p2.y);
	glEnd();
}

double Rect2D::perimeter() const
{
	return 2 * width + 2 * height;
}

double Rect2D::area() const
{
	return width * height;
}

Point2D Rect2D::center() const
{
	return Point2D(p.x + width / 2, p.y + height / 2);
}

//Returns the point on the rectangle given the x and y location parameters on a scale from 0-100%
//The (0%,0%)th position is the reference point, p.
//Any percentage out of this range will return a location outside of the object
Point2D Rect2D::getLocation(double xscale, double yscale) const
{
	return Point2D(p.x + (width * xscale), p.y + (height * yscale));
}

bool Rect2D::contains(const Point2D & loc) const
{
	return loc.x > p.x && loc.x < p.x + width &&
		loc.y > p.y && loc.y < p.y + height;
}

void Rect2D::set(const Point2D & pos)
{
	p = pos;
}

void Rect2D::move(double dx, double dy)
{
	p.move(dx, dy);
}

void Rect2D::present() const
{
	if (filled)
	{
		glBegin(GL_QUADS);
		glVertex2d(p.x, p.y);
		glVertex2d(p.x + width, p.y);
		glVertex2d(p.x + width, p.y + height);
		glVertex2d(p.x, p.y + height);
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		Point2D po = p;
		Point2D pn = po;
		pn.move(width, 0.0);
		glVertex2d(po.x, po.y);
		glVertex2d(pn.x, pn.y);
		po = pn;
		pn.move(0.0, height);
		glVertex2d(po.x, po.y);
		glVertex2d(pn.x, pn.y);
		po = pn;
		pn.move(-width, 0.0);
		glVertex2d(po.x, po.y);
		glVertex2d(pn.x, pn.y);
		po = pn;
		pn.move(0.0, -height);
		glVertex2d(po.x, po.y);
		glVertex2d(pn.x, pn.y);
		glEnd();
	}
}

void Rect2D::render() const
{
	Object2D::render();
	present();
}

void Rect2D::render(const Color & col) const
{
	Object2D::render(col);
	present();
}

double Circle2D::perimeter() const
{
	return 2 * PI * radius;
}

double Circle2D::area() const
{
	return PI * radius * radius;
}

bool Circle2D::contains(const Point2D & loc) const
{
	return (loc.x - p.x) * (loc.x - p.x) + (loc.y - p.y) * (loc.y - p.y) <
		radius * radius;
}

void Circle2D::set(const Point2D & pos)
{
	p = pos;
}

void Circle2D::move(double dx, double dy)
{
	p.move(dx, dy);
}

void Circle2D::present() const
{
	const double ARC_LINE = PI / 180;
	if (filled)
	{	
		glBegin(GL_POLYGON);
		double theta = 0.0;
		while (theta <= 2 * PI)
		{
			glVertex2d(p.x + radius * cos(theta), p.y + radius * sin(theta));
			theta += ARC_LINE;
		}
		glEnd();
	}
	else
	{	
		glBegin(GL_LINE_LOOP);
		double theta = 0.0;
		while (theta <= 2 * PI)
		{
			glVertex2d(p.x + radius * cos(theta), p.y + radius * sin(theta));
			theta += ARC_LINE;
		}
		glEnd();
	}
}

void Circle2D::render() const
{
	Object2D::render();
	present();
}

void Circle2D::render(const Color & col) const
{
	Object2D::render(col);
	present();
}

double Triangle2D::perimeter() const
{
	return Line2D(p1, p2).magnitude() + Line2D(p2, p3).magnitude() + Line2D(p3, p1).magnitude();
}

double Triangle2D::area() const
{
	Line2D baseLine = Line2D(p2, p3);
	Line2D projectedLine = Line2D(p1, baseLine.projectedPoint(p1));
	return 0.5 * baseLine.magnitude() * projectedLine.magnitude();
}
	
// http://www.blackpawn.com/texts/pointinpoly/
bool Triangle2D::contains(const Point2D & loc) const
{
	Point2D v0 = p3 - p1;
	Point2D v1 = p2 - p1;
	Point2D v2 = loc - p1;

	double dot00 = Line2D(v0, v0).dot();
	double dot01 = Line2D(v0, v1).dot();
	double dot02 = Line2D(v0, v2).dot();
	double dot11 = Line2D(v1, v1).dot();
	double dot12 = Line2D(v1, v2).dot();

	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return u > 0 && v > 0 && u + v < 1;
}

void Triangle2D::set(const Point2D & pos)
{
	Point2D cent = Point2D((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
	Point2D moveBy = pos - cent;
	p1 += moveBy;
	p2 += moveBy;
	p3 += moveBy;
}
	
void Triangle2D::move(double dx, double dy)
{
	p1.move(dx, dy);
	p2.move(dx, dy);
	p3.move(dx, dy);
}

void Triangle2D::present() const
{
	if (filled)
	{
		glBegin(GL_TRIANGLES);
		glVertex2d(p1.x, p1.y);
		glVertex2d(p2.x, p2.y);
		glVertex2d(p3.x, p3.y);
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		glVertex2d(p1.x, p1.y);
		glVertex2d(p2.x, p2.y);
		glVertex2d(p2.x, p2.y);
		glVertex2d(p3.x, p3.y);
		glVertex2d(p3.x, p3.y);
		glVertex2d(p1.x, p1.y);
		glEnd();
	}
}

void Triangle2D::render() const
{
	Object2D::render();
	present();
}

void Triangle2D::render(const Color & col) const
{
	Object2D::render(col);
	present();
}

double Poly2D::perimeter() const
{
	double total = 0.0;
	for (unsigned i = 1; i < points.size(); ++i)
		total += Line2D(points[i - 1], points[i]).magnitude();
	return total;
}

void Poly2D::set(const Point2D & pos)
{
	Point2D cent;
	for (unsigned i = 0; i < points.size(); ++i)
		cent += points[i];
	cent /= points.size();
	Point2D moveBy = pos - cent;
	for (unsigned i = 0; i < points.size(); ++i)
		points[i] += moveBy;
}

void Poly2D::move(double dx, double dy)
{
	for (unsigned i = 0; i < points.size(); ++i)
		points[i].move(dx, dy);
}

void Poly2D::present() const
{
	if (points.size() < 3)
		return;
	if (filled)
	{
		glBegin(GL_POLYGON);
		for (unsigned i = 0; i < points.size(); ++i)
			glVertex2d(points[i].x, points[i].y);
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < points.size(); ++i)
		{
			glVertex2d(points[i].x, points[i].y);
		}
		if (points.size() > 2)
		{
			glVertex2d(points[0].x, points[0].y);
		}
		glEnd();
	}
}

void Poly2D::render() const
{
	Object2D::render();
	present();
}

void Poly2D::render(const Color & col) const
{
	Object2D::render(col);
	present();
}
