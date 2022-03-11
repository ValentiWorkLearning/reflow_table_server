#include <device_includes/ih_relay_controller.hpp>

#include <gpiod.hpp>

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <cstdint>

namespace Reflow::Devices::Relay
{
class RelayController::RelayControllerImpl
{
public:
    RelayControllerImpl()
        :
        m_pGpioChip{std::make_unique<gpiod::chip>(kGpioChipName)}
    {
        spdlog::info("gpiochip with title:{} exposes {} lines", kGpioChipName,m_pGpioChip->num_lines());
        m_concreteLine = std::move(m_pGpioChip->get_line(kGpioLine));
        m_concreteLine.request(gpiod::line_request{.consumer = kConsumerName, .request_type=gpiod::line_request::DIRECTION_OUTPUT,.flags=0});
    }
    void enable()
    {
        m_concreteLine.set_value(1);
    }
    void disable()
    {
        m_concreteLine.set_value(0);
    }
private:

    static const inline std::string kGpioChipName = "gpiochip0";
    static const inline std::string kConsumerName = "reflow_server";

    static constexpr inline std::uint16_t kGpioLine = 10;
private:
    std::unique_ptr<gpiod::chip> m_pGpioChip;
    gpiod::line m_concreteLine;
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
