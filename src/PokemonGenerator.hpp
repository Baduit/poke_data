#pragma once

#include <filesystem>

#include <tl/generator.hpp>

#include <Pokemon.hpp>
#include <units/lenght/Decimeter.hpp>
#include <units/mass/Hectogram.hpp>

namespace pokemon
{

class PokemonGenerator
{
	public:
		static tl::generator<Pokemon> generatePokemon(std::filesystem::path data_location);

	private:
		struct PokemonInfo
		{
			Decimeter height;
			Hectogram weight;
			std::string image_url;
			std::filesystem::path specie;
			tl::generator<std::filesystem::path> types;
		};
		static PokemonInfo getPokemonInfo(const std::filesystem::path& pokemon_filename);


		struct PokemonSpecieInfo
		{
			std::map<Language, std::string> names;
			std::map<Language, std::vector<std::string>> descriptions;
		};
		static PokemonSpecieInfo getPokemonSpeciesInfo(const std::filesystem::path& pokemon_specie_filename);


		struct TypeInfo
		{
			Language language;
			Type type;
		};
		static tl::generator<TypeInfo> getTypeInfo(std::filesystem::path type_filename);

};

} // namespace pokemon
