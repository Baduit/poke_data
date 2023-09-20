#include <Pokemon.hpp>

namespace pokemon
{

nlohmann::json OneLanguagePokemon::serialize_for_pokeguesser() const
{
	nlohmann::json j;
	j["name"] = name;
	j["descriptions"] = descriptions;
	j["height"] = height;
	j["weight"] = weight;
	j["types"] = types;
	j["image_url"] = image_url;
	return j;
}

nlohmann::json OneLanguagePokemon::serialize_for_pokedle() const
{
	nlohmann::json j;
	j["name"] = name;
	j["height"] = height;
	j["weight"] = weight;
	j["types"] = types;
	j["generation"] = generation;
	j["color"] = color;
	return j;
}

tl::generator<std::pair<Language, OneLanguagePokemon>> Pokemon::generatePkemonByLanguage()
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
			Language color_lang = std::move(std::get<3>(pairs).first);

			// They are all in an ordered map, so the order should be the same
			if (name_lang != description_lang || description_lang != type_lang || type_lang != color_lang)
			{
				auto error_msg = std::format("Incoherent langage names = {}, descriptions = {}, types = {}, color_lang = {}", name_lang.value_of(), description_lang.value_of(), type_lang.value_of(), color_lang.value_of());
				throw std::runtime_error(std::move(error_msg));
			}

			return
				std::make_tuple(
					std::move(name_lang),
					std::move(std::get<0>(pairs).second),
					std::move(std::get<1>(pairs).second),
					std::move(std::get<2>(pairs).second),
					std::move(std::get<3>(pairs).second)
				);
		};

	for (auto&& [l, n, d, t, c]: std::views::zip(std::move(names), std::move(descriptions), std::move(types), std::move(color)) | std::views::transform(flatten))
	{
		auto splitted_pokemon = std::make_pair(l, OneLanguagePokemon{
			.id = id,
			.name = std::move(n),
			.descriptions = std::move(d),
			.height = height,
			.weight = weight,
			.types = std::move(t),
			.image_url = image_url,
			.generation  = generation,
			.color = std::move(c)
		});
		co_yield splitted_pokemon;
	}

}

} // namespace pokemon
