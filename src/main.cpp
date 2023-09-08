#include <iostream>

#include <PokemonGenerator.hpp>
#include <MetaData.hpp>

int main(int, char**)
{
	std::cout << "Hello, World!" << std::endl;
	try
	{
		for (auto&& complete_pokemon: pokemon::PokemonGenerator::generatePokemon("C:/Users/Athlena/Documents/GitHub/api-data/data"))
		{
			for (auto&& [language, pokemon]: complete_pokemon.generatePkemonByLanguage())
			{
				std::cout << nlohmann::json(pokemon).dump(4) << std::endl;
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}