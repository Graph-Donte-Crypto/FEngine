//Includes From std lib
#include "UseFull/Utils/StdDiagnosticIgnore.hpp"

#include <cstdlib>
#include <thread>
#include <chrono>

#include "UseFull/Utils/StdDiagnosticIgnoreEnd.hpp"

//Includes from UseFull
#include <UseFull/SFMLUp/GUI/Button.hpp>
#include <UseFull/Utils/Random.hpp>
#include <UseFull/Utils/Stream.hpp>
#include "UseFull/Utils/FlagParser.hpp"

//Includes From GameCore
#include "SourceCode/GameCore/GameCore.hpp"

//Other Game Includes
#include "SourceCode/Window.hpp"
#include "SourceCode/Game/Entity.hpp"
#include "SourceCode/Game/Player.hpp"
#include "SourceCode/Game/Block.hpp"
#include "SourceCode/Game/BlockTeleporter.hpp"

using namespace sfup;
using namespace sfup::gui;
using namespace uft;

//-lWs2_32 -liphlpapi

class Basic;

#define EPS 0.001

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
	p1->setImage(p1->texture_unfolded);

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

	lift->addRasBasic(OController.action_set);

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

	sf::RenderWindow & window    = GameWindow::window;
	size_t           & limit_fps = GameWindow::limit_fps;

	Label label(Codir<2>({0, 0}, {200, 200}));
	label.setTextColor(sf::Color::White);
	label.setText("Hello label");

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
				OController.action_set._fixArray();
			}
		}
		if (Event.KeyPressed[sf::Keyboard::Escape]) return true;
		OController.stepAction();

		if (Event.KeyPressed[sf::Keyboard::F1])
			OController.debug = !OController.debug;

		WorldView.update();

		//Clear screen
		window.clear(sf::Color::Black);

		//Draw Processor
		WorldView.use();
		OController.stepDraw();
		for (size_t i = 0; i < 100; i++) Drawer.drawText(i*50, {(double)(i*50), 500});

		currentTime = clock.restart().asSeconds();
		fps = (double)1.0 / currentTime;


		fps_stream << fps;
		text.setString("FPS: " + fps_stream.str());

		window.draw(text);
		fps_stream.str("");
		fps_stream.clear();

		label.drawSelf();
		label.drawTo(window);

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
		main_window->label.setText("Main window");
		main_window->moveRelative({300, 100});
		main_window->addParent(OController.action_win_set);



		Button * single_player = new Button(Codir<2>({0, 0}, {150, 20}));
		single_player->unique_name = "single_player";
		single_player->label.setText("Single Player Game");
		//single_player->moveRelative(main_window->codir.left_up);
		single_player->moveRelative({25, 40});
		single_player->lambda = [&state] () mutable {
			if (state.active) state.pause = mainLoop(MainLoopType::resume);
			else state.pause = mainLoop(MainLoopType::init);
			state.active = true;
			XY current_center = GuiView.current_view.center();
			for (size_t i = 0; i < OController.action_win_set.length; i++)
				OController.action_win_set[i]->moveRelative(current_center - state.current_view);
			state.current_view = current_center;
		};
		main_window->addChild(single_player);


		Button * setting = new Button(Codir<2>({0, 0}, {150, 20}));
		setting->unique_name = "settings";
		setting->label.setText("Setting");
		//setting->moveRelative(main_window->codir.left_up);
		setting->moveRelative({25, 80});
		setting->lambda = []() {
			std::chrono::seconds sec(1);
			std::this_thread::sleep_for(sec);
		};
		main_window->addChild(setting);



		Window * exit_window = new Window(Codir<2>({0, 0}, {150, 120}));
		exit_window->unique_name = "exit_window";
		exit_window->label.setText("Exit?");
		exit_window->moveRelative({300, 100});

		Button * exit_button_yes = new Button(Codir<2>({0, 0}, {100, 20}));
		exit_button_yes->unique_name = "exit button yes";
		exit_button_yes->label.setText("Yes");
		//exit_button_yes->moveRelative(exit_window->codir.left_up);
		exit_button_yes->moveRelative({20, 40});
		exit_button_yes->lambda = []() {
			exit(1);
		};
		exit_window->addChild(exit_button_yes);

		Button * exit_button_no = new Button(Codir<2>({0, 0}, {100, 20}));
		exit_button_no->unique_name = "exit button no";
		exit_button_no->label.setText("Back");
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
		exit->label.setText("Exit");
		//exit->moveRelative(main_window->codir.left_up);
		exit->moveRelative({25, 120});
		//вылетает потому что main_window и exit_window уже не существует
		exit->lambda = [main_window, exit_window]() {
			Ras<BaseGui> * ras = main_window->ras_record->storage;
			main_window->removeParent();
			exit_window->addParent(*ras);
			exit_window->moveRelative(main_window->focus_offset + XY({20, 0}) - exit_window->focus_offset);
		};
		main_window->addChild(exit);

		state.main_window = main_window;
	}

	sf::RenderWindow & window = GameWindow::window;

	Button * button = (Button *) state.main_window->getByName("single_player").valueOr(nullptr);

	while (window.isOpen()) {
		//EventKeeper processor
		Event.flush();
		Event.load(window);
		if (Event.KeyPressed[sf::Keyboard::Escape] && state.active) {
			auto element = state.main_window->getByName("single_player");
			if (element.isOk) element.value->actionRealized();
		}

		//Action processor
		if (!pause) OController.stepAction();
		OController.stepActionWin();

		WorldView.update();

		//Clear screen
		window.clear(sf::Color::Black);

		//Draw Processor
		WorldView.use();
		OController.stepDraw();

		GuiView.use();
		OController.stepDrawWin();

		//Display part
		window.display();
	}
	return 0;
}

