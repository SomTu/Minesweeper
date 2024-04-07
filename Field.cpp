#include "Field.h"

Field::Field() :
	is_hidden(true),
	is_flagged(false),
	is_bomb(false),
	bomb_index(0),
	is_exploded(false),
	x_dim(-1),
	y_dim(-1)
{

}
Move_result Field::play() {
	if (!is_hidden) {
		return Move_result::INVALID;
	}
	if (is_flagged) {
		return Move_result::INVALID;
	}
	if (is_bomb) {
		return Move_result::BOMB;
	}
	else {
		is_hidden = false;
		return Move_result::SUCCESS;
	}
}
Move_result Field::flag() {
	if (!is_hidden) {
		return Move_result::INVALID;
	}
	if (is_flagged) {
		is_flagged = false;
		return Move_result::UNFLAG;
	}
	is_flagged = true;
	return Move_result::FLAG;
}
std::string Field::get_texture() {
	if (is_exploded) {
		return TEXTURE_EXPLOSION;
	}
	if (is_flagged) {
		return TEXTURE_FLAG;
	}
	if (is_hidden) {
		return TEXTURE_HIDDEN;
	}
	if (is_bomb) {
		return TEXTURE_BOMB;
	}
	if (bomb_index < 0 || bomb_index > 8) {
		return TEXTURE_ERROR;
	}
	return TEXTURE_NUMBERS[bomb_index];
}