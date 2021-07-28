#ifndef FENGINE_SC_TexturesHandler
#define FENGINE_SC_TexturesHandler

#include <UseFull/Templates/Ras.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct Textures {
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

	inline static uft::Ras<sf::Texture> set;
	inline static uft::Ras<sf::String> paths;
	inline static uft::Ras<sf::Image> images;

	static bool load(const char * str) {
		auto rec = paths.add(new sf::String(str));
		sf::String & sfstr = *rec->record;
		
		bool success = false;
		
		sf::Image * image = new sf::Image();
		success = image->loadFromFile(sfstr);
		if (!success) {
			std::cout << "Error on load image [" << sfstr.toAnsiString() << "]\n";
			rec->remove();
			delete image;
			return false;
		}

		sf::Texture * texture = new sf::Texture();
		success = texture->loadFromImage(*image);
		if (!success) {
			std::cout << "Error on load image as texture for [" << sfstr.toAnsiString() << "]\n";
			rec->remove();
			delete image;
			delete texture;
			return false;
		}

		set.add(texture);
		images.add(image);

		std::cout << "Loaded successfully [" << sfstr.toAnsiString() << "]\n";

		return true;
	}
};

#endif