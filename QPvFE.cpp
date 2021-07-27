#include "UseFull/SFMLUp/View.hpp"
#include <queue>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <list>
#include <iterator>
#include <cassert>
#include <functional>
#include <chrono>
#include <thread>

#include <stdlib.h>
#include <cctype>

#include <UseFull/Math/Vector.hpp>
#include <UseFull/Math/Intersect.hpp>
#include <UseFull/Utils/Random.hpp>
#include <UseFull/Utils/Stream.hpp>
#include <UseFull/SFMLUp/Drawer.hpp>
#include <UseFull/SFMLUp/Event.hpp>
#include <UseFull/SFMLUp/Mouse.hpp>
#include <UseFull/SFMLUp/Fonts.hpp>
#include <UseFull/SFMLUp/View.hpp>
#include <UseFull/Templates/Array.hpp>
#include <UseFull/Templates/Ras.hpp>

#include <UseFull/SFMLUp/GUI/Frame.hpp>
#include <UseFull/SFMLUp/GUI/FocusTracker.hpp>
#include <UseFull/SFMLUp/GUI/Button.hpp>

using namespace sfup;
using namespace sfup::gui;
using namespace uft;

bool debug = false;

struct GlobalStruct {
	const char * window_name = "QP version FE 12";
	sf::RenderWindow window;

	size_t limit_fps = 60;

	sf::Clock clock;
	double current_time;
	double current_fps;

	size_t window_width = 800;
	size_t window_height = 600;

	size_t video_mode = sf::Style::Close;

	void calcFps() {
		current_time = clock.restart().asSeconds();
        current_fps = (double)1.0 / current_time;
	}

	void createWindow(size_t mode) {
		window.create(sf::VideoMode(window_width, window_height), window_name, mode);
		window.setFramerateLimit(limit_fps);
	}

	void loadWindowSettings() {

	}

	GlobalStruct()
        : current_time(0)
        , current_fps(0)
    {
		//createWindow(sf::Style::Fullscreen);
		//createWindow(sf::Style::Close);
	}

	bool checkIntersect2Codir(const Codir<2> & a, const Codir<2> & b) {

#define EPS 0.001
		bool fline13 = ( (a.left_up[0] < b.right_down[0] + EPS) && (a.left_up[0] + EPS > b.left_up[0] || a.right_down[0] + EPS > b.right_down[0]) ) || ( (b.left_up[0] < a.right_down[0] + EPS) && (a.left_up[0] < b.left_up[0] + EPS || a.right_down[0] < b.right_down[0] + EPS) );
		bool fline24 = ( (a.left_up[1] < b.right_down[1] + EPS) && (a.left_up[1] + EPS > b.left_up[1] || a.right_down[1] + EPS > b.right_down[1]) ) || ( (b.left_up[1] < a.right_down[1] + EPS) && (a.left_up[1] < b.left_up[1] + EPS || a.right_down[1] < b.right_down[1] + EPS) );
#undef EPS

		return fline24 && fline13;
	}
}
Global;

//-lWs2_32 -liphlpapi

class Basic;

#define EPS 0.001

bool show_debug = false;

struct TexturesClass {
public:

	/*
		TODO: сделать бинарное дерево соответствий
			строка -> текстура
	*/

	/*

	//Нужен метод
	//Type & Array::newObjectPlace()
	//  если нужно - увеличить память, после вернуть ссылку на новую запись

	Array<sf::Texture> textures;
	Array<sf::String> paths;
	Array<sf::Image> images;

	void addRecord(const sf::String & string) {
		paths.addCopy(string);

		Image & image = images.newObjectPlace();
		image = Image();
		image.loadFromFile(string);

		Texture & texture = textures.newObjectPlace();
		texture = Texture();
		texture.loadFromImage(image);
	}

	void recursiveTexuresFind(const char * path) {
		RecFileFinder rff = RecFileFinder(path);
		while (rff.exist) {
			RecFileFinderObject object = rff.getNextObject();
			if (strcmp(object.afterdot, "png") == 0)
				addRecord(sf::String(object.path_relative))
		}
	}

	Texture getTextureByPath(const char * path) {

	}

	*/

	Ras<sf::Texture> set;
	Ras<sf::String> paths;
	Ras<sf::Image> images;

