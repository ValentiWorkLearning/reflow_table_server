#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace Reflow::Presets
{
class Preset : public boost::intrusive_ref_counter<Preset>
{

public:
    explicit Preset(std::string_view presetName);

public:
    using Ptr = boost::intrusive_ptr<Preset>;

    struct StageItem
    {
        std::uint32_t temperatureStep;
        std::chrono::seconds stageDuration;

        bool operator==(const StageItem& other) const noexcept
        {
            return temperatureStep == other.temperatureStep && stageDuration == other.stageDuration;
        }
    };

public:
    void setName(std::string_view presetName);

    const std::string& presetName() const;
    void addStageItem(StageItem&& stageItem);
    void replaceStageItem(std::size_t stageItemIdx, StageItem&& stageItem);

    using TTraverseFunction = std::function<void(const StageItem&)>;
    void forEachStage(TTraverseFunction traverser);

    std::size_t numStages() const noexcept;

    const StageItem& getStageItem(std::size_t stageItemIdx) const;
    StageItem& getStageItem(std::size_t stageItemIdx);

private:
    using TItemsStorage = std::vector<StageItem>;
    std::string m_presetName;
    TItemsStorage m_presetItems;
};

class PresetsHolder : public boost::intrusive_ref_counter<PresetsHolder>
{
public:
    using Ptr = boost::intrusive_ptr<PresetsHolder>;

public:
    bool isEmpty() const;

    std::size_t addNewPreset(std::string_view presetName);

    std::size_t presetsCount() const;

    Preset::Ptr getPresetById(std::size_t presetId);

    const Preset::Ptr getPresetById(std::size_t presetId) const;

    using TEnumerateCallback = std::function<void(std::size_t, const Preset::Ptr&)>;

    void forEachPreset(TEnumerateCallback callback);

private:
    using TPresetsStorage = std::map<std::size_t, Preset::Ptr>;

    TPresetsStorage m_presetsStorage;
};
} // namespace Reflow::Presets