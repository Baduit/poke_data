#include <iostream>
#include <fstream>
#include <format>

#include <PokemonGenerator.hpp>
#include <MetaData.hpp>

constexpr std::string_view help_message =
		"The first argument is the path of the data directory. "
		"(for example C:/Users/MyUserName/Documents/GitHub/api-data/data)\n"
		"The second argument is the path of the output data directory will be written "
		"(it must exists and be empty or at least without any already generated files)";


void write_to_file(const std::filesystem::path& output_filename, const nlohmann::json& value)
{
	std::ofstream output_stream(output_filename);
	output_stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	output_stream << value.dump(4);
}

auto handle_args(int argc, char** argv)
{
	if (argc != 3)
		throw std::runtime_error(std::format("Wrong number of argument.\nHelp:\n", help_message));

	std::filesystem::path input_dir = argv[1];
	if (!std::filesystem::exists(input_dir))
		throw std::runtime_error(std::format("Invalid input directory.\nHelp:\n", help_message));

	std::filesystem::path output_dir = argv[2];
	if (!std::filesystem::exists(output_dir))
		throw std::runtime_error(std::format("Invalid output directory.\nHelp:\n", help_message));
	
	return std::make_pair(input_dir, output_dir);
}

struct PokemonData
{
	pokemon::MetaData metadata;
	std::vector<pokemon::OneLanguagePokemon> pokemons;
};

int main(int argc, char** argv)
{
	try
	{
		auto [input_dir, output_dir] = handle_args(argc, argv);

		std::map<pokemon::Language, PokemonData> pokemon_data;
		for (auto&& complete_pokemon: pokemon::PokemonGenerator::generatePokemon(input_dir))
		{
			for (auto&& [language, pokemon]: complete_pokemon.generatePkemonByLanguage())
			{
				pokemon_data[language].metadata.names.push_back(pokemon.name);
				pokemon_data[language].pokemons.push_back(std::move(pokemon));
			}
		}

		// Create main output folder
		std::filesystem::path generated_dir = output_dir / "generated_data";
		if (!std::filesystem::create_directory(generated_dir))
			throw std::runtime_error(std::format("Can't create data directory inside output directory : {}", generated_dir.string()));	
	
		// Create a folder per langage
		for (const auto& [language, data]: pokemon_data)
		{
			const auto& [metadata, pokemons] = data;
			std::filesystem::path langage_dir = generated_dir / language.value_of();
			if (!std::filesystem::create_directory(langage_dir))
				throw std::runtime_error(std::format("Can't create the langage directory : {}", langage_dir.string()));
			
			// Serialize the metadata at the top level of the langage directiry
			write_to_file(langage_dir / "metadata.json", metadata);

			// Create the pokeguesser directory
			std::filesystem::path pokeguesser_directory = langage_dir / "pokeguesser";
			if (!std::filesystem::create_directory(pokeguesser_directory))
				throw std::runtime_error(std::format("Can't create the pokeguesser directory : {}", pokeguesser_directory.string()));

			// Serizalize all pokemons with only the data needed for pokeguesser with the name id.json
			for (const auto& pokemon: pokemons)
			{
				std::filesystem::path pokemon_filename = std::format("{}_{}.json", pokemon.id, pokemon.name);
				write_to_file(pokeguesser_directory / pokemon_filename, pokemon);
			}

		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}