#include <commands/commands_list.hpp>
#include <commands/commands_parser.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <tl/expected.hpp>

namespace Reflow::Commands
{

class CommandsParser::CommandsParserImpl
{
public:
    TCommandContext parseCommandContext(std::string_view commandContext)
    {
        try
        {

            auto parsedJson = nlohmann::json::parse(commandContext);
            if (parsedJson.is_discarded())
            {
                return tl::make_unexpected(Reflow::Commands::Messages::kInvalidJson);
            }

            if (auto it = parsedJson.find("command"); it != parsedJson.end())
            {
                auto commandValue = it->get<std::string>();
                if (!isCommandAllowed(commandValue))
                {
                    return tl::make_unexpected(Reflow::Commands::Messages::kInvalidCommand);
                }

                if (commandValue == kStartCommand)
                {
                    return StartReflow{};
                }
                else if (commandValue == kStopCommand)
                {
                    return StopReflow{};
                }
                else if (commandValue == kSelectPresetCommand)
                {
                    if (auto presetHash = parsedJson.find("preset-id");
                        presetHash != parsedJson.end())
                    {
                        return SelectPreset
                        {
                            .presetId = presetHash->get<std::size_t>()
                        };
                    }
                }
            }
            return tl::make_unexpected(Reflow::Commands::Messages::kMissedEntry);
        }
        catch (const nlohmann::json::parse_error& ex)
        {
            return tl::make_unexpected(Reflow::Commands::Messages::kInvalidJson);
        }
    }

private:
    bool isCommandAllowed(std::string_view isAllowedCheck)
    {
        auto it = kAllowedCommandsList.find(isAllowedCheck);
        return it != kAllowedCommandsList.end();
    }

private:
    static constexpr inline std::string_view kStartCommand = "start";
    static constexpr inline std::string_view kStopCommand = "stop";
    static constexpr inline std::string_view kSelectPresetCommand = "select-preset";

    using TAllowedCommands = std::set<std::string_view>;
    static inline TAllowedCommands kAllowedCommandsList{
        kStartCommand,
        kStopCommand,
        kSelectPresetCommand};
};

TCommandContext CommandsParser::parseCommandContext(std::string_view commandContext)
{
    return m_pImpl->parseCommandContext(commandContext);
}

CommandsParser::CommandsParser() : m_pImpl{std::make_unique<CommandsParserImpl>()}
{
}
CommandsParser::~CommandsParser() = default;

} // namespace Reflow::Commands