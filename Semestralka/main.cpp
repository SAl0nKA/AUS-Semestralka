#include "MemLeak.h"
#include "Console.h"

int main(){
	Console* console;
	try {
		console = new Console("RT.csv");
	} catch (const std::exception& e) {
		printf("%s\n", e.what());
		printf("Exiting\n");
		//delete console;
		return 1;
	}

	bool dontStop = true;
	while (dontStop) {
		dontStop = console->mainMenu();
	}
	delete console;
}
