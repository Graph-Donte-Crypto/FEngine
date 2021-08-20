#ifndef FENGINE_SC_G_Basic
#define FENGINE_SC_G_Basic

#include "SFML/Graphics.hpp"

#include "UseFull/Math/Shape.hpp"
#include "UseFull/Math/Intersect.hpp"
#include "UseFull/Templates/Ras.hpp"
#include "UseFull/SFMLUp/Drawer.hpp"

#include "ClassType.hpp"
#include "Textures.hpp"
#include "GameWindow.hpp"

using namespace math;
using namespace uft;
using namespace sfup;

#define EPS 0.001

class Basic {
public:

	int id = 0;
	ClassType type = ClassType::Basic;

	Codir<2> codir = Codir<2>({0, 0}, {0, 0});
	XY center = {0, 0};
	bool solid = true;
	bool inertional = false;
	bool elastic = false;
	bool show_debug = false;

	//Физические переменные
	double mass = 0;
	double mu = 0.95;
	XY v = {0, 0};

	//Конец физических переменных

	XY gravity = {0, 0.1};

	XY distance = {0, 0};

	inline static Textures::Record * texture_default = nullptr;
	Textures::Record * texture_record = nullptr;

	sf::Sprite sprite;

	unsigned short frame_int = 0;

	XY size;

	RasRec<Basic> * ras_record;
	Basic()
		: size({0, 0})
        , ras_record(nullptr)
	{}
	Basic(const Codir<2> & _codir)
		: codir(_codir.left_up + XY{EPS, EPS}, _codir.right_down - XY{EPS, EPS})
        , center(_codir.center())
        , size(_codir.size())
        , ras_record(nullptr)
    {}

	virtual void addDefault() {
	}

	virtual void moveRelative(const XY & delta) {
		codir += delta;
		center += delta;
	}

	void setImage(Textures::Record * record) {
		texture_record = record;
		sprite.setTexture(texture_record->texture);
		sprite.setTextureRect(sf::IntRect(0, 0, size[0], size[1]));
	}

/*
	void setImageString(int i) {
		image_int = i;
		texture = Textures::set[i];
		sprite.setTexture(*texture);
		sprite.setTextureRect(sf::IntRect(0, 0, size[0], size[1]));
	}
*/

	virtual void setDefaultImage() {
		setImage(texture_default);
	}

	virtual void textAllInfo() {
		Drawer.Text << "\nid = " << id;
		Drawer.Text << "\npointer = " << this;
		Drawer.Text << "\ntype = " << type;
		Drawer.Text << "\nmass = " << mass;
		Drawer.Text << "\ncenter = " << center[0] << " " << center[1];
		Drawer.Text << "\nleft_up = " << codir.left_up[0] << " " << codir.left_up[1];
		Drawer.Text << "\nright_down = " << codir.right_down[0] << " " << codir.right_down[1];
		Drawer.Text << "\nsolid = " << solid;
		Drawer.Text << "\ninertional = " << inertional;
		Drawer.Text << "\nelastic = " << elastic;
		Drawer.Text << "\ngravity = " << gravity[0] << " " << gravity[1];

		Drawer.Text << "\nv = " << v[0] << " " << v[1];
	}

	virtual void drawAllInfo() {
		if (show_debug) {
			textAllInfo();
			Drawer.drawText({codir.right_down[0], codir.left_up[1]});
		}
	}

	void printAllInfo() {
		printf("type %s\n", (const char *)type);
		printf("mass %lf\n", mass);
		printf("center %lf %lf\n", center[0], center[1]);
		printf("left_up %lf %lf\n", codir.left_up[0], codir.left_up[1]);
		printf("right_down %lf %lf\n", codir.right_down[0], codir.right_down[1]);

		printf("solid %i\n", solid);
		printf("inertional %i\n", inertional);
		printf("elastic %i\n", elastic);

		printf("gravity %lf %lf\n", gravity[0], gravity[1]);

		printf("v %lf %lf\n", v[0], v[1]);
		printf("distance %lf %lf\n", distance[0], distance[1]);
		printf("size %lf %lf\n", size[0], size[1]);
		printf("mu %lf\n", mu);
	}

	void addRasBasic(Ras<Basic> & set) {ras_record = set.add(this);}
	void removeRasBasic() {if (ras_record != nullptr) ras_record = ras_record->remove();}

	virtual int action() { return 0; }
	virtual int draw() {
		Drawer.drawCodir(codir, sf::Color::White, sf::Color::Transparent);
		GameWindow::window.draw(sprite);
		return 1;
	}

	virtual int remove() {
		removeRasBasic();
		delete this;
		return 0;
	}

	size_t bisection_iteration_normal = 64;
	size_t bisection_iteration_special = 16;

	bool ScI2B(const XY & xy, const Basic & o) {
		return math::checkIntersectCodirWithCodir(codir + xy, o.codir, EPS);
	}

	std::pair<Basic *, Basic *> firstCollision(const XY dxy, const Ras<Basic> & set, size_t sens) {
		XY move; move = 0;
		XY d = dxy/sens;
		Basic * p1 = nullptr;
		Basic * p2 = nullptr;
		for (size_t i = 0; i < sens; i++) {
			move += d;
			for (size_t j = 0; j < set.length; j++) {
				if ( this != set[j] && ScI2B(move, *set[j]) ) {
					move -= d;
					goto stage2;
				}
			}
		}
		return std::make_pair(p2, p1);

	stage2:

		XY buf;
		for (size_t i = 0; i < set.length; i++) {
			if (this != set[i]) {
				buf = {d[0], 0};
				if ( ScI2B(move + buf, *set[i]) ) p2 = set[i];//for x
				buf = {0, d[1]};
				if ( ScI2B(move + buf, *set[i]) ) p1 = set[i];//for y
			}
		}
		return std::make_pair(p2, p1);
	}

