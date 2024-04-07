#pragma once
#ifndef _FIELD_H_
#define _FIELD_H_

#include <string>

#define TEXTURE_SIZE 33 //square texture, in px
// it isnt the same as picture sizes, as QPushButtons are problematic

enum Move_result {
	SUCCESS = 0,
	INVALID = 1,
	BOMB = 2,
	FLAG = 3,
	UNFLAG = 4
};

class Game_handler;
class Field {
	//private:
public:
	friend Game_handler;
	bool is_hidden;
	bool is_flagged;
	bool is_bomb;
	int bomb_index;
	bool is_exploded;
	int x_dim, y_dim;
public:
	Field();
	Move_result play();
	Move_result flag();
	std::string get_texture();
};

const std::string TEXTURE_HIDDEN = "pics/hidden.jpg";
const std::string TEXTURE_FLAG = "pics/flag.jpg";
const std::string TEXTURE_BOMB = "pics/bomb.jpg";
const std::string TEXTURE_EXPLOSION = "pics/explosion.jpg";
const std::string TEXTURE_NUMBERS[9] = {
	"pics/empty.jpg",
	"pics/one.jpg",
	"pics/two.jpg",
	"pics/three.jpg",
	"pics/four.jpg",
	"pics/five.jpg",
	"pics/six.jpg",
	"pics/seven.jpg",
	"pics/eight.jpg"
};
const std::string TEXTURE_ERROR = "pics/error.gif";

#endif // _FIELD_H