#include "Game_handler.h"
#include <random>
#include <cmath>
#include <iostream>

Game_handler::Game_handler(QObject* parent)
	: QObject(parent),
	x_dim(X_DIM_MIN),
	y_dim(Y_DIM_MIN),
	num_of_mines(0),
	flagged_fields_correct(0),
	flagged_fields_total(0)
{}
Game_handler::~Game_handler() {
	clear_game();
}
void Game_handler::clear_game() {
	for (int i = 0; i < game_vec.size(); i++) {
		game_vec.at(i).clear();
	}
	game_vec.clear();
	x_dim = X_DIM_MIN;
	y_dim = Y_DIM_MIN;
	num_of_mines = 0;
	flagged_fields_correct = 0;
	flagged_fields_total = 0;
}
void Game_handler::generate_game(int x, int y, int mines_percentage) {
	x_dim = x;
	y_dim = y;
	flagged_fields_correct = 0;
	flagged_fields_total = 0;
	num_of_mines = (int)std::floor(x_dim * y_dim * mines_percentage / 100);
	for (auto& i : game_vec) {
		i.clear();
	}
	game_vec.clear();
	// first pass, generating empty field
	for (int i = 0; i < x_dim; i++) {
		std::vector<Field> new_vec;
		game_vec.push_back(new_vec);
		for (int j = 0; j < y_dim; j++) {
			Field new_field;
			new_field.x_dim = i;
			new_field.y_dim = j;
			game_vec.at(i).push_back(new_field);
		}
	}
	// second pass, inserting mines
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist_x(0, x_dim - 1);
	std::uniform_int_distribution<int> dist_y(0, y_dim - 1);
	for (int i = 0; i < (num_of_mines > 0 ? num_of_mines : 1); i++) {
		while (true) {
			int rnd_x{ dist_x(mt) }, rnd_y{ dist_y(mt) };
			if (!get_field(rnd_x, rnd_y).is_bomb) {
				get_field(rnd_x, rnd_y).is_bomb = true;
				break;
			}
		}
	}
	// third pass, calculating numbers around mines
	for (int i = 0; i < x_dim; i++) {
		for (int j = 0; j < y_dim; j++) {
			if (get_field(i, j).is_bomb == true) continue;
			int count_of_neighboring_mines = 0;
			for (auto elem : get_neighboring_fields(i, j)) {
				if (elem->is_bomb == true) {
					count_of_neighboring_mines++;
				}
			}
			get_field(i, j).bomb_index = count_of_neighboring_mines;
		}
	}
}
void Game_handler::left_click(int x, int y) {
	if (get_field(x, y).is_flagged == true) return;
	if (get_field(x, y).is_bomb == true) {
		get_field(x, y).is_exploded = true;
		uncover_all_fields();
	}
	flood_fill(x, y);
	emit order_repaint(x, y);
}
void Game_handler::right_click(int x, int y) {
	Field& field = get_field(x, y);
	if (field.is_hidden == false) {
		emit order_repaint(x, y);
		return;
	}
	Move_result MR = field.flag();
	if (MR == Move_result::INVALID) {}
	else {
		update_flagged_counter(field, MR == Move_result::FLAG);
	}
	emit order_repaint(x, y);
}
Field& Game_handler::get_field(int x, int y) {
	return game_vec.at(x).at(y);
}
std::vector<Field*> Game_handler::get_neighboring_fields(int x, int y) {
	std::vector<Field*> out;
	// 0 1 2
	// 3 X 4
	// 5 6 7
	// added in this order
	if (x > 0 && y > 0) out.push_back(&get_field(x - 1, y - 1));
	if (y > 0) out.push_back(&get_field(x, y - 1));
	if (x < x_dim - 1 && y > 0) out.push_back(&get_field(x + 1, y - 1));
	if (x > 0) out.push_back(&get_field(x - 1, y));
	if (x < x_dim - 1) out.push_back(&get_field(x + 1, y));
	if (x > 0 && y < y_dim - 1) out.push_back(&get_field(x - 1, y + 1));
	if (y < y_dim - 1) out.push_back(&get_field(x, y + 1));
	if (x < x_dim - 1 && y < y_dim - 1) out.push_back(&get_field(x + 1, y + 1));
	return out;
}
void Game_handler::update_flagged_counter(const Field& field, bool will_be_flagged) {
	if (will_be_flagged == true) { // increase counter
		flagged_fields_total++;
		if (field.is_bomb) {
			flagged_fields_correct++;
		}
	}
	if (will_be_flagged == false) {
		flagged_fields_total--;
		if (field.is_bomb) {
			flagged_fields_correct--;
		}
	}
	std::cout
		<< "Num of mines: "
		<< num_of_mines
		<< " , Flagged fields total: "
		<< flagged_fields_total
		<< ", Flagged fields correct: "
		<< flagged_fields_correct
		<< "\n";
	if (flagged_fields_correct == num_of_mines) {
		std::cout << "WON\n";
		uncover_all_fields();
	}
	return;
}
void Game_handler::flood_fill_update_stack_and_field(
	std::stack<Field*>& iteration_stack,
	Field* seed_field) {
	if (seed_field->is_hidden == false) return;
	if (seed_field->is_flagged == true) return;
	seed_field->is_hidden = false;
	int x = seed_field->x_dim;
	int y = seed_field->y_dim;
	if (seed_field->bomb_index == 0) {
		std::vector<Field*> neighbors = get_neighboring_fields(x, y);
		for (auto i : neighbors) {
			iteration_stack.push(i);
		}
	}
	emit order_repaint(x, y);
}
void Game_handler::flood_fill(int x, int y) {
	std::stack<Field*> iteration_stack;
	iteration_stack.push(&get_field(x, y));
	do {
		Field* temp = iteration_stack.top();
		iteration_stack.pop();
		flood_fill_update_stack_and_field(iteration_stack, temp);
	} while (!iteration_stack.empty());
}
void Game_handler::uncover_all_fields() {
	for (int i = 0; i < game_vec.size(); i++) {
		for (int j = 0; j < game_vec.at(i).size(); j++) {
			get_field(i, j).is_hidden = false;
			emit order_repaint(i, j);
		}
	}
}