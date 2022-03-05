#include <fmt/format.h>
#include <presets/presets_holder.hpp>

namespace Reflow::Presets
{
Preset::Preset(std::string_view presetName) : m_presetName{presetName}
{
}
const std::string& Preset::presetName() const
{
    return m_presetName;
}

void Preset::setName(std::string_view presetName)
{
    m_presetName = presetName;
}

void Preset::addStageItem(StageItem&& stageItem)
{
    m_presetItems.push_back(std::move(stageItem));
}

void Preset::replaceStageItem(std::size_t stageItemIdx, StageItem&& stageItem)
{
    const bool hasToPush = m_presetItems.empty() ? true : stageItemIdx > m_presetItems.size() - 1;
    if (hasToPush)
    {
        m_presetItems.push_back(std::move(stageItem));
    }
    else
    {
        m_presetItems.at(stageItemIdx) = std::move(stageItem);
    }
}

void Preset::forEachStage(TTraverseFunction traverser)
{
    for (const auto& stageItem : m_presetItems)
        traverser(stageItem);
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
void PresetsHolder::forEachPreset(TEnumerateCallback callback)
{
    for (const auto& [presetHash, presetPtr] : m_presetsStorage)
    {
        callback(presetHash, presetPtr);
    }
}
std::size_t Preset::numStages() const noexcept
{
    return m_presetItems.size();
}

const Preset::StageItem& Preset::getStageItem(std::size_t stageItemIdx)
{
    return m_presetItems.at(stageItemIdx);
}

} // namespace Reflow::Presets
