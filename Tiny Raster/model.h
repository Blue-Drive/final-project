//these twolines and #endif at end of file are include guards
#ifndef __MODEL_H__  //this line checks if __MODEL_H__ has been defined earlier in the file/included file, prevents double delaration of identifiers like types, enums, or static variables
#define __MODEL_H__  //this line defines it if not      #pragma once serves same purpose as these two lines, but not supported by all IDEs/compilers
//in larger projects you could intentionally or unintentionally include a header file many times which can result in warnings like variable already declared

#include <vector>
#include "geometry.h"

class Model {       //the body is defined here, the functions in the cpp file
private:
	std::vector<Vec3f> verts_; //vector of Vec3<float>s because vertex coordinates are floats
	std::vector<std::vector<int> > faces_;  //vector of int vectors
public:
	Model(const char *filename); //gets taken over by constructor definition in cpp file	
	~Model();  //destructor
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> getFace(int idx);
};

#endif //__MODEL_H__
