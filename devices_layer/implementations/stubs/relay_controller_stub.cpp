#include <device_includes/ih_relay_controller.hpp>
#include <spdlog/spdlog.h>

namespace Reflow::Devices::Relay
{
class RelayController::RelayControllerImpl
{
public:
    void enable()
    {
        spdlog::info("RelayController::enable()");
    }
    void disable()
    {
        spdlog::info("RelayController::disable()");
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