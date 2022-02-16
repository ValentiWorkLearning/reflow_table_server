#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Reflow::Presets
{
struct Preset
{

    struct StageItem
    {
        std::uint32_t tempereatureStep;
        std::chrono::seconds stageDuration;
    };

    using TItemsStorage = std::vector<StageItem>;
    TItemsStorage presetItems;
};
class PresetsHolder
{
public:
    bool isEmpty() const;

private:
    using TPresetsStorage = std::map<std::string, Preset>;

    TPresetsStorage m_presetsStorage;
};
} // namespace Reflow::Presets