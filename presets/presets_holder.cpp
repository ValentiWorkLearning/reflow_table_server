#include "inc\presets\presets_holder.hpp"
#include "presets/presets_holder.hpp"
#include <fmt/format.h>

namespace Reflow::Presets
{
Preset::Preset(std::string_view presetName) : m_presetName{presetName}
{
}
const std::string& Preset::presetName() const
{
    return m_presetName;
}
void Preset::addStageItem(StageItem&& stageItem)
{
    m_presetItems.push_back(stageItem);
}
bool PresetsHolder::isEmpty() const
{
    return m_presetsStorage.empty();
}
std::size_t PresetsHolder::addNewPreset(std::string_view presetName)
{
    if (auto it = m_presetsStorage.find(std::hash<std::string_view>()(presetName));
        it != m_presetsStorage.end())
        throw std::runtime_error(
            fmt::format("Attempt to add an existing preset with the name:{}", presetName));
    auto presetHash = std::hash<std::string_view>()(presetName);
    m_presetsStorage.insert({presetHash, new Preset{presetName}});
    return presetHash;
}
Preset::Ptr PresetsHolder::getPresetById(std::size_t presetId)
{
    if (auto it = m_presetsStorage.find(presetId); it != m_presetsStorage.end())
        return it->second;

    throw std::runtime_error(fmt::format("Attempt to take unexisting preset by id:{}", presetId));
}
const Preset::Ptr PresetsHolder::getPresetById(std::size_t presetId) const
{
    if (auto it = m_presetsStorage.find(presetId); it != m_presetsStorage.end())
        return it->second;

    throw std::runtime_error(fmt::format("Attempt to take unexisting preset by id:{}", presetId));
}

std::size_t PresetsHolder::presetsCount() const
{
    return m_presetsStorage.size();
}
} // namespace Reflow::Presets