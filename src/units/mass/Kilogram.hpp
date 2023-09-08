#pragma once

#include <units.h>
#include <nlohmann/json.hpp>

namespace pokemon
{

using Kilogram = units::mass::kilogram_t;

} // namespace pokemon


// ADL serialize
namespace nlohmann
{

template <>
struct adl_serializer<pokemon::Kilogram>
{
	static void to_json(json& j, const pokemon::Kilogram& mass)
	{
		j = mass.to<double>();
	}

	static void from_json(const json& j, pokemon::Kilogram& mass)
	{
		mass = pokemon::Kilogram(j.get<double>());
	}
};


}