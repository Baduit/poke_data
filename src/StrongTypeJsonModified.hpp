#pragma once

#include <nlohmann/json.hpp>

struct SerializableToJson
{

	template <typename T>
	struct modifier
	{
		friend void to_json(nlohmann::json& j, const T& t) {
			j = t.value_of();
		}

		friend void from_json(const nlohmann::json& j, T& t) {
			j.get_to(t.value_of());
		}
	};

};