	void load() {
        paths.add(new sf::String("source/void.png"));
		paths.add(new sf::String("source/robot-open.png"));
		paths.add(new sf::String("source/robot-closed.png"));

		for (size_t i = 0; i < paths.length; i++) {
			std::cout << paths[i]->toAnsiString() << '\n';
			sf::Texture * texture = new sf::Texture();
			sf::Image * image = new sf::Image();
			image->loadFromFile(*(paths[i]));
			texture->loadFromImage(*image);
			set.add(texture);
			images.add(image);
		}
	}

}
Textures;

struct ClassType {

	const char * ptr = nullptr;

	#define Type(type) type = #type

	static constexpr const char
		* Type(Null),
		* Type(Basic),
		* Type(Entity),
		* Type(Block),
		* Type(BlockTeleporter),
		* Type(Player),
		* Type(NetworkPlayer)
		;

	#undef Type

	ClassType(const char * p) : ptr(p) {}
	operator const char * () const { return ptr;}
	ClassType & operator = (const char * p) { ptr = p; return *this;}
	bool operator == (const ClassType & type) { return ptr == type.ptr;};
	bool operator != (const ClassType & type) { return ptr != type.ptr;};
};

ClassType param = ClassType::Null;

class Basic {
public:
	int id = 0;
	int default_image = 0;
	ClassType type = ClassType::Basic;

	Codir<2> codir = Codir<2>({0, 0}, {0, 0});
	XY center = {0, 0};
	bool solid = true;
	bool inertional = false;
	bool elastic = false;

	//Физические переменные
	double mass = 0;
	double mu = 0.95;
	XY v = {0, 0};

	//Конец физических переменных

	XY gravity = {0, 0.1};

	XY distance = {0, 0};

	sf::Texture texture;
	sf::Sprite sprite;

