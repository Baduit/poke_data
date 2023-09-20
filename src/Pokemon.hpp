#pragma once

#include <map>
#include <string>
#include <format>
#include <tuple>

#include <nlohmann/json.hpp>
#include <tl/generator.hpp>

#include <Language.hpp>
#include <Type.hpp>
#include <Color.hpp>
#include <units/lenght/Meter.hpp>
#include <units/mass/Kilogram.hpp>

namespace pokemon
{

struct OneLanguagePokemon
{
	std::size_t id;
	std::string name;
	std::vector<std::string> descriptions;
	Meter height;
	Kilogram weight;
	std::vector<Type> types;
	std::string image_url;
	std::size_t generation;
	Color color;

	nlohmann::json serialize_for_pokeguesser() const;
	nlohmann::json serialize_for_pokedle() const;

};

struct Pokemon
{
	// It consumes the Pokemon by moving out data
	tl::generator<std::pair<Language, OneLanguagePokemon>> generatePkemonByLanguage();

	std::size_t id;
	std::map<Language, std::string> names;
	std::map<Language, std::vector<std::string>> descriptions;
	Meter height;
	Kilogram weight;
	std::map<Language, std::vector<Type>> types;
	std::string image_url;
	std::size_t generation;
	std::map<Language, Color> color;
};

} // namespace pokemon


