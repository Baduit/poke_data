#pragma once

#include <nlohmann/json.hpp>

namespace pokemon
{

struct MetaData
{
	std::vector<std::string> names;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetaData, names)

} // namespace pokemon
