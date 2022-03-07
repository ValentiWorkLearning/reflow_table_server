#include <boost/signals2/connection.hpp>
#include <commands/commands_list.hpp>
#include <commands/commands_parser.hpp>

namespace Reflow::Commands
{

class CommandsParser::CommandsParserImpl
{
public:
    void parseCommandContext(std::string_view commandContext)
    {
    }

    boost::signals2::connection subscribeOnCommandContextParsed(TOnCommandContextReady slot)
    {
    }

private:
};

void CommandsParser::parseCommandContext(std::string_view commandContext)
{
    m_pImpl->parseCommandContext(commandContext);
}

boost::signals2::connection CommandsParser::subscribeOnCommandContextReady(
    TOnCommandContextReady slot)
{
    return m_pImpl->subscribeOnCommandContextParsed(slot);
}

CommandsParser::CommandsParser() : m_pImpl{std::make_unique<CommandsParserImpl>()}
{
}
CommandsParser::~CommandsParser() = default;

} // namespace Reflow::Commands