int main(int argc, char * argv[]) {

	FlagParser parser(argc, argv);
	//FlagParserAutoHelpSource = &parser;

	parser.add(Flag(
		{"-help", "--help", "-h", "--h", "?", "-?", "help"},
		"show all flags and it's descriptions",
		[](FlagParser * parser) {
			parser->printAutoGeneratedHelp();
			exit(0);
		}
	));
	parser.add(Flag(
		{"-width"},
		"set game window width. Expect one int parameter: width",
		[](const char * const * argv, size_t left) -> size_t {
			if (left < 1) {
				printf("flag [%s] expected one parameter", argv[0]);
				exit(0);
			}
			GameWindow::width  = std::stoi(argv[1]);
			return 1;
		}
	));
	parser.add(Flag(
		{"-height"},
		"set game window height. Expect one int parameter: height",
		[](const char * const * argv, size_t left) -> size_t {
			if (left < 1) {
				printf("flag [%s] expected one parameter", argv[0]);
				exit(0);
			}
			GameWindow::height = std::stoi(argv[1]);
			return 1;
		}
	));
	parser.add(Flag(
		{"-fps"},
		"set fps for game window. Expect one int parameter: fps count",
		[](const char * const * argv, size_t left) -> size_t {
			if (left < 1) {
				printf("flag [%s] expected one parameter", argv[0]);
				exit(0);
			}
			GameWindow::limit_fps = std::stoi(argv[1]);
			return 1;
		}
	));

	parser.execute();

	GameWindow::create(sf::Style::Close);

	Drawer.target = &GameWindow::window;
	Mouse.window  = &GameWindow::window;

	GameWindow::resetView(WorldView);
	GameWindow::resetView(GuiView);

	//Textures::load("source/void.png");

	Basic::texture_default = Textures::load("assets/objects/Default/textures/void.png").ok();	

	Player::texture_folded = Textures::load("assets/objects/Robot/textures/robot-closed.png").ok();
	Player::texture_unfolded = Textures::load("assets/objects/Robot/textures/robot-open.png").ok();

	Fonts.load("assets/fonts/verdana.ttf", "verdana");
	Fonts.load("assets/fonts/ubuntumono/UbuntuMono-R.ttf" , "UbuntuMono-R" );
	Fonts.load("assets/fonts/ubuntumono/UbuntuMono-RI.ttf", "UbuntuMono-RI");
	Fonts.load("assets/fonts/ubuntumono/UbuntuMono-B.ttf" , "UbuntuMono-B" );
	Fonts.load("assets/fonts/ubuntumono/UbuntuMono-BI.ttf", "UbuntuMono-BI");

	Fonts.setFontToText("UbuntuMono-R", Drawer.text);

	ubermenu(UbermenuType::main, false, true);

	return 0;
}
