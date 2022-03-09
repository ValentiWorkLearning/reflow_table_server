#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <memory>
#include <functional>
#include <commands/commands_list.hpp>

namespace Reflow::Commands
{
class CommandsParser : public boost::intrusive_ref_counter<CommandsParser>
{
public:
    using Ptr = boost::intrusive_ptr<CommandsParser>;
public:
    
    CommandsParser();
    ~CommandsParser();

public:
    TCommandContext parseCommandContext(std::string_view commandContext);
private:
    class CommandsParserImpl;
    std::unique_ptr<CommandsParserImpl> m_pImpl;
};
}