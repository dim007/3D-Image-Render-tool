#include "glm\glm.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "scene.h"


using namespace std;
using namespace glm;

int main(int argc, char* argv[]) {
	if (argc == 1) {
		cout << "No input file specified." << endl;
		return -1;
	}
	ifstream input;
	input.open(argv[1]); //opening file
	if (!input.is_open()) {
		cout << "File not good." << endl;
		return -1;
	}
	cout << "File good. Reading file..." << endl;
	Scene myscene = Scene();
	myscene.readfile(argv[1]);
	cout << "Read done. Rendering Scene..." << endl;
	myscene.render();
}
