#ifndef FENGINE_SC_G_BlockTeleporter
#define FENGINE_SC_G_BlockTeleporter

#include "../GameCore/GameCore.hpp"
#include "Block.hpp"

class BlockTeleporter : public Block {
public:
	ClassType target = ClassType::Null;
	bool active = true;
	bool draw_teleport_zone = false;
	Basic * target_object = nullptr;
	int timeout_limit = 60;
	int timeout = 0;
	enum class TeleportForm {absolute, relative} teleportForm = TeleportForm::absolute;
	enum class TeleportType {center, intersect} teleportType = TeleportType::center;

	XY delta;
	Codir<2> codir_teleport;

	BlockTeleporter(const Codir<2> & codir) : Block(codir) {
		type = ClassType::BlockTeleporter;
	}

	BlockTeleporter() : Block() {
		type = ClassType::BlockTeleporter;
	}

	bool _checkCenterCase(const Basic * ptr) {
		return (
			teleportType == TeleportType::center &&
			ptr->center >= codir_teleport.left_up &&
			ptr->center <= codir_teleport.right_down
		);
	}
	bool _checkIntersectCase(const Basic * ptr) {
		return
			teleportType == TeleportType::intersect &&
			math::checkIntersectCodirWithCodir(codir_teleport, ptr->codir, EPS);
	}

	int action() override {
		if (timeout) timeout -= 1;
		if (active && !timeout) {
			for (size_t i = 0; i < OController.action_set.length; i++) {
				Basic * ptr = OController.action_set[i];
				if (( target == ptr->type || ptr == target_object) &&
					(_checkCenterCase(ptr) || _checkIntersectCase(ptr))
					) {
					if (teleportForm == TeleportForm::absolute)
						ptr->moveRelative(delta - ptr->center);
					else
						ptr->moveRelative(delta);
					timeout = timeout_limit;
				}
			}
		}
		return 0;
	}
	void drawTeleportZone() {
		Drawer.drawCodir(codir_teleport, sf::Color::Red, sf::Color::Transparent);
		XY temp = codir_teleport.center();
		if (teleportForm == TeleportForm::absolute) {
			Drawer.drawLine(Line<2>(temp, delta), sf::Color::Green);
		}
		else {
			Drawer.drawLine(Line<2>(temp, temp + delta), sf::Color::Red);
			Drawer.drawCodir(codir_teleport + delta, sf::Color::Green, sf::Color::Transparent);
		}
	}
	int draw() override {
		drawSelf();
		if (draw_teleport_zone) drawTeleportZone();
		return 1;
	}
};

#endif