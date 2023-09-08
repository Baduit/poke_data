#pragma once

#include <units.h>
#include <nlohmann/json.hpp>

namespace pokemon
{

using Meter = units::length::meter_t;

} // namespace pokemon


// ADL serialize
namespace nlohmann
{

template <>
struct adl_serializer<pokemon::Meter>
{
	static void to_json(json& j, const pokemon::Meter& lenght)
	{
		j = lenght.to<double>();
	}

	static void from_json(const json& j, pokemon::Meter& lenght)
	{
		lenght = pokemon::Meter(j.get<double>());
	}
};

}