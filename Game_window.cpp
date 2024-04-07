#include "Game_window.h"
#include "QtWidgets/qradiobutton.h"
#include "QtWidgets/qslider.h"
#include "QtWidgets/qlabel.h"
#include "QtWidgets/qbuttongroup.h"
#include "QtWidgets/qpushbutton"
#include "Game_handler.h"
#include "QtWidgets/qboxlayout"
#include "QtWidgets/qspinbox.h"
#include <QRect>
#include <QGuiApplication>
#include <QScreen>
#include <string>
#include "Q_mouse_button.h"
#include <qtimer.h>

#include <QDebug>
#include <iostream>

void setup_label(
	QLabel* label,
	const QString& text,
	const QFont& font = QFont(),
	const QString& style_sheet = ""
) {
	label->setText(text);
	label->setStyleSheet(style_sheet);
}
void setup_radio_button(
	QRadioButton* obj,
	int x_pos,
	int y_pos,
	int h,
	int w,
	const QString& text,
	const QString& style_sheet = "",
	bool set_checkable = true
) {
	obj->setCheckable(set_checkable);
	obj->setGeometry(x_pos, y_pos, w, h);
	obj->setText(text);
	obj->setStyleSheet(style_sheet);
}
void setup_push_button(
	QPushButton* obj,
	const QString& text,
	const QString& style_sheet = "",
	bool set_checkable = true
) {
	obj->setCheckable(set_checkable);
	obj->setText(text);
	obj->setStyleSheet(style_sheet);
}
void setup_slider(
	QSlider* obj,
	int minimum,
	int maximum,
	Qt::Orientation orientation,
	int base_value,
	int page_step = 10,
	int single_step = 1
) {
	obj->setMinimum(minimum);
	obj->setMaximum(maximum);
	obj->setOrientation(orientation);
	obj->setValue(base_value);
	obj->setPageStep(page_step);
	obj->setSingleStep(single_step);
}
void setup_spin_box(
	QSpinBox* obj,
	int minimum,
	int maximum,
	int base_value,
	int single_step = 1
) {
	obj->setMinimum(minimum);
	obj->setMaximum(maximum);
	obj->setValue(base_value);
	obj->setSingleStep(single_step);
}


