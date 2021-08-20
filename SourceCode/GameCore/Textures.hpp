#ifndef FENGINE_SC_Textures
#define FENGINE_SC_Textures

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/String.hpp>
#include <UseFull/Templates/Ras.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct Textures {
public:

	template <typename T>
	struct TypeDef {
		T value;
		constexpr TypeDef(T value): value{value} { }
		auto operator<=>(const TypeDef<T> &) const = default;
		explicit operator T();
	};

	struct TextureId : public TypeDef<unsigned int> { 
		explicit TextureId(unsigned int value) : TypeDef<unsigned int>(value) {}
	};

	struct Record {
		sf::Texture texture;
		sf::Image image;
		sf::String path;

		Record(const char * str) {
			path = sf::String(str);
		}

		bool load() {
			bool success = image.loadFromFile(path);
			if (!success) {
				std::cout << "Error on load image [" << path.toAnsiString() << "]\n";
				return false;
			}

			success = texture.loadFromImage(image);
			if (!success) {
				std::cout << "Error on load image as texture for [" << path.toAnsiString() << "]\n";
				return false;
			}

			std::cout << "Loaded successfully [" << path.toAnsiString() << "]\n";

			return success;

		}

	};

	inline static std::vector<Record *> set;

	static uft::Ok<Record *> load(const char * str) {

		Record * record = new Record(str);
		bool success = record->load();
		if (!success) {
			delete record; 
			return {};
		}
		else {
			set.push_back(record);
			return record;
		}
	}
};

#endif