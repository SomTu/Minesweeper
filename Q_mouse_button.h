#pragma once

#include "QtWidgets/qpushbutton.h"
#include "QtWidgets/qwidget.h"
#include <QMouseEvent>
#include "Field.h"

#define GAME_RESET_BUTTON_INDEX -69

class Game_window;
class Q_mouse_button : public QPushButton
{
	Q_OBJECT
protected:
	friend Game_window;
	int x_pos, y_pos;
	Field& field;
	void mousePressEvent(QMouseEvent* event) override;
public:
	Q_mouse_button(QWidget* parent, int x_pos, int y_pos, Field& field);
	~Q_mouse_button();
signals:
	void left_click(int x, int y);
	void right_click(int x, int y);
	void game_reset_signal();
};
