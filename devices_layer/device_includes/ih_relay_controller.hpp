#pragma once
#include <memory>

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace Reflow::Devices::Relay
{
class RelayController : public boost::intrusive_ref_counter<RelayController>
{

public:
    RelayController();
    ~RelayController();

    using Ptr = boost::intrusive_ptr<RelayController>;

public:
    void enable();
    void disable();

private:
    class RelayControllerImpl;
    std::unique_ptr<RelayControllerImpl> m_pImpl;
};
} // namespace Reflow::Devices::Relay