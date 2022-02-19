#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace Reflow::Commands
{
class CommandsParser : public boost::intrusive_ref_counter<CommandsParser>
{
public:
    using Ptr = boost::intrusive_ptr<CommandsParser>;
};
}