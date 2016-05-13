#pragma once
template <class T> class Point
{
public:
	T x();
	T y();
	void x(T x);
	void y(T y);

	Point();
	Point(T x, T y);
	~Point();

private:
	T _x;
	T _y;
};


template <class T>
Point<T>::Point()
{
}

template <class T>
Point<T>::Point(T x, T y) : _x(x), _y(y)
{
}

template <class T>
Point<T>::~Point()
{
}

template <class T>
T Point<T>::x()
{
	return _x;
}

template <class T>
T Point<T>::y()
{
	return _y;
}

template <class T>
void Point<T>::x(T x)
{
	_x = x;
}

template <class T>
void Point<T>::y(T y)
{
	_y = y;
}


class DPoint : public Point<double>
{
public:
	DPoint() : Point()
	{}

	DPoint(double x, double y) : Point(x, y)
	{}
};