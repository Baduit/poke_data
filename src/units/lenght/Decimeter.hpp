#pragma once

#include <units.h>
#include <nlohmann/json.hpp>

namespace pokemon
{

using Decimeter = units::length::decimeter_t;

} // namespace pokemon


// ADL serialize
namespace nlohmann
{

template <>
struct adl_serializer<pokemon::Decimeter>
{
	static void to_json(json& j, const pokemon::Decimeter& lenght)
	{
		j = lenght.to<double>();
	}

	static void from_json(const json& j, pokemon::Decimeter& lenght)
	{
		lenght = pokemon::Decimeter(j.get<double>());
	}
};

}