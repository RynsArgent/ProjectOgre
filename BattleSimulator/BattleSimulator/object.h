#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef IN_WINDOWS
#include <Windows.h>
#endif

#ifdef IN_MAC 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>

struct Color
{
	double r;
	double g;
	double b;
	double a;
	Color() : r(1.0), g(1.0), b(1.0), a(0.0) {}
	Color(const double & nr, const double & ng, const double & nb, const double & na = 0.0)
		: r(nr), g(ng), b(nb), a(na)
	{}
};

#define WHITE Color(1.0, 1.0, 1.0)
#define BLACK Color(0.0, 0.0, 0.0)
#define RED Color(1.0, 0.0, 0.0)
#define GREEN Color(0.0, 1.0, 0.0)
#define BLUE Color(0.0, 0.0, 1.0)
#define YELLOW Color(1.0, 1.0, 0.0)
#define PINK Color(1.0, 0.0, 1.0)
#define CYAN Color(0.0, 1.0, 1.0)
#define ORANGE Color(1.0, 0.5, 0.0)
#define VIOLET Color(0.5, 0.0, 1.0)
#define GRAY Color(0.5, 0.5, 0.5)
#define BROWN Color(0.6, 0.3, 0.0)

struct Object2D
{
	Color color;
	Object2D() : color() {}
	Object2D(const Color & c) : color(c) {}

	virtual void move(double dx, double dy) = 0;
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Object2D() {}
};

struct Shape2D : public Object2D
{
	bool filled;
	Shape2D() : Object2D(), filled(false) {}
	Shape2D(const Color & c, bool f = false)
		: Object2D(c), filled(f)
	{}
	
	virtual void present() const = 0;

	virtual ~Shape2D() {}
};

struct Point2D : public Object2D
{
	double x;
	double y;

	Point2D() : Object2D(), x(0.0), y(0.0) {}
	Point2D(const double & nx, const double & ny, const Color & col = WHITE) 
		: Object2D(col), x(nx), y(ny)
	{}
	
	Point2D operator+(const Point2D & rhs) const { return Point2D(x + rhs.x, y + rhs.y); }
	Point2D operator-(const Point2D & rhs) const { return Point2D(x - rhs.x, y - rhs.y); }
	Point2D operator*(double val) const { return Point2D(x * val, y * val); }
	Point2D operator/(double val) const { return Point2D(x / val, y / val); }
	Point2D operator+=(const Point2D & rhs) { x += rhs.x; y += rhs.y; return *this; }
	Point2D operator-=(const Point2D & rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	Point2D operator*=(double val) { x *= val; y *= val; return *this; }
	Point2D operator/=(double val) { x /= val; y /= val; return *this; }
	
	double magnitude() const;
	double magnitude2() const;
	Point2D normalized() const;

	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Point2D() {}
};

struct Line2D : public Object2D
{
	Point2D p1;
	Point2D p2;

	Line2D() : Object2D(), p1(), p2() {}
	Line2D(double x1, double y1, double x2, double y2, const Color & col = WHITE)
		: Object2D(col), p1(x1, y1), p2(x2, y2)
	{}
	Line2D(const Point2D & p1, const Point2D & p2, const Color & col = WHITE)
		: Object2D(col), p1(p1), p2(p2)
	{}
	
	bool isOnLine(const Point2D & p) const;
	double magnitude() const;
	double magnitude2() const;
	double angle() const;
	double dot() const;
	double cross() const;
	Point2D delta() const;
	Point2D midpoint() const;
	Point2D slopeformFX() const; //y = mx + b
	Point2D slopeformFY() const; //x = my + b
	Point2D projectedPoint(const Point2D & o) const;
	double angleDifference(const Line2D & l2) const;

	void reorderPoints();

	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Line2D() {}
};

struct Rect2D : public Shape2D
{
	Point2D p;
	double width;
	double height;

	Rect2D() : Shape2D(), p(), width(0.0), height(0.0) {}
	Rect2D(const Point2D & p, double width, double height, const Color & col = WHITE, bool fill = false) 
		: Shape2D(col, fill), p(p), width(width), height(height) 
	{}

	double perimeter() const;
	double area() const;
	Point2D center() const;
	Point2D getLocation(double xscale, double yscale) const;
	bool contains(const Point2D & loc) const;
	
	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void present() const;
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Rect2D() {}
};

struct Circle2D : public Shape2D
{
	Point2D p;
	double radius;

	Circle2D() : Shape2D(), p(), radius(0.0) {}
	Circle2D(const Point2D & p, double radius, Color col = Color(), bool fill = false)
		: Shape2D(col, fill), p(p), radius(radius)
	{}

	double perimeter() const;
	double area() const;
	bool contains(const Point2D & loc) const;
	
	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void present() const;
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Circle2D() {}
};

struct Triangle2D : public Shape2D
{
	Point2D p1;
	Point2D p2;
	Point2D p3;
	
	Triangle2D() : Shape2D(), p1(), p2(), p3() {}
	Triangle2D(const Point2D & p1, const Point2D & p2, const Point2D & p3, const Color & col = WHITE, bool fill = false)
		: Shape2D(col, fill), p1(p1), p2(p2), p3(p3)
	{}
	
	double perimeter() const;
	double area() const;
	bool contains(const Point2D & loc) const;
	
	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void present() const;
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Triangle2D() {}
};

struct Poly2D : public Shape2D
{
	std::vector<Point2D> points;

	Poly2D() : Shape2D(), points() {}
	Poly2D(const std::vector<Point2D> & points, const Color & col = WHITE, bool fill = false)
		: Shape2D(col, fill), points(points)
	{}

	double perimeter() const;
	
	void set(const Point2D & pos);
	virtual void move(double dx, double dy);
	virtual void present() const;
	virtual void render() const;
	virtual void render(const Color & col) const;

	virtual ~Poly2D() {}
};

#endif
