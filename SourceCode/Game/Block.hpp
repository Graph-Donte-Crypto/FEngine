#ifndef FENGINE_SC_G_Block
#define FENGINE_SC_G_Block

#include "../GameCore/GameCore.hpp"

class Block : public Basic {
public:
	XY left_down, right_up;
	void textAllInfo() override {
		Basic::textAllInfo();
		Drawer.Text << "\nleft_down = " << left_down[0] << " " << left_down[1];
		Drawer.Text << "\nright_up = " << right_up[0] << " " << right_up[1];
	}

	void moveRelative(const XY & delta) override {
		Basic::moveRelative(delta);
		left_down = {codir.left_up[0], codir.right_down[1]};
		right_up = {codir.right_down[0], codir.left_up[1]};
	}


	Block()
		: Basic()
	{
		type = ClassType::Block;
		inertional = false;
		mass = 10;
	}

	Block(const Codir<2> & codir)
		: Basic(codir)
		, left_down({codir.left_up[0], codir.right_down[1]})
		, right_up({codir.right_down[0], codir.left_up[1]})
	{
		mass = 10;
		type = ClassType::Block;
		inertional = false;
	}

	void drawSelf() {
		Drawer.drawCodir(codir, sf::Color::White, sf::Color::Black);
		if (show_debug) drawAllInfo();
	}
	int draw() override {
		drawSelf();
		return 1;
	}
	int action() override {
		return 0;
	}
	void addDefault() override {
		addRasBasic(OController.action_set);
	}

};

#endif