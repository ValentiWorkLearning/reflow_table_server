#include "presets/presets_holder.hpp"

namespace Reflow::Presets
{

bool PresetsHolder::isEmpty() const
{
    return m_presetsStorage.empty();
}
} // namespace Reflow::Presets