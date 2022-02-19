#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace Reflow::Controller
{
class ReflowProcessController : public boost::intrusive_ref_counter<ReflowProcessController>
{
public:
    using Ptr = boost::intrusive_ptr<ReflowProcessController>;
};
} // namespace Reflow::Commands