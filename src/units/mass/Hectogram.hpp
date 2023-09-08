#pragma once

#include <units.h>
#include <nlohmann/json.hpp>

namespace pokemon
{

using Hectogram = units::mass::hectogram_t;

} // namespace pokemon


// ADL serialize
namespace nlohmann
{

template <>
struct adl_serializer<pokemon::Hectogram>
{
	static void to_json(json& j, const pokemon::Hectogram& mass)
	{
		j = mass.to<double>();
	}

	static void from_json(const json& j, pokemon::Hectogram& mass)
	{
		mass = pokemon::Hectogram(j.get<double>());
	}
};

}