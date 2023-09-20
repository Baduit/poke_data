#pragma once

#include <string>

#include <strong_type/strong_type.hpp>
#include <strong_type/regular.hpp>
#include <strong_type/ordered.hpp>

#include <StrongTypeJsonModified.hpp>

namespace pokemon
{

using Color = strong::type<std::string, struct Color_, strong::regular, SerializableToJson>;

} // namespace pokemon
