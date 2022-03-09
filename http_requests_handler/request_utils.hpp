#pragma once

#include <variant>
#include <presets/presets_holder.hpp>
#include <string>

namespace RequestUtils
{
using TStagesContainer = std::vector<Reflow::Presets::Preset::StageItem>;
using TPresetRequestVariant = std::variant<std::string,TStagesContainer , std::monostate>;

TPresetRequestVariant parsePresetUpdateRequest(std::string_view requestBody);

std::optional<std::string> parsePresetName(std::string_view requestBody);
}