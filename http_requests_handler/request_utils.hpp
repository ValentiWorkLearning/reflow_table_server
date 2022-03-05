#pragma once

#include <variant>
#include <presets/presets_holder.hpp>
#include <string>

template <typename... Ts>
struct Overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;


namespace RequestUtils
{
using TStagesContainer = std::vector<Reflow::Presets::Preset::StageItem>;
using TPresetRequestVariant = std::variant<std::string,TStagesContainer , std::monostate>;

TPresetRequestVariant parsePresetUpdateRequest(std::string_view requestBody);

}