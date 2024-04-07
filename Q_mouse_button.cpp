#include "Q_mouse_button.h"

#include <iostream>

Q_mouse_button::Q_mouse_button(QWidget* parent, int x_pos, int y_pos, Field& field)
	: QPushButton(parent),
	x_pos(x_pos),
	y_pos(y_pos),
	field(field)
{}
Q_mouse_button::~Q_mouse_button()
{}
void Q_mouse_button::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		if (x_pos == GAME_RESET_BUTTON_INDEX && y_pos == GAME_RESET_BUTTON_INDEX) {
			emit game_reset_signal();
			return;
		}
		emit left_click(x_pos, y_pos);
	}
	if (event->button() == Qt::RightButton) {
		emit right_click(x_pos, y_pos);
	}
}