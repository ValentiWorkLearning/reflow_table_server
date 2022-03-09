#include "reflow_controller/reflow_controller.hpp"

namespace Reflow::Controller
{
class ReflowProcessController::ReflowProcessControllerImpl
{
public:
    void postCommand(Reflow::Commands::TCommandVariant contextVariant)
    {
    }
};

void ReflowProcessController::postCommand(Reflow::Commands::TCommandVariant commandContext)
{
    m_pImpl->postCommand(commandContext);
}
ReflowProcessController::ReflowProcessController()
    : m_pImpl{std::make_unique<ReflowProcessControllerImpl>()}
{
}
ReflowProcessController::~ReflowProcessController() = default;
} // namespace Reflow::Controller