Game_window::Game_window(QWidget* parent)
	: QWidget(parent),
	game_handler(parent),
	custom_x_dim(X_DIM_MIN),
	custom_y_dim(Y_DIM_MIN),
	custom_mines_percentage(0),
	current_option(0)
{
	setWindowIcon(QIcon("pics/bomb.jpg"));
	slot_choose_difficulty();
}
Game_window::~Game_window() {
	clear_game();
}
void Game_window::clear_game() {
	custom_x_dim = X_DIM_MIN;
	custom_y_dim = Y_DIM_MIN;
	custom_mines_percentage = 0;
	current_option = 0;
	game_handler.clear_game();
	// fields vector is already clear if destroy_all_widgets() was call
}
void Game_window::delete_all_widgets() {
	QList<QObject*> list = this->children();
	for (auto i : list) {
		i->~QObject();
	}
	list.clear();
}
void Game_window::slot_game() {
	delete_all_widgets();
	switch (current_option) {
	case ID_EASY:
		game_handler.generate_game(EASY_X_DIM, EASY_Y_DIM, EASY_MINE_PERCENTAGE);
		break;
	case ID_MEDIUM:
		game_handler.generate_game(MEDIUM_X_DIM, MEDIUM_Y_DIM, MEDIUM_MINE_PERCENTAGE);
		break;
	case ID_HARD:
		game_handler.generate_game(HARD_X_DIM, HARD_Y_DIM, HARD_MINE_PERCENTAGE);
		break;
	case ID_CUSTOM:
		game_handler.generate_game(custom_x_dim, custom_y_dim, custom_mines_percentage);
		break;
	}
	int x_size = 2 * UNIVERSAL_MARGIN + game_handler.x_dim * TEXTURE_SIZE;
	int y_size = 3 * UNIVERSAL_MARGIN + TOP_MENU_HEIGHT + game_handler.y_dim * TEXTURE_SIZE;
	setFixedSize(x_size, y_size);
	center_window(x_size, y_size);
	const int FIELD_Y_COORD_START = 2 * UNIVERSAL_MARGIN + TOP_MENU_HEIGHT;

	// create the field
	for (int i = 0; i < game_handler.x_dim; i++) {
		fields.push_back(std::vector<Q_mouse_button*>());
		for (int j = 0; j < game_handler.y_dim; j++) {
			Q_mouse_button* btn = new Q_mouse_button(this, i, j, game_handler.get_field(i, j));
			btn->setGeometry(UNIVERSAL_MARGIN + i * TEXTURE_SIZE, FIELD_Y_COORD_START + j * TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE);
			QString style_sheet = "QPushButton { background-image: url(\"";
			style_sheet.append(game_handler.get_field(i, j).get_texture());
			style_sheet.append("\");}");
			btn->setStyleSheet(style_sheet);
			fields.at(i).push_back(btn);
			connect(btn, SIGNAL(left_click(int, int)), this, SLOT(slot_left_click(int, int)));
			connect(btn, SIGNAL(right_click(int, int)), this, SLOT(slot_right_click(int, int)));
			btn->show();
		}
	}
	// create game-reset menu
	QPushButton* button_game_reset{ new QPushButton(this) };
	button_game_reset->setGeometry(UNIVERSAL_MARGIN + x_size / 2 - GAME_RESET_TEXTURE_SIZE, UNIVERSAL_MARGIN, GAME_RESET_TEXTURE_SIZE, GAME_RESET_TEXTURE_SIZE);
	QString style_sheet = "QPushButton { background-image: url(\"";
	style_sheet.append(TEXTURE_GAME_RESET);
	style_sheet.append("\");}");
	button_game_reset->setStyleSheet(style_sheet);
	button_game_reset->show();
	connect(button_game_reset, SIGNAL(released()), this, SLOT(slot_exit_app()));




	// create timer
	QLabel* label_time = new QLabel(this);
	label_time->setObjectName("label_time");
	QTimer* timer = new QTimer(this);
	timer->setInterval(1000);
	timer->start(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_update_time_label()));
	timer_seconds = 0;
	timer_minutes = 0;
	label_time->move(10, 10);
	//label_time->setText("hello");
	//label_time->setMargin(10);
	//label_time->setMinimumWidth(100);
	slot_update_time_label();
	label_time->show();




	connect(&game_handler, SIGNAL(order_repaint(int, int)), this, SLOT(slot_repaint_field(int, int)));
}
void Game_window::slot_update_time_label() {
	QString time_text = QString("Time elapsed:\n %1 minutes\n %2 seconds.").arg(QString::number(timer_minutes)).arg(timer_seconds);
	this->findChild<QLabel*>("label_time")->setText(time_text);
	this->findChild<QLabel*>("label_time")->adjustSize();
	if (timer_seconds < 60) {
		timer_seconds++;
	}
	else {
		timer_minutes++;
		timer_seconds = 0;
	}
}

void Game_window::slot_choose_difficulty() {
	delete_all_widgets();
	this->clear_game();
	setFixedSize(FIXED_SIZE_DIFFICULTY_X, FIXED_SIZE_DIFFICULTY_Y);
	setStyleSheet(STYLE_SETUP_WINDOW);
	QButtonGroup* group{ new QButtonGroup(this) };
	QRadioButton* option_easy{ new QRadioButton(this) },
		* option_medium{ new QRadioButton(this) },
		* option_hard{ new QRadioButton(this) },
		* option_custom{ new QRadioButton(this) };
	QPushButton* button_choose_difficulty{ new QPushButton(this) },
		* button_quit{ new QPushButton(this) };
	QLabel* message{ new QLabel(this) };
	group->addButton(option_easy, ID_EASY);
	group->addButton(option_medium, ID_MEDIUM);
	group->addButton(option_hard, ID_HARD);
	group->addButton(option_custom, ID_CUSTOM);
	group->setExclusive(true);
	setup_label(message, QObject::tr("Welcome to Minesweeper\nPlease choose difficulty:"), FONT_MESSAGE, STYLE_MESSAGE);
	message->setGeometry(25, 25, 250, 50);
	message->setAlignment(Qt::AlignmentFlag::AlignCenter);
	setup_radio_button(option_easy, 75, 80, 30, 150, "EASY", STYLE_OPTION);
	setup_radio_button(option_medium, 75, 135, 30, 150, "MEDIUM", STYLE_OPTION);
	setup_radio_button(option_hard, 75, 190, 30, 150, "HARD", STYLE_OPTION);
	setup_radio_button(option_custom, 75, 245, 30, 150, "CUSTOM", STYLE_OPTION);
	setup_push_button(button_choose_difficulty, "Continue", "", false);
	button_choose_difficulty->setGeometry(25, 300, 112, 30);
	setup_push_button(button_quit, "Quit", "", false);
	button_quit->setGeometry(162, 300, 112, 30);
	option_easy->setChecked(true);

	connect(button_quit, SIGNAL(released()), this, SLOT(close()));
	connect(group, SIGNAL(idPressed(int)), this, SLOT(slot_change_option(int)));
	connect(button_choose_difficulty, SIGNAL(released()), this, SLOT(slot_difficulty_is_chosen()));
	connect(&game_handler, SIGNAL(game_won()), this, SLOT(close()));
}
void Game_window::slot_custom_setup() {
	delete_all_widgets();
	setFixedSize(FIXED_SIZE_CUSTOM_X, FIXED_SIZE_CUSTOM_Y);
	QBoxLayout* layout_main{ new QBoxLayout(QBoxLayout::TopToBottom, this) },
		* layout_x_and_y{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_x{ new QBoxLayout(QBoxLayout::TopToBottom, this) },
		* layout_y{ new QBoxLayout(QBoxLayout::TopToBottom, this) },
		* layout_x_label{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_y_label{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_x_input{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_y_input{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_mines{ new QBoxLayout(QBoxLayout::TopToBottom, this) },
		* layout_mine_label{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_buttons{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_play{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_quit{ new QBoxLayout(QBoxLayout::LeftToRight, this) },
		* layout_mine_count{ new QBoxLayout(QBoxLayout::LeftToRight, this) };
	QSlider* slider_x_dim{ new QSlider(this) },
		* slider_y_dim{ new QSlider(this) },
		* slider_mine_count{ new QSlider(this) };
	QSpinBox* spin_box_x_dim{ new QSpinBox(this) },
		* spin_box_y_dim{ new QSpinBox(this) },
		* spin_box_mine_count{ new QSpinBox(this) };
	QLabel* label_x_dim{ new QLabel(this) },
		* label_y_dim{ new QLabel(this) },
		* label_mine_count{ new QLabel(this) };
	QPushButton* button_play{ new QPushButton(this) },
		* button_quit{ new QPushButton(this) };
	{
		setup_slider(slider_x_dim, X_DIM_MIN, X_DIM_MAX, Qt::Horizontal, X_DIM_MIN);
		setup_slider(slider_y_dim, Y_DIM_MIN, Y_DIM_MAX, Qt::Horizontal, Y_DIM_MIN);
		setup_slider(slider_mine_count, 0, 100, Qt::Horizontal, 0);
		setup_spin_box(spin_box_x_dim, X_DIM_MIN, X_DIM_MAX, X_DIM_MIN);
		setup_spin_box(spin_box_y_dim, Y_DIM_MIN, Y_DIM_MAX, Y_DIM_MIN);
		setup_spin_box(spin_box_mine_count, 0, 100, 0);
		setup_label(label_x_dim, "Choose number of columns");
		setup_label(label_y_dim, "Choose number of rows");
		setup_label(label_mine_count, "Choose percentage of mines");
		setup_push_button(button_play, "Play", "", false);
		setup_push_button(button_quit, "Quit", "", false);
		layout_main->addLayout(layout_x_and_y);
		layout_main->addLayout(layout_mines);
		layout_x_and_y->addLayout(layout_x);
		layout_x_and_y->addLayout(layout_y);
		layout_x->addLayout(layout_x_label);
		layout_x->addLayout(layout_x_input);
		layout_y->addLayout(layout_y_label);
		layout_y->addLayout(layout_y_input);
		layout_x_label->addWidget(label_x_dim);
		layout_y_label->addWidget(label_y_dim);
		layout_x_input->addWidget(slider_x_dim);
		layout_x_input->addWidget(spin_box_x_dim);
		layout_y_input->addWidget(slider_y_dim);
		layout_y_input->addWidget(spin_box_y_dim);
		layout_mines->addLayout(layout_mine_label);
		layout_mines->addLayout(layout_mine_count);
		layout_mine_label->addWidget(label_mine_count);
		layout_mine_count->addWidget(slider_mine_count);
		layout_mine_count->addWidget(spin_box_mine_count);
		layout_main->addLayout(layout_buttons);
		layout_buttons->addWidget(button_play);
		layout_buttons->addWidget(button_quit);
	}
	connect(slider_x_dim, SIGNAL(valueChanged(int)), spin_box_x_dim, SLOT(setValue(int)));
	connect(slider_x_dim, SIGNAL(valueChanged(int)), this, SLOT(slot_change_x_dim(int)));
	connect(spin_box_x_dim, SIGNAL(valueChanged(int)), slider_x_dim, SLOT(setValue(int)));
	connect(spin_box_x_dim, SIGNAL(valueChanged(int)), this, SLOT(slot_change_x_dim(int)));
	connect(slider_y_dim, SIGNAL(valueChanged(int)), spin_box_y_dim, SLOT(setValue(int)));
	connect(slider_y_dim, SIGNAL(valueChanged(int)), this, SLOT(slot_change_y_dim(int)));
	connect(spin_box_y_dim, SIGNAL(valueChanged(int)), slider_y_dim, SLOT(setValue(int)));
	connect(spin_box_y_dim, SIGNAL(valueChanged(int)), this, SLOT(slot_change_y_dim(int)));
	connect(slider_mine_count, SIGNAL(valueChanged(int)), spin_box_mine_count, SLOT(setValue(int)));
	connect(slider_mine_count, SIGNAL(valueChanged(int)), this, SLOT(slot_change_mine_percentage(int)));
	connect(spin_box_mine_count, SIGNAL(valueChanged(int)), slider_mine_count, SLOT(setValue(int)));
	connect(spin_box_mine_count, SIGNAL(valueChanged(int)), this, SLOT(slot_change_mine_percentage(int)));

	connect(button_play, SIGNAL(released()), this, SLOT(slot_game()));
	connect(button_quit, SIGNAL(released()), this, SLOT(close()));
}
void Game_window::slot_change_option(int option) {
	bool easy = option == ID_EASY;
	bool medium = option == ID_MEDIUM;
	bool hard = option == ID_HARD;
	bool custom = option == ID_CUSTOM;
	if (!easy && !medium && !hard && !custom) {
		throw std::logic_error("Invalid game mode entered");
	}
	current_option = option;
}
void Game_window::slot_change_x_dim(int val) {
	custom_x_dim = val;
}
void Game_window::slot_change_y_dim(int val) {
	custom_y_dim = val;
}
void Game_window::slot_change_mine_percentage(int val) {
	custom_mines_percentage = val;
}
void Game_window::slot_difficulty_is_chosen() {
	bool easy = current_option == ID_EASY;
	bool medium = current_option == ID_MEDIUM;
	bool hard = current_option == ID_HARD;
	bool custom = current_option == ID_CUSTOM;
	if (!easy && !medium && !hard && !custom) {
		throw std::logic_error("Invalid game mode entered");
	}
	if (easy || medium || hard) {
		slot_game();
	}
	if (custom) {
		slot_custom_setup();
	}
}
void Game_window::center_window(int window_x_size, int window_y_size) {
	{ // Not mine
		QScreen* screen = QGuiApplication::primaryScreen();
		QRect screenGeometry = screen->geometry();
		int x_pos = (screenGeometry.width() - window_x_size) / 2;
		int y_pos = (screenGeometry.height() - window_y_size) / 2;
		this->move(x_pos, y_pos);
	}
}
void Game_window::slot_left_click(int x, int y) {
	game_handler.left_click(x, y);
}
void Game_window::slot_right_click(int x, int y) {
	game_handler.right_click(x, y);
}
void Game_window::slot_repaint_field(int x, int y) {
	QString style_sheet = "QPushButton { background-image: url(\"";
	style_sheet.append(fields.at(x).at(y)->field.get_texture());
	style_sheet.append("\");}");
	fields.at(x).at(y)->setStyleSheet(style_sheet);
}
void Game_window::slot_exit_app() {
	delete_all_widgets();
	setFixedSize(200, 200);
	QLabel* text = new QLabel(this);
	QPushButton* quit_button = new QPushButton(this);
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	text->setText("Game ended");
	quit_button->setText("Quit");
	layout->addWidget(text);
	layout->addWidget(quit_button);
	connect(quit_button, SIGNAL(released()), this, SLOT(close()));
}