	unsigned short frame_int = 0;
	unsigned short image_int = 0;

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
	void setImageString(int i) {
		image_int = i;
		texture = sf::Texture(*(Textures.set[i]));
		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, size[0], size[1]));
	}

	virtual void setDefaultImage() {
		setImageString(default_image);
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
		Drawer.drawCodir(codir, sf::Color::White);
		Global.window.draw(sprite);
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
		return Global.checkIntersect2Codir(codir + xy,	o.codir);
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

static
class ActionStack {
public:

	Ras<Basic  > action_set;
	Ras<BaseGui> action_win_set;
	size_t action_count, draw_count;
	size_t action_i = 0, action_win_i = 0;

	ActionStack()
	    : action_count(0)
	    , draw_count(0)
    {
		action_set.current = &action_i;
		action_win_set.current = &action_win_i;
	}
	~ActionStack() {
		for (size_t i = 0; i < action_set.length; i++) {
			delete action_set[i];
		}
		for (size_t i = 0; i < action_win_set.length; i++) {
			delete action_win_set[i];
		}
	}
	void stepAction() {
	    Mouse.getPosition();

		action_count = 0;
		for (action_i = 0; action_i < action_set.length; action_i++) {
			action_count += action_set[action_i]->action();
		}
		action_set._fixArray();

	}
	void stepActionWin() {
        Mouse.getPosition();
        FocusTracker::nextTurn();

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
		Drawer.Text << "Count(A|D): " << action_count << " | " << draw_count;
		Drawer.drawText({0, 20});
		Drawer.drawText("ACTIVE: Ubuntu::Mono::Regular", {0, 40});
		Drawer.Text << "focus: " << FocusTracker::focus << " | " << "focus on next turn: " << FocusTracker::focus_next_turn;
		Drawer.drawText({0, 60});
	}

	void stepDrawWin() {
		for (size_t i = 0; i < action_win_set.length; i++) {
			action_win_set[i]->drawSelf();
			action_win_set[i]->drawTo(Global.window);
		}
	}
}
AStack;

//Engine part

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
			ret = firstCollision(distance, AStack.action_set, (tern_count+1)*5);
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
		Drawer.drawCodir(codir, sf::Color::White);
		Global.window.draw(sprite);
		if (show_debug) drawAllInfo();
		return 1;
	}

	void addDefault() override {
		addRasBasic(AStack.action_set);
	}
};

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

	Player()
        : Entity()
        , state(State::right)
    {
        default_image = 1;
    }

	Player(const Codir<2> & codir)
        : Entity(codir)
        , state(State::right)
        , size_folded({size[0], size[1] / 2})
        , size_normal(size)
    {
        frame_count = 3;
        type = ClassType::Player;
        elastic = false;
        default_image = 1;
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
			setImageString(2);
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
			for (size_t i = 0; i < AStack.action_set.length; i++) {
				bool intersect = Global.checkIntersect2Codir(
					Codir<2>(codir.right_down - size_normal + 2 * XY{EPS, EPS}, codir.right_down),
					AStack.action_set[i]->codir
				);
				if (intersect && AStack.action_set[i] != this) {
					try_to_open = false;
					break;
				}
			}
			if (try_to_open) {
				folded = false;
				setImageString(1);
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
		Global.window.draw(sprite);

		if (show_debug) drawAllInfo();

		return 1;
	}
};

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
		Drawer.drawCodirFilled(codir, sf::Color::White, sf::Color::Black);
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
		addRasBasic(AStack.action_set);
	}

};

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
			Global.checkIntersect2Codir(codir_teleport, ptr->codir);
	}

	int action() override {
		if (timeout) timeout -= 1;
		if (active && !timeout) {
			for (size_t i = 0; i < AStack.action_set.length; i++) {
				Basic * ptr = AStack.action_set[i];
				if (( ptr->type == target  || ptr == target_object) &&
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
		Drawer.drawCodir(codir_teleport, sf::Color::Red);
		XY temp = codir_teleport.center();
		if (teleportForm == TeleportForm::absolute) {
			Drawer.drawLine(Line<2>(temp, delta), sf::Color::Green);
		}
		else {
			Drawer.drawLine(Line<2>(temp, temp + delta), sf::Color::Red);
			Drawer.drawCodir(codir_teleport + delta, sf::Color::Green);
		}
	}
	int draw() override {
		drawSelf();
		if (draw_teleport_zone) drawTeleportZone();
		return 1;
	}
};

class Window : public BaseGui {
public:
	//ctrlm -> control_menu
	double ctrlm_paddind = 20;
	XY moveble_center = XY{0, 0};

	Window(const Codir<2> & codir)
        : BaseGui(codir)
    {
		FocusTracker::codir_focus = Codir<2>({0, 0}, {codir.size()[0], ctrlm_paddind});
	}
	~Window() {
		elements.flushHard();
	}
	void drawSelf() override {

		color_background = sf::Color(255, 0, 0, 255);

		BaseGui::drawSelf();

		Drawer.drawLine(
			*this,
			Line<2>({0, ctrlm_paddind}, {codir.size()[0], ctrlm_paddind}),
			color_outline
		);

		sf::Font temp_font;
		sf::Text text("", *Fonts.getByName("UbuntuMono-R").valueOr(&temp_font), 14);
		text.setPosition(0, 0);
		text.setString("Main Menu");
		text.setFillColor(sf::Color::Black);

		this->draw(text);

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
		moveble_center = codir.center() - Mouse.inWorld;
	}
	void moveRelative(const XY & delta) override {
		BaseGui::moveRelative(delta);
	}
	void actionPressing() override {
		moveRelative(moveble_center - codir.center() + Mouse.inWorld);
	}
	void action() override {
		if (active) {
			actionForElements();
			FocusTracker::focus_offset = getAbsoluteOffset();
			checkFocus(Mouse.inWorld);
		}
	}
};

enum class MainLoopType {server, client, resume, init};

Player * p1 = nullptr;

void initLevel1() {

	Entity * en2 = nullptr;
	en2 = new Entity(Codir<2>({0, 0}, {50, 100}));
	en2->addDefault();
	en2->moveRelative({500,-600});

	p1 = new Player(Codir<2>({150, 100}, {200, 200}));
	p1->addDefault();
	p1->moveRelative({350, 0});
	p1->setImageString(1);

	Block * block;
	block = new Block(Codir<2>({100, 201}, {500, 300}));
	block->addDefault();

	block = new Block(Codir<2>({50, 800}, {15000, 900}));
	block->addDefault();

	BlockTeleporter * lift = new BlockTeleporter(Codir<2>({600, 800}, {700, 900}));

	lift->target_object = p1;
	lift->target = ClassType::NetworkPlayer;
	lift->teleportForm = BlockTeleporter::TeleportForm::absolute;
	lift->teleportType = BlockTeleporter::TeleportType::center;
	lift->delta = {700, -500};
	lift->codir_teleport = Codir<2>(lift->codir.left_up + XY{0, -300}, lift->right_up);
	lift->timeout_limit = 60;
	lift->active = true;
	lift->draw_teleport_zone = true;

	lift->addRasBasic(AStack.action_set);

	block = new Block(Codir<2>({0, -500}, {50, 900}));
	block->addDefault();

	WorldView.followUp(p1->center);
}

bool mainLoop(MainLoopType loopType) {
	sf::Clock clock;
	sf::Font temp_font;
	sf::Text text("", *Fonts.getByName("UbuntuMono-R").valueOr(&temp_font), 14);
	text.setPosition(0, 30);
	std::ostringstream fps_stream;
	double currentTime = 0;
	double fps = 0;

	if (loopType == MainLoopType::init) initLevel1();

	Entity * entity = nullptr;

	sf::RenderWindow & window    = Global.window;
	size_t           & limit_fps = Global.limit_fps;

	while (window.isOpen()) {
		//EventKeeper processor
		Event.flush();
		Event.load(window);

		if (Event.KeyPressing[sf::Keyboard::Add]) {
			limit_fps++;
			window.setFramerateLimit(limit_fps);
		}
		else if (Event.KeyPressing[sf::Keyboard::Subtract]) {
			if (limit_fps > 1) limit_fps -= 1;
			window.setFramerateLimit(limit_fps);
		}
		if (Event.KeyPressed[sf::Keyboard::Q]) {
			XY tlu = {
				randomFromInterval<double>(50, 150),
				randomFromInterval<double>(50, 150)
			};
			XY trd = tlu;
			trd += {
				randomFromInterval<double>(50, 100),
				randomFromInterval<double>(50, 100)
			};
			entity = new Entity(Codir<2>(tlu, trd));
			entity->addDefault();
			entity->moveRelative({
				randomFromInterval<double>(300, 400),
				-randomFromInterval<double>(0, 100)
			});
		}
		if (Event.KeyPressed[sf::Keyboard::E]) {
			if (entity != nullptr) {
				entity->remove();
				entity = nullptr;
				AStack.action_set._fixArray();
			}
		}
		if (Event.KeyPressed[sf::Keyboard::Escape]) return true;
		AStack.stepAction();

		if (Event.KeyPressed[sf::Keyboard::F1])
			show_debug = !show_debug;

		WorldView.update();

		//Clear screen
		window.clear(sf::Color::Black);

		//Draw Processor
		WorldView.use();
		AStack.stepDraw();
		for (size_t i = 0; i < 100; i++) Drawer.drawText(i*50, {(double)(i*50), 500});

		currentTime = clock.restart().asSeconds();
		fps = (double)1.0 / currentTime;


		fps_stream << fps;
		text.setString("FPS: " + fps_stream.str());

		window.draw(text);
		fps_stream.str("");
		fps_stream.clear();

		//Display part
		window.display();
	}

	return 0;
}

struct MenuState {
	bool active = true;
	bool pause = true;
	XY current_view;
	Window * main_window = nullptr;
};

enum class UbermenuType{main, ingame};
int ubermenu(UbermenuType type, bool active, bool pause) {
	MenuState state;

	state.active = active;
	state.pause = pause;
	state.current_view = {400, 300};

	if (type == UbermenuType::main) {
		Window * main_window = new Window(Codir<2>({0, 0}, {200, 400}));
		main_window->unique_name = "main window";
		main_window->moveRelative({300, 100});
		main_window->addParent(AStack.action_win_set);



		Button * single_player = new Button(Codir<2>({0, 0}, {150, 20}));
		single_player->unique_name = "single_player";
		single_player->text.setString("Single Player Game");
		single_player->centerTheText();
		//single_player->moveRelative(main_window->codir.left_up);
		single_player->moveRelative({25, 40});
		single_player->lambda = [&state] () mutable {
			if (state.active) state.pause = mainLoop(MainLoopType::resume);
			else state.pause = mainLoop(MainLoopType::init);
			state.active = true;
			XY current_center = GuiView.current_view.center();
			for (size_t i = 0; i < AStack.action_win_set.length; i++)
				AStack.action_win_set[i]->moveRelative(current_center - state.current_view);
			state.current_view = current_center;
		};
		main_window->addChild(single_player);


		Button * setting = new Button(Codir<2>({0, 0}, {150, 20}));
		setting->unique_name = "settings";
		setting->text.setString("Setting");
		setting->centerTheText();
		//setting->moveRelative(main_window->codir.left_up);
		setting->moveRelative({25, 80});
		setting->lambda = []() {
			std::chrono::seconds sec(1);
			std::this_thread::sleep_for(sec);
		};
		main_window->addChild(setting);



		Window * exit_window = new Window(Codir<2>({0, 0}, {150, 120}));
		exit_window->unique_name = "exit_window";
		exit_window->moveRelative({300, 100});

		Button * exit_button_yes = new Button(Codir<2>({0, 0}, {100, 20}));
		exit_button_yes->unique_name = "exit button yes";
		exit_button_yes->text.setString("Yes");
		exit_button_yes->centerTheText();
		//exit_button_yes->moveRelative(exit_window->codir.left_up);
		exit_button_yes->moveRelative({20, 40});
		exit_button_yes->lambda = []() {
			exit(1);
		};
		exit_window->addChild(exit_button_yes);

		Button * exit_button_no = new Button(Codir<2>({0, 0}, {100, 20}));
		exit_button_no->unique_name = "exit button no";
		exit_button_no->text.setString("Back");
		exit_button_no->centerTheText();
		//exit_button_no->moveRelative(exit_window->codir.left_up);
		exit_button_no->moveRelative({20, 80});
		exit_button_no->lambda = [main_window, exit_window]() {
			Ras<BaseGui> * ras = exit_window->ras_record->storage;
			exit_window->removeParent();
			main_window->addParent(*ras);
		};
		exit_window->addChild(exit_button_no);



		Button * exit = new Button(Codir<2>({0, 0}, {150, 20}));
		exit->unique_name = "exit";
		exit->text.setString("Exit");
		exit->centerTheText();
		//exit->moveRelative(main_window->codir.left_up);
		exit->moveRelative({25, 120});
		//вылетает потому что main_window и exit_window уже не существует
		exit->lambda = [main_window, exit_window]() {
			Ras<BaseGui> * ras = main_window->ras_record->storage;
			main_window->removeParent();
			exit_window->addParent(*ras);
		};
		main_window->addChild(exit);

		state.main_window = main_window;
	}

	sf::RenderWindow & window = Global.window;

	Button * button = (Button *) state.main_window->getByName("single_player").valueOr(nullptr);

	while (window.isOpen()) {
		//EventKeeper processor
		Event.flush();
		Event.load(window);
		if (Event.KeyPressed[sf::Keyboard::Escape] && state.active) {
			auto element = state.main_window->getByName("single_player");
			if (element.isOk) element.value->actionRealized();
		}

		if (Event.KeyPressed[sf::Keyboard::B])
            debug = !debug;

		//Action processor
		if (!pause) AStack.stepAction();
        AStack.stepActionWin();

		WorldView.update();

		//Clear screen
		window.clear(sf::Color::Green);

		//Draw Processor
		WorldView.use();
		AStack.stepDraw();

		GuiView.use();
		AStack.stepDrawWin();

		//Display part
		window.display();
	}
	return 0;
}

struct Flag {
    std::vector<const char *> name_variants;
    const char * desc = nullptr;
    size_t (*value_func)(const char * const *, size_t) = nullptr;
    void   (*empty_func)(             ) = nullptr;
    Flag(std::initializer_list<const char *> list, const char * _desc, size_t (*_value_func)(const char * const *, size_t))
        : desc(_desc)
        , value_func(_value_func)
        , empty_func(nullptr)
    {
        name_variants.insert(name_variants.end(), list.begin(), list.end());
    }
    Flag(std::initializer_list<const char *> list, const char * _desc, void (*_empty_func)())
        : desc(_desc)
        , value_func(nullptr)
        , empty_func(_empty_func)
    {
        name_variants.insert(name_variants.end(), list.begin(), list.end());
    }
    Flag()
        : desc(nullptr)
        , value_func(nullptr)
        , empty_func(nullptr)
    {}

    bool is(const char * value) {
        for (size_t i = 0; i < name_variants.size(); i++)
            if (strcmp(name_variants[i], value) == 0) return true;
        return false;
    }

    std::string getNameVariants() {
        if (name_variants.size() > 0) {
            std::string first = "[" + std::string(name_variants[0]) + "]";
            return utils::GetStream<const char *>(name_variants)
                .Ignore(1)
                .Reduce<std::string>([](auto p, auto x){ return p + " | [" + x + "]";}, first);
        }
        else return "";
    }

    size_t invoke(const char * const * argv, size_t left) {
        if (value_func) return value_func(argv, left);
        if (empty_func) empty_func();
        return 0;
    }
};

struct FlagParser {
    size_t argc;
    char * const * argv;
    std::vector<Flag> flags;

    FlagParser(size_t _argc, char * const * _argv)
        : argc(_argc)
        , argv(_argv)
    {}

    void execute() {
        for (size_t i = 1; i < argc; i++) {
            bool is = false;
            for (size_t j = 0; j < flags.size(); j++) {
                if (flags[j].is(argv[i])) {
                    i += flags[j].invoke(argv + i, argc - i - 1);
                    is = true;
                    break;
                }
            }
            if (!is) printf("flag [%s] unresolved\n", argv[i]);
        }
    }
}
* FlagParserAutoHelpSource = nullptr;

void FlagParserAutoHelp() {
    FlagParser & p = *FlagParserAutoHelpSource;

    utils::GetStream<Flag>(p.flags)
        .ForAll([](Flag & flag) { printf("%s => %s\n", flag.getNameVariants().c_str(), flag.desc); })
    ;
    exit(0);
}

int main(int argc, char * argv[]) {

    FlagParser parser(argc, argv);
    FlagParserAutoHelpSource = &parser;

    auto & flags = parser.flags;
    flags.push_back(Flag(
        {"-help", "--help", "-h", "--h", "?", "-?", "help"},
        "show all flags and it's descriptions",
        FlagParserAutoHelp
    ));
    flags.push_back(Flag(
        {"-width"},
        "set game window width. Expect one int parameter: width",
        [](const char * const * argv, size_t left) -> size_t {
            if (left < 1) {
                printf("flag [%s] expected one parameter", argv[0]);
                exit(0);
            }
            Global.window_width  = std::stoi(argv[1]);
            return 1;
        }
    ));
    flags.push_back(Flag(
        {"-height"},
        "set game window height. Expect one int parameter: height",
        [](const char * const * argv, size_t left) -> size_t {
            if (left < 1) {
                printf("flag [%s] expected one parameter", argv[0]);
                exit(0);
            }
            Global.window_height = std::stoi(argv[1]);
            return 1;
        }
    ));
    flags.push_back(Flag(
        {"-fps"},
        "set fps for game window. Expect one int parameter: fps count",
        [](const char * const * argv, size_t left) -> size_t {
            if (left < 1) {
                printf("flag [%s] expected one parameter", argv[0]);
                exit(0);
            }
            Global.limit_fps = std::stoi(argv[1]);
            return 1;
        }
    ));

    parser.execute();

    Global.createWindow(sf::Style::Close);
	Drawer.target = &Global.window;
	Mouse.window  = &Global.window;
	WorldView.reset(&Global.window, Global.window_width, Global.window_height);
	GuiView.reset(&Global.window, Global.window_width, Global.window_height);

	Textures.load();

	Fonts.load("source/verdana.ttf", "verdana");
	Fonts.load("ubuntumono/UbuntuMono-R.ttf" , "UbuntuMono-R" );
	Fonts.load("ubuntumono/UbuntuMono-RI.ttf", "UbuntuMono-RI");
	Fonts.load("ubuntumono/UbuntuMono-B.ttf" , "UbuntuMono-B" );
	Fonts.load("ubuntumono/UbuntuMono-BI.ttf", "UbuntuMono-BI");

	Fonts.setFontToText("UbuntuMono-R", Drawer.text);

	ubermenu(UbermenuType::main, false, true);

	return 0;
}
