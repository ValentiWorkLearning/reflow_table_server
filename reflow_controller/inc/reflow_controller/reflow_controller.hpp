#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <commands/commands_list.hpp>

namespace Reflow::Controller
{
class ReflowProcessController : public boost::intrusive_ref_counter<ReflowProcessController>
{

public:
    ReflowProcessController();
    ~ReflowProcessController();

public:
    void postCommand(Reflow::Commands::TCommandVariant contextVariant);

public:
    using Ptr = boost::intrusive_ptr<ReflowProcessController>;

private:
    class ReflowProcessControllerImpl;
    std::unique_ptr<ReflowProcessControllerImpl> m_pImpl;
};
} // namespace Reflow::Controller