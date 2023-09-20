#include <ranges>
#include <expected>
#include <fstream>

#include <PokemonGenerator.hpp>


namespace
{

enum class ToIntError
{
	InvalidArgument, // std::errc::invalid_argument
	ResultOutOfRange // std::errc::result_out_of_range 
};

std::expected<std::size_t, ToIntError> to_int(std::string_view s) 
{
	std::size_t value;
	auto result = std::from_chars(s.data(), s.data() + s.size(), value);
	if (result.ec == std::errc{})
		return value;
	else if (result.ec == std::errc::invalid_argument)
		return std::unexpected(ToIntError::InvalidArgument);
	else if (result.ec == std::errc::result_out_of_range)
		return std::unexpected(ToIntError::ResultOutOfRange);
	else
		std::unreachable();
};

bool is_dir(const std::filesystem::directory_entry& entry)
{
	return entry.is_directory();
}

auto get_file_info(const std::filesystem::directory_entry& entry)
{
	return
		std::make_pair(
			to_int(entry.path().filename().string()),
			entry.path() / "index.json"
		);
};


auto is_valid_pokemon_id = [](const auto& pair_arg) -> bool
{
	const auto& [id, _] = pair_arg;
	auto is_not_pokemon_special_form = [](std::size_t i){ return i < 10000; };
	return id.transform(is_not_pokemon_special_form).value_or(false);
};

auto unwrap_id = [](const auto& pair_arg)
{
	const auto& [id, path] = pair_arg;
	return std::make_pair(*id, path);
};

} // namespace

namespace pokemon
{

// Implementation
tl::generator<Pokemon> PokemonGenerator::generatePokemon(std::filesystem::path data_location)
{
	std::filesystem::path pokemon_directory = data_location;
	pokemon_directory += "/api/v2/pokemon";

	for (const auto& [id, pokemon_filename] : std::filesystem::directory_iterator{ pokemon_directory }
		| std::views::filter(is_dir)
		| std::views::transform(get_file_info)
		| std::views::filter(is_valid_pokemon_id)
		| std::views::transform(unwrap_id))
	{
		Pokemon pokemon{};
		pokemon.id = id;

		auto pokemon_info = getPokemonInfo(pokemon_filename);
		pokemon.height = pokemon_info.height;
		pokemon.weight = pokemon_info.weight;
		pokemon.image_url = std::move(pokemon_info.image_url);

		auto spec_location = data_location / pokemon_info.specie;
		auto pokemon_specie_info = getPokemonSpeciesInfo(data_location / pokemon_info.specie / "index.json");
		pokemon.names = std::move(pokemon_specie_info.names);
		pokemon.descriptions = std::move(pokemon_specie_info.descriptions);
		pokemon.generation = pokemon_specie_info.generation;

		for (const auto& type_url : pokemon_info.types)
		{
			for (auto&& [language, type]: getTypeInfo(data_location / type_url / "index.json"))
			{
				pokemon.types[std::move(language)].push_back(std::move(type));
			}
		}

		pokemon.color = getColorInfo(data_location / pokemon_specie_info.color / "index.json");
		
		// It could be optimized probably, I'm  bit tired and that's the first idea I got
		std::erase_if(pokemon.names, [&pokemon](const auto& pair_lang_name){ return !pokemon.descriptions.contains(pair_lang_name.first); });
		std::erase_if(pokemon.names, [&pokemon](const auto& pair_lang_name){ return !pokemon.types.contains(pair_lang_name.first); });

		std::erase_if(pokemon.descriptions, [&pokemon](const auto& pair_lang_name){ return !pokemon.names.contains(pair_lang_name.first); });
		std::erase_if(pokemon.descriptions, [&pokemon](const auto& pair_lang_name){ return !pokemon.types.contains(pair_lang_name.first); });

		std::erase_if(pokemon.types, [&pokemon](const auto& pair_lang_name){ return !pokemon.names.contains(pair_lang_name.first); });
		std::erase_if(pokemon.types, [&pokemon](const auto& pair_lang_name){ return !pokemon.descriptions.contains(pair_lang_name.first); });

		std::erase_if(pokemon.color, [&pokemon](const auto& pair_lang_name){ return !pokemon.names.contains(pair_lang_name.first); });

		co_yield pokemon;
	}

}

PokemonGenerator::PokemonInfo PokemonGenerator::getPokemonInfo(const std::filesystem::path& pokemon_filename)
{
	std::ifstream stream(pokemon_filename);
	nlohmann::json data = nlohmann::json::parse(stream);

	auto height = data.at("height").get<Decimeter>();
	auto weight = data.at("weight").get<Hectogram>();
	auto image_url = data.at("sprites").at("other").at("official-artwork").at("front_default").get<std::string>();
	auto specie = data.at("species").at("url").get<std::string_view>().substr(1); // Remove first /

	auto types_generator =
		[](auto data_types) -> tl::generator<std::filesystem::path>
		{
			auto type_obj_to_url = [](auto&& type_obj){ return type_obj.at("type").at("url"); };
			for (auto&& t: data_types | std::views::transform(type_obj_to_url))
			{
				std::filesystem::path a_type = t.get<std::string_view>().substr(1);
				co_yield a_type;
			}
		};
	
	return
		PokemonInfo{
			.height = height,
			.weight = weight,
			.image_url = std::move(image_url),
			.specie = std::move(specie),
			.types = types_generator(std::move(data.at("types")))
		};
}



PokemonGenerator::PokemonSpecieInfo PokemonGenerator::getPokemonSpeciesInfo(const std::filesystem::path& pokemon_specie_filename)
{
	std::ifstream stream(pokemon_specie_filename);
	nlohmann::json data = nlohmann::json::parse(stream);
	
	std::map<Language, std::string> names;
	for (const auto& name_object: data.at("names"))
	{
		auto language  = name_object.at("language").at("name").get<Language>();
		auto name = name_object.at("name").get<std::string>();
		names.insert(std::make_pair(std::move(language), std::move(name)));
	}

	std::map<Language, std::vector<std::string>> descriptions;
	for (const auto& description_object: data.at("flavor_text_entries"))
	{
		auto language  = description_object.at("language").at("name").get<Language>();
		auto description = description_object.at("flavor_text").get<std::string>();
		descriptions[std::move(language)].push_back(std::move(description));
	}

	std::string_view generation = data.at("generation").at("url").get<std::string_view>();
	generation = generation.substr(0, generation.length() -1); // Remove last /
	generation = generation.substr(generation.find_last_of('/') + 1);

	std::filesystem::path color_url = data.at("color").at("url").get<std::string_view>().substr(1);
	
	return
		PokemonSpecieInfo{
			.names = std::move(names),
			.descriptions = std::move(descriptions),
			.generation = to_int(generation).value(),
			.color = color_url
		};
}



tl::generator<PokemonGenerator::TypeInfo> PokemonGenerator::getTypeInfo(std::filesystem::path type_filename)
{
	std::ifstream stream(type_filename);
	nlohmann::json data = nlohmann::json::parse(stream);

	for (const auto& type_obj: data.at("names"))
	{
		auto language = type_obj.at("language").at("name").get<Language>();
		auto type = type_obj.at("name").get<Type>();
		auto type_info = TypeInfo{
				.language = language,
				.type = type
			};
		co_yield type_info;
	}
}

std::map<Language, Color> PokemonGenerator::getColorInfo(std::filesystem::path color_filename)
{
	std::ifstream stream(color_filename);
	nlohmann::json data = nlohmann::json::parse(stream);

	std::map<Language, Color> color_per_language;

	for (const auto& type_obj: data.at("names"))
	{
		auto language = type_obj.at("language").at("name").get<Language>();
		auto color = type_obj.at("name").get<Color>();
		color_per_language[language] = color;
	}

	return color_per_language;
}
	
} // namespace pokemon
