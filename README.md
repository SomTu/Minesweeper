A Minesweeper game copy

Game is written in C++ with Qt handling I/O and graphical side of program.
It was written in Visual Studio with Qt extension, which handled all .h and other files depended on by Qt, which are not included in this repo.

It is an unfinished copy, as it is missing the ability to reset the game as well as a proper game-ending interface.
It still launches terminal, which was used to debug some parts of the game and still writes out current game state.
If one desires to play, it needs to be compiled either using some Qt interface or manually by adding all dependencies to the folder containing .exe file. It can be then deployed using, for example, windeployqt tool.
