#ifndef __PV2D_H
#define __PV2D_H

#include <freeglut.h>

#include <math.h> 
class PV2D {

private:

	GLdouble _x, _y;

public:

	PV2D() : _x(0.0), _y(0.0) {}
	PV2D(GLdouble x, GLdouble y) : _x(x), _y(y) {}
	PV2D(const PV2D &p) : _x(p._x), _y(p._y) {}

	~PV2D() {
	}
	bool operator==(PV2D other) {
		 return ( _x == other._x && _y == other._y );
	}

	bool operator!=(PV2D other) {
		return !operator==(other);
	}

	void set_x(GLdouble x) {
		_x = x;
	}

	void set_y(GLdouble y) {
		_y = y;
	}
	
	GLdouble get_x() {
		return _x;
	}

	GLdouble get_y() {
		return _y;
	}

	GLdouble distanciaPto2Pto(PV2D q) {
		return sqrt( pow( (q._x - _x),2 ) + pow( (q._y - _y),2 ) ); 
	}

	PV2D puntoMedio(PV2D q)
	{
		return PV2D((_x + q._x)/2, (_y + q._y)/2);
	}



	PV2D crearVector_RestaPto2Pto(PV2D q) {
		return PV2D((q._x - _x), (q._y - _y));
	}

	PV2D sumarVect2Pto(PV2D v) {
		return PV2D((_x + v._x), (_y + v._y));
	}


	void invertir()
	{
		_x = -_x;
		_y = -_y;
	}

	PV2D sumarVect2Vect(PV2D w) {
		return PV2D((w._x + _x), (w._y + _y));
	}

	PV2D restarVect2Vect(PV2D w) {
		return PV2D((_x - w._x), (_y - w._y));
	}

	PV2D multiplicarVect2Escalar(GLdouble k) {
		return PV2D((k * _x), (k * _y));
	}


	GLdouble moduloVect() {
		return sqrt( pow(_x, 2) + pow(_y, 2));
	}
	PV2D normalizarVect() {
		if (moduloVect() != 0)
			return PV2D ( _x/moduloVect(), _y/moduloVect());
		else
			return PV2D();
	}

	GLdouble productoEscalar(PV2D w) {
		return ((_x * w._x) + (_y * w._y));
	}

	PV2D vectorNormal() {
		return PV2D ( -_y, _x);
	}

}; 

#endif // __PV2D_H