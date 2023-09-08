#pragma once

#include <map>
#include <string>
#include <format>
#include <tuple>

#include <nlohmann/json.hpp>
#include <tl/generator.hpp>

#include <Language.hpp>
#include <Type.hpp>
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
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OneLanguagePokemon, id, name, descriptions, height, weight, types, image_url)

struct Pokemon
{
	// It consumes the Pokemon by moving out data
	tl::generator<std::pair<Language, OneLanguagePokemon>> generatePkemonByLanguage()
	{
		if (names.size() != descriptions.size() && descriptions.size() != types.size())
		{
			auto error_msg = std::format("Incoherent number of language names = {}, descriptions = {}, types = {}", names.size(), descriptions.size(), types.size());
			throw std::runtime_error(std::move(error_msg));
		}

		auto flatten =
			[](auto&& pairs)
			{
				Language name_lang = std::move(std::get<0>(pairs).first);
				Language description_lang = std::move(std::get<1>(pairs).first);
				Language type_lang = std::move(std::get<2>(pairs).first);

				// They are all in an ordered map, so the order should be the same
				if (name_lang != description_lang || description_lang != type_lang)
				{
					auto error_msg = std::format("Incoherent langage names = {}, descriptions = {}, types = {}", name_lang.value_of(), description_lang.value_of(), type_lang.value_of());
					throw std::runtime_error(std::move(error_msg));
				}

				return
					std::make_tuple(
						std::move(name_lang),
						std::move(std::get<0>(pairs).second),
						std::move(std::get<1>(pairs).second),
						std::move(std::get<2>(pairs).second)
					);
			};

		for (auto&& [l, n, d, t]: std::views::zip(std::move(names), std::move(descriptions), std::move(types)) | std::views::transform(flatten))
		{
			auto splitted_pokemon = std::make_pair(l, OneLanguagePokemon{
				.id = id,
				.name = std::move(n),
				.descriptions = std::move(d),
				.height = height,
				.weight = weight,
				.types = std::move(t),
				.image_url = image_url
			});
			co_yield splitted_pokemon;
		}

	}

	std::size_t id;
	std::map<Language, std::string> names;
	std::map<Language, std::vector<std::string>> descriptions;
	Meter height;
	Kilogram weight;
	std::map<Language, std::vector<Type>> types;
	std::string image_url;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Pokemon, id, names, descriptions, height, weight, types, image_url)

} // namespace pokemon


