#ifndef FENGINE_SC_Window
#define FENGINE_SC_Window

#include "UseFull/SFMLUp/GUI/BaseGui.hpp"
#include "UseFull/SFMLUp/GUI/Label.hpp"
#include "UseFull/SFMLUp/Mouse.hpp"

class Window : public sfup::gui::BaseGui {
public:
	//ctrlm -> control_menu
	double ctrlm_paddind = 20;
	math::XY moveble_center = math::XY{0, 0};
	sfup::gui::Label label;

	Window(const math::Codir<2> & codir)
	: BaseGui(codir)
	, label(math::Codir<2>({0, 0}, {codir.size()[0], ctrlm_paddind}))
	{
		FocusTracker::codir_focus = math::Codir<2>({0, 0}, {codir.size()[0], ctrlm_paddind});
		color_background = sf::Color::Black;
		color_outline = sf::Color::White;
		label.color_outline = sf::Color::Transparent;
		label.setTextColor(sf::Color::White);
	}
	~Window() {
		elements.flushHard();
	}
	void drawSelf() override {

		BaseGui::drawSelf();

		sfup::Drawer.drawLine(
			*this,
			math::Line<2>({0, ctrlm_paddind}, {codir.size()[0], ctrlm_paddind}),
			color_outline
		);

		label.drawSelf();
		label.drawTo(*this);

		drawSelfForElements();
	}
	void actionNotFocused() override {
		if (state_before.pressed) {
			actionPressing();
			FocusTracker::focus = this;
			FocusTracker::focus_next_turn = this;
			state_after.pressed = true;
			state_after.focused = true;
		}
	}
	void actionFocused() override {

	}
	void actionPressed() override {
		moveble_center = codir.center() - sfup::Mouse.inWorld;
	}
	void moveRelative(const math::XY & delta) override {
		BaseGui::moveRelative(delta);
		//label.moveRelative(delta);
	}
	void actionPressing() override {
		moveRelative(moveble_center - codir.center() + sfup::Mouse.inWorld);
	}
	void action() override {
		if (active) {
			actionForElements();
			FocusTracker::focus_offset = getAbsoluteOffset();
			checkFocus(sfup::Mouse.inWorld);
		}
	}
};

#endif