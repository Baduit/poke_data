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

struct PokemonData
{
	std::vector<std::pair<std::size_t, std::string>> names_with_id;
	std::vector<pokemon::OneLanguagePokemon> pokemons;
};		


void write_to_file(const std::filesystem::path& output_filename, const nlohmann::json& value);
std::pair<std::filesystem::path, std::filesystem::path> handle_args(int argc, char** argv);
std::map<pokemon::Language, PokemonData> extract_pokemon_data(const std::filesystem::path& input_dir);
std::filesystem::path generate_data_directory(const std::filesystem::path& output_dir);
std::filesystem::path generate_language_directory(const pokemon::Language& language, const std::filesystem::path& generated_dir);
pokemon::MetaData generate_metadata(std::vector<std::pair<std::size_t, std::string>> names_with_id); // Copy intended
std::filesystem::path generate_pokeguesser_directory(const std::filesystem::path& language_dir);
void write_pokeguesser_data(const std::filesystem::path& pokeguesser_directory, const std::vector<pokemon::OneLanguagePokemon>& pokemons);
std::filesystem::path generate_pokedle_directory(const std::filesystem::path& language_dir);
void write_pokedle_data(const std::filesystem::path& pokedle_directory, const std::vector<pokemon::OneLanguagePokemon>& pokemons);


int main(int argc, char** argv)
{
	try
	{
		auto [input_dir, output_dir] = handle_args(argc, argv);
		auto pokemon_data = extract_pokemon_data(input_dir);
		std::filesystem::path generated_dir = generate_data_directory(output_dir);
	
		for (const auto& [language, data]: pokemon_data)
		{
			const auto& [names_with_id, pokemons] = data;
			auto metadata = generate_metadata(names_with_id);
			std::filesystem::path language_dir = generate_language_directory(language, generated_dir);
			write_to_file(language_dir / "metadata.json", metadata);
			std::filesystem::path pokeguesser_directory = generate_pokeguesser_directory(language_dir);
			write_pokeguesser_data(pokeguesser_directory, pokemons);
			std::filesystem::path pokedle_directory = generate_pokedle_directory(language_dir);
			write_pokedle_data(pokedle_directory, pokemons);
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void write_to_file(const std::filesystem::path& output_filename, const nlohmann::json& value)
{
	std::ofstream output_stream(output_filename);
	output_stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	output_stream << value.dump(4);
}

std::pair<std::filesystem::path, std::filesystem::path> handle_args(int argc, char** argv)
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

std::map<pokemon::Language, PokemonData> extract_pokemon_data(const std::filesystem::path& input_dir)
{
	std::map<pokemon::Language, PokemonData> pokemon_data;
	for (auto&& complete_pokemon: pokemon::PokemonGenerator::generatePokemon(input_dir))
	{
		for (auto&& [language, pokemon]: complete_pokemon.generatePkemonByLanguage())
		{
			pokemon_data[language].names_with_id.emplace_back(pokemon.id, pokemon.name);
			pokemon_data[language].pokemons.push_back(std::move(pokemon));
		}
	}
	return pokemon_data;
}

std::filesystem::path generate_data_directory(const std::filesystem::path& output_dir)
{
	std::filesystem::path generated_dir = output_dir / "generated_data";
	if (!std::filesystem::create_directory(generated_dir))
		throw std::runtime_error(std::format("Can't create data directory inside output directory : {}", generated_dir.string()));
	return generated_dir;
}

std::filesystem::path generate_language_directory(const pokemon::Language& language, const std::filesystem::path& generated_dir)
{
	std::filesystem::path language_dir = generated_dir / language.value_of();
	if (!std::filesystem::create_directory(language_dir))
		throw std::runtime_error(std::format("Can't create the language directory : {}", language_dir.string()));
	return language_dir;
}

pokemon::MetaData generate_metadata(std::vector<std::pair<std::size_t, std::string>> names_with_id)
{
	std::ranges::sort(names_with_id, [](const auto& a, const auto& b) { return a.first < b.first; });
	return pokemon::MetaData{ .names = std::views::transform(names_with_id, [](auto&& n){ return n.second; }) | std::ranges::to<std::vector>()};
}

std::filesystem::path generate_pokeguesser_directory(const std::filesystem::path& language_dir)
{
	std::filesystem::path pokeguesser_directory = language_dir / "pokeguesser";
	if (!std::filesystem::create_directory(pokeguesser_directory))
		throw std::runtime_error(std::format("Can't create the pokeguesser directory : {}", pokeguesser_directory.string()));
	return pokeguesser_directory;
}

void write_pokeguesser_data(const std::filesystem::path& pokeguesser_directory, const std::vector<pokemon::OneLanguagePokemon>& pokemons)
{
	for (const auto& pokemon: pokemons)
	{
		std::filesystem::path pokemon_filename = std::format("{}.json", pokemon.id);
		write_to_file(pokeguesser_directory / pokemon_filename, pokemon.serialize_for_pokeguesser());
	}
}

std::filesystem::path generate_pokedle_directory(const std::filesystem::path& language_dir)
{
	std::filesystem::path pokedle_directory = language_dir / "pokedle";
	if (!std::filesystem::create_directory(pokedle_directory))
		throw std::runtime_error(std::format("Can't create the pokedle directory : {}", pokedle_directory.string()));
	return pokedle_directory;
}

void write_pokedle_data(const std::filesystem::path& pokedle_directory, const std::vector<pokemon::OneLanguagePokemon>& pokemons)
{
	for (const auto& pokemon: pokemons)
	{
		std::filesystem::path pokemon_filename = std::format("{}.json", pokemon.id);
		write_to_file(pokedle_directory / pokemon_filename, pokemon.serialize_for_pokedle());
	}
}
