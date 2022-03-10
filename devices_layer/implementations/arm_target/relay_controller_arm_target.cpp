#include <device_includes/ih_relay_controller.hpp>

namespace Reflow::Devices::Relay
{
class RelayController::RelayControllerImpl
{
public:
    void enable()
    {
    }
    void disable()
    {
    }
};

void RelayController::enable()
{
    m_pImpl->enable();
}

void RelayController::disable()
{
    m_pImpl->disable();
}

RelayController::RelayController() : m_pImpl{std::make_unique<RelayControllerImpl>()}
{
}

RelayController::~RelayController() = default;
} // namespace Reflow::Devices::Relay