#include "request_utils.hpp"
#include <nlohmann/json.hpp>

namespace
{
std::optional<RequestUtils::TStagesContainer> parseStagesArray(
    const nlohmann::json::array_t& stagesArray)
{
    RequestUtils::TStagesContainer stagesContainer{};
    for (const auto& item : stagesArray)
    {
        if (!item.is_object())
            return std::nullopt;

        Reflow::Presets::Preset::StageItem stageItem;
        if (auto tempIt = item.find("temperature"); tempIt != item.end())
        {
            if (!tempIt->is_number())
                return std::nullopt;


            stageItem.temperatureStep = tempIt->get<std::uint32_t>();
        }

        if (auto durationIt = item.find("duration"); durationIt != item.end())
        {
            if (!durationIt->is_number())
                return std::nullopt;
            stageItem.stageDuration = std::chrono::seconds(durationIt->get<std::uint32_t>());
        }
        stagesContainer.push_back(stageItem);
    }
    if (stagesContainer.empty())
        return std::nullopt;
    return stagesContainer;
}
} // namespace

namespace RequestUtils
{
TPresetRequestVariant parsePresetUpdateRequest(std::string_view requestBody)
{
    auto ojsonObj = nlohmann::json{requestBody};
    if (ojsonObj.is_discarded())
        return std::monostate{};

    if (!ojsonObj.is_object())
        return std::monostate{};

    auto oObjType = ojsonObj.get_ref<const nlohmann::json::object_t&>();
    if (auto it = oObjType.find("name"); it != oObjType.end())
    {
        return it->second.get<std::string>();
    }
    else if (auto it = oObjType.find("stages"); it != oObjType.end())
    {
        if (!it->second.is_array())
            return std::monostate{};

        auto stagesArray = it->second.get_ref<const nlohmann::json::array_t&>();
        if (auto parseResult = parseStagesArray(stagesArray); parseResult)
            return parseResult.value();
        else
            return std::monostate{};

    }

    return std::monostate{};
}
} // namespace RequestUtils