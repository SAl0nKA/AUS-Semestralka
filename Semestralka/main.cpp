//#include <iostream>
#include <vector>
#include "RTParser.h"
#include "Algorithm.h"
#include "MemLeak.h"
#include "Console.h"

int main(){
	Console* console = new Console("RT.csv");

	bool dontStop = true;
	while (dontStop) {
		dontStop = console->mainMenu();
	}
	delete console;
}
