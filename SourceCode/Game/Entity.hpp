#ifndef FENGINE_SC_G_Entity
#define FENGINE_SC_G_Entity

#include "../GameCore/GameCore.hpp"

class Entity : public Basic {
public:
	XY speed;
	uft::Array<Basic *> collisions = uft::Array<Basic *>(4);
	Basic * standing = nullptr;

	int frame_count = 3;
	XY sc = {0.5, 0.5}; //coef Soft Collision

	Entity() : Basic() {
		type = ClassType::Entity;
	}

	Entity(const Codir<2> & codir) : Basic(codir) {
		type = ClassType::Entity;
		gravity = {0, 0.163 * 2};
		XY temp = codir.size();
		mass = temp[0] * temp[1];
		inertional = true;
		elastic = true;
	}

	virtual int control() {return 0;}
	int remove() override {
		removeRasBasic();
		std::cout << "Deleted " << (unsigned long long)this << std::endl;
		delete this;
		return 1;
	}
	int actionMovement() {
		std::pair<Basic *, Basic *> ret;
		int tern_count = 0;
		collisions.removeAll();
		//collision_ras.flush();

		while (distance.norm() >= 0.001) {
			if (tern_count > 10) {
				printf("tern_count > 10; Expected any symbol...\n");
				getchar();
			}
			ret = firstCollision(distance, OController.action_set, (tern_count+1)*5);
			tern_count++;

			if (ret.first == nullptr && ret.second == nullptr) {
				moveRelative(distance);
				distance = 0;
			}
			else {
				if (ret.first)  collisions.addCopy(ret.first);
				if (ret.second) collisions.addCopy(ret.second);
				if (ret.first != nullptr && ret.second == nullptr) {
					bringCloserTo(*(ret.first));
					collisionX(*(ret.first));

				}
				else if (ret.first == nullptr && ret.second != nullptr) {
					bringCloserTo(*(ret.second));
					collisionY(*(ret.second));
					if (gravity[1] * distance[1] > 0)
						standing = ret.second;
				}
				else {
					if (ret.first == ret.second) {
						if (ScI2B(XY{0, 0}, *(ret.first))) {
							printf("Fatal Intersect\n");
							printf("    obj %i with ((%lf, %lf) || (%lf, %lf)) &&", (int)((unsigned long long)(this)), codir.left_up[0], codir.left_up[1], codir.right_down[0], codir.right_down[1]);
							printf("    obj %i with ((%lf, %lf) || (%lf, %lf))\n", (int)((unsigned long long)(ret.first)), ret.first->codir.left_up[0], ret.first->codir.left_up[1], ret.first->codir.right_down[0], ret.first->codir.right_down[1]);
							XY delta = (center - ret.first->center).ort();
							while (ScI2B(XY{0, 0}, *(ret.first))) {
								if (inertional) moveRelative(delta);
								if (ret.first->inertional) ret.first->moveRelative(-delta);
							}
							//this->remove();
							continue;
						}

					}
					bringCloserToX(*(ret.first));
					bringCloserToY(*(ret.second));
					collisionX(*(ret.second));
					collisionY(*(ret.second));

				}

			}
		}
		if (standing && !collisions.indexByEquation(standing).isOk) standing = nullptr;
		return 0;
	}

	int action() override {
		v += gravity;
		control();
		speed = v;
		if (standing) {
			v[0] *= 1 - mu * mass / (100000 + mass);
			if (abs(v[0]) < 0.01) v[0] = 0;
		}
		//distance.xy.notCoDir(gravity) += standing->v.xy.notCoDir(gravity);
		//
		distance = v;

		if (actionMovement() == 1) return 0;

		animation();
		return 1;
	}


	virtual void animation() {

	}

	int draw() override {
		Drawer.drawCodir(codir, sf::Color::White, sf::Color::Transparent);
		GameWindow::window.draw(sprite);
		if (show_debug) drawAllInfo();
		return 1;
	}

	void addDefault() override {
		addRasBasic(OController.action_set);
	}
};

#endif