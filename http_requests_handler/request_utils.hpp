#pragma once

#include <variant>
#include <presets/presets_holder.hpp>
#include <string>
#include <tl/expected.hpp>
#include <reflow_controller/reflow_controller.hpp>

namespace RequestUtils
{
using TStagesContainer = std::vector<Reflow::Presets::Preset::StageItem>;
using TPresetRequestVariant = std::variant<std::string,TStagesContainer , std::monostate>;

TPresetRequestVariant parsePresetUpdateRequest(std::string_view requestBody);

std::optional<std::string> parsePresetName(std::string_view requestBody);

tl::expected<Reflow::Controller::RegulatorParams,std::string_view> parseRegulatorParams(
    std::string_view requestBody);

}