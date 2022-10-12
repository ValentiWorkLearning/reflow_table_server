#pragma once
#include <variant>
#include <string>
#include <tl/expected.hpp>
#include <reflow_controller/reflow_controller_types.hpp>

namespace Reflow::Commands
{

struct StartReflow
{
};

struct StopReflow
{
};

struct SelectPreset
{
    std::size_t presetId;
};

struct SetRegulatorParams
{
    Reflow::Controller::RegulatorParams params;
};

using TCommandVariant = std::variant<StartReflow, StopReflow,SelectPreset,SetRegulatorParams>;
using TCommandContext = tl::expected<TCommandVariant,std::string_view>;

} // namespace Reflow::Commands

namespace Reflow::Commands::Messages
{
constexpr inline std::string_view kInvalidJson =
    "Invalid JSON has been passed to the command parser";
constexpr inline std::string_view kMissedEntry = "Couldn't find the command JSON entry";
constexpr inline std::string_view kInvalidCommand = "Command is not allowed to run";
} // namespace Reflow::Commands::Messages