	//return 0 if no collision
	//return 1 if collision dy
	//return 2 if collision dx
	//return 3 if collision dx && dy



	void bringCloserToX(const Basic & obj) {
		XY delta_x = {distance[0], 0};

		if (ScI2B(delta_x, obj)) {
			double a = 0;
			double b = delta_x[0];

			delta_x[0] *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(delta_x, obj)) b = delta_x[0];
				else a = delta_x[0];
				delta_x[0] = (a + b) / 2;
			}
			size_t i;
			for (i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(delta_x, obj)) {
					b = delta_x[0];
					delta_x[0] = (a + b) / 2;
				}
				else break;
			}
			if (i == bisection_iteration_special) delta_x[0] = 0;
		}

		distance[0] = delta_x[0];

		if (ScI2B(distance, obj)) {

			XY a = {0, 0};
			XY b = distance;

			distance *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else {
					a = distance;
					distance = (a + b) / 2;
				}
			}


			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special - 1) {
					distance = XY{0, 0};
				}
			}

		}
	}

	void bringCloserToY(const Basic & obj) {
		XY delta_y = {0, distance[1]};

		if (ScI2B(delta_y, obj)) {
			double a = 0;
			double b = delta_y[1];

			delta_y[1] *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(delta_y, obj)) {
					b = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
				else {
					a = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
			}
			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(delta_y, obj)) {
					b = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special - 1) {
					delta_y[1] = 0;
				}
			}
		}

		distance[1] = delta_y[1];

		if (ScI2B(distance, obj)) {

			XY a = {0, 0};
			XY b = distance;

			distance *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else {
					a = distance;
					distance = (a + b) / 2;
				}
			}


			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special - 1) {
					distance = XY{0, 0};
				}
			}

		}
	}

	void bringCloserTo(const Basic & obj) {
		XY delta_x = {distance[0], 0};
		XY delta_y = {0, distance[1]};

		if (ScI2B(delta_y, obj)) {
			double a = 0;
			double b = delta_y[1];

			delta_y[1] *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(delta_y, obj)) {
					b = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
				else {
					a = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
			}
			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(delta_y, obj)) {
					b = delta_y[1];
					delta_y[1] = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special-1) {
					delta_y[1] = 0;
				}
			}
		}

		if (ScI2B(delta_x, obj)) {
			double a = 0;
			double b = delta_x[0];

			delta_x[0] *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(delta_x, obj)) {
					b = delta_x[0];
					delta_x[0] = (a + b) / 2;
				}
				else {
					a = delta_x[0];
					delta_x[0] = (a + b) / 2;
				}
			}
			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(delta_x, obj)) {
					b = delta_x[0];
					delta_x[0] = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special-1) {
					delta_x[0] = 0;
				}
			}
		}

		distance = {delta_x[0], delta_y[1]};

		if (ScI2B(distance, obj)) {

			XY a = {0, 0};
			XY b = distance;

			distance *= 0.5;

			for (size_t i = 0; i < bisection_iteration_normal; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else {
					a = distance;
					distance = (a + b) / 2;
				}
			}


			for (size_t i = 0; i < bisection_iteration_special; i++) {
				if (ScI2B(distance, obj)) {
					b = distance;
					distance = (a + b) / 2;
				}
				else break;
				if (i == bisection_iteration_special-1) {
					distance = XY{0, 0};
				}
			}

		}
	}

	int collisionX(Basic & obj) {
		double k = 0;
		if (elastic) k += 0.5;
		if (obj.elastic) k += 0.5;
		double & v1 = v[0];
		double & v2 = obj.v[0];

		double nv1 = v1;
		double nv2 = v2;

		if (obj.inertional == false) {
			nv1 = v1 - (1 + k) * (v1 - v2);
		}
		else if (inertional == false) {
			nv2 = v2 + (1 + k) * (v1 - v2);
		}
		else {
			nv1 = v1 - (1 + k) * (obj.mass / (mass + obj.mass)) * (v1 - v2);
			nv2 = v2 + (1 + k) * (mass     / (mass + obj.mass)) * (v1 - v2);
		}

		v1 = nv1;
		v2 = nv2;

		return 0;
	}

	int collisionY(Basic & obj) {

		double k = 0;
		if (elastic) k += 0.5;
		if (obj.elastic) k += 0.5;
		//k = 0.5
		double & v1 = v[1];
		double & v2 = obj.v[1];

		double nv1 = v1;
		double nv2 = v2;

		if (obj.inertional == false) {
			nv1 = v1 - (1 + k) * (v1 - v2);
		}
		else if (inertional == false) {
			nv2 = v2 + (1 + k) * (v1 - v2);
		}
		else {
			nv1 = v1 - (1 + k) * (obj.mass / (mass + obj.mass)) * (v1 - v2);
			nv2 = v2 + (1 + k) * (mass     / (mass + obj.mass)) * (v1 - v2);
		}

		v1 = nv1;
		v2 = nv2;

		return 0;
	}
	virtual ~Basic() {

	}
};


#endif