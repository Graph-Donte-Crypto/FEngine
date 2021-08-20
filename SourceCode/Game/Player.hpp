#ifndef FENGINE_SC_G_Player
#define FENGINE_SC_G_Player

#include "../GameCore/GameCore.hpp"
#include "Entity.hpp"

class Player : public Entity {
public:
	enum class State {
		left,
		right
	} state;

	bool folded = false;
	bool switch_frame = false;
	XY size_folded, size_normal;
	double dv_one = 0.5; //max speed = (dv_one * mu)/(1 - mu)

	inline static Textures::Record * texture_folded = nullptr;
	inline static Textures::Record * texture_unfolded = nullptr;

	Player()
		: Entity()
		, state(State::right)
	{}

	Player(const Codir<2> & codir)
		: Entity(codir)
		, state(State::right)
		, size_folded({size[0], size[1] / 2})
		, size_normal(size)
	{
		frame_count = 3;
		type = ClassType::Player;
		elastic = false;
	}
	void moveLeft() {
		v[0] -= dv_one;
		state = State::left;
		switch_frame = true;
	}
	void moveRight() {
		v[0] += dv_one;
		state = State::right;
		switch_frame = true;
	}
	void moveJump() {
		v[1] -= 10;
	}
	void moveFold() {
		if (!folded) {
			folded = true;
			setImage(texture_folded);
			frame_count = 3;
			size = size_folded;
			codir.left_up = codir.right_down + (2 * XY{EPS, EPS}) - size;
			center = codir.left_up + codir.size() / 2;
			v[1] += 3;
		}
	}

	void moveUnfold() {
		if (folded) {
			bool try_to_open = true;
			for (size_t i = 0; i < OController.action_set.length; i++) {
				bool intersect = math::checkIntersectCodirWithCodir(
					Codir<2>(codir.right_down - size_normal + 2 * XY{EPS, EPS}, codir.right_down),
					OController.action_set[i]->codir,
					EPS
				);
				if (intersect && OController.action_set[i] != this) {
					try_to_open = false;
					break;
				}
			}
			if (try_to_open) {
				folded = false;
				setImage(texture_unfolded);
				frame_count = 3;
				v[1] -= 3;

				size = size_normal;
				codir.left_up = codir.right_down + (2 * XY{EPS, EPS}) - size;
				center = codir.left_up + codir.size() / 2;
			}
		}
	}
	int control() override {
		if (Event.KeyPressing[sf::Keyboard::LShift]) moveFold();
		else moveUnfold();
		if (standing) {
			switch_frame = false;
			if (Event.KeyPressing[sf::Keyboard::D]) moveRight();
			if (Event.KeyPressing[sf::Keyboard::A]) moveLeft();
			if (Event.KeyPressing[sf::Keyboard::W]) moveJump();
		}
		if (switch_frame) frame_int++;
		return 1;
	}

	void animation() override {
		frame_int = frame_int % frame_count;
		switch (state) {
			case State::right:
				sprite.setTextureRect(sf::IntRect(frame_int * size[0] , 0, size[0], size[1]));
				break;
			case State::left:
				sprite.setTextureRect(sf::IntRect(size[0] + frame_int * size[0], 0, -size[0], size[1]));
				break;
			default:
				break;
		}
		sprite.setPosition(codir.left_up[0], codir.left_up[1]);
	}

	int draw() override {
		GameWindow::window.draw(sprite);

		if (show_debug) drawAllInfo();

		return 1;
	}
};

#endif