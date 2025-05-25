#include <iostream>
#include <string>
#include <cstring>
#include <fstream>  //file stream
#include <sstream>  //String stream, allows you to treat strings like input/output streams
#include <vector>
#include "model.h"

//below is constructor for Model outside of class definition, class definition in model.h
//this reads and loads the file into verts_ and faces_ variables in Model, does not use all data in obj file
Model::Model(const char* filename) : verts_(), faces_() {  //had to learn how passing a string via a char pointer works
    std::ifstream in;
    in.open (filename, std::ifstream::in); //I think the filepath goes here
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {             //other destructors/functions and functions for Model class here.
}

int Model::nverts() {           //function nverts, return type in, namespace Model (a function of the Model class)
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::getFace(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];  //returns a vertex array at index i (a face index number, 1-2492, model has 2,492 triangle faces) from the array of all vertices
}

