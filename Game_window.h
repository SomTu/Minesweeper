#pragma once

#include "Game_handler.h"
#include "QtWidgets/qwidget.h"
#include "qstring.h"
#include "qfont.h"
#include "Q_mouse_button.h"
#include <string>

class Game_window : public QWidget {
	Q_OBJECT
private:
	int custom_x_dim, custom_y_dim, custom_mines_percentage;
	int current_option;
	Game_handler game_handler;
	std::vector<std::vector<Q_mouse_button*>> fields;

	int timer_seconds, timer_minutes;

public:
	explicit Game_window(QWidget* parent = nullptr);
	~Game_window();
private:
	void delete_all_widgets();
	void center_window(int window_x_size, int window_y_size);
	void clear_game();
signals:
private slots:
	void slot_game();
	void slot_choose_difficulty();
	void slot_custom_setup();
	void slot_change_option(int option);
	void slot_change_x_dim(int val);
	void slot_change_y_dim(int val);
	void slot_change_mine_percentage(int val);
	void slot_difficulty_is_chosen();
	void slot_left_click(int x, int y);
	void slot_right_click(int x, int y);
	void slot_repaint_field(int x, int y);
	void slot_exit_app();

	void slot_update_time_label();
};

// BEGIN Game window constant sizes in px
const int UNIVERSAL_MARGIN = 25;
const int TOP_MENU_HEIGHT = 50;
const int GAME_RESET_TEXTURE_SIZE = 50; // assumes square texture
// END Game window constant sizes in px

const std::string TEXTURE_GAME_RESET = "pics/new_game.jpg";

const int ID_EASY = 0;
const int ID_MEDIUM = 1;
const int ID_HARD = 2;
const int ID_CUSTOM = 3;

const int FIXED_SIZE_DIFFICULTY_X{ 300 }, FIXED_SIZE_DIFFICULTY_Y{ 370 };
const int FIXED_SIZE_CUSTOM_X{ 320 }, FIXED_SIZE_CUSTOM_Y{ 300 };

const int EASY_X_DIM{ 10 }, EASY_Y_DIM{ 10 }, EASY_MINE_PERCENTAGE{ 5 };
const int MEDIUM_X_DIM{ X_DIM_MAX * 2 / 3 }, MEDIUM_Y_DIM{ Y_DIM_MAX * 2 / 3 }, MEDIUM_MINE_PERCENTAGE{ 15 };
const int HARD_X_DIM{ X_DIM_MAX }, HARD_Y_DIM{ Y_DIM_MAX }, HARD_MINE_PERCENTAGE{ 30 };

const QString STYLE_SETUP_WINDOW = "";
const QFont FONT_MESSAGE("", 20, 1);
const QString STYLE_MESSAGE = "QLabel {"
//"background-color: red;"
"}";
const QString STYLE_OPTION = "QPushButton {"
"background-color: red;"
"}";

