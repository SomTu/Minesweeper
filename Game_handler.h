#pragma once

#include "QtWidgets/qwidget.h"
#include "Field.h"
#include <stack>

const int X_DIM_MIN{ 3 };
const int Y_DIM_MIN{ 3 };
const int X_DIM_MAX{ 30 };
const int Y_DIM_MAX{ 15 };

enum Game_state {
	PLAYING = 0,
	WON = 1,
	LOST = 2
};

class Game_window;
class Game_handler : public QObject {
	Q_OBJECT
private:
	friend class Game_window;
	std::vector<std::vector<Field>> game_vec;
	int x_dim, y_dim, num_of_mines, flagged_fields_correct, flagged_fields_total;
	Field& get_field(int x, int y);
	std::vector<Field*> get_neighboring_fields(int x, int y);
	void clear_game();
	void update_flagged_counter(const Field& field, bool will_be_flagged);
	void flood_fill_update_stack_and_field(std::stack<Field*>& iteration_stack, Field* seed_field);
	void flood_fill(int x, int y);
	void uncover_all_fields();
public:
	Game_handler(QObject* parent);
	~Game_handler();
	void generate_game(int x, int y, int mines_percentage);
	void left_click(int x, int y);
	void right_click(int x, int y);
signals:
	void order_repaint(int x, int y);
public slots:
};
