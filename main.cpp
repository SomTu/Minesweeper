#include "QtWidgets/qapplication.h"
#include "Game_window.h"
#include <Windows.h>
#include <iostream>


int main(int argc, char** argv) {
	QApplication app(argc, argv);
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::cout << "hi";
	Game_window window;
	window.show();
	return app.exec();
}