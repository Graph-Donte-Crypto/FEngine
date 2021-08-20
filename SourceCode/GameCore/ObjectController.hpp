#ifndef FENGINE_SC_ObjectController
#define FENGINE_SC_ObjectController

#include "UseFull/SFMLUp/View.hpp"
#include "UseFull/Templates/Ras.hpp"
#include "UseFull/SFMLUp/GUI/BaseGui.hpp"
#include "UseFull/SFMLUp/GUI/FocusTracker.hpp"
#include "UseFull/SFMLUp/Mouse.hpp"
#include "UseFull/SFMLUp/Drawer.hpp"

#include "GameWindow.hpp"
#include "Basic.hpp"

static
class ObjectController {
public:

	uft::Ras<Basic  > action_set;
	uft::Ras<sfup::gui::BaseGui> action_win_set;
	size_t action_count, draw_count;
	size_t action_i = 0, action_win_i = 0;
	bool debug = false;

	ObjectController()
		: action_count(0)
		, draw_count(0)
	{
		action_set.current = &action_i;
		action_win_set.current = &action_win_i;
	}
	~ObjectController() {
		for (size_t i = 0; i < action_set.length; i++) {
			delete action_set[i];
		}
		for (size_t i = 0; i < action_win_set.length; i++) {
			delete action_win_set[i];
		}
	}
	void stepAction() {
		sfup::Mouse.getPosition();

		action_count = 0;
		for (action_i = 0; action_i < action_set.length; action_i++) {
			action_count += action_set[action_i]->action();
		}
		action_set._fixArray();

	}
	void stepActionWin() {
		sfup::Mouse.getPosition();
		sfup::gui::FocusTracker::nextTurn();

		for (action_win_i = 0; action_win_i < action_win_set.length; action_win_i++) {
			action_win_set[action_win_i]->action();
		}
		action_win_set._fixArray();

	}
	void stepDraw() {

		draw_count = 0;
		for (size_t i = 0; i < action_set.length; i++) {
			draw_count += action_set[i]->draw();
		}

		GuiView.use();
		sfup::Drawer.Text << "FPS: " << GameWindow::calcFps();
		sfup::Drawer.drawText({0, 2});
		sfup::Drawer.Text << "Count(A|D): " << action_count << " | " << draw_count;
		sfup::Drawer.drawText({0, 17});
		sfup::Drawer.drawText("ACTIVE: Ubuntu::Mono::Regular", {0, 32});
		sfup::Drawer.Text << "focus: " << sfup::gui::FocusTracker::focus << " | " << "focus on next turn: " << sfup::gui::FocusTracker::focus_next_turn;
		sfup::Drawer.drawText({0, 47});
	}

	void stepDrawWin() {
		for (size_t i = 0; i < action_win_set.length; i++) {
			action_win_set[i]->drawSelf();
			action_win_set[i]->drawTo(GameWindow::window);
		}
	}
}
OController;

#endif