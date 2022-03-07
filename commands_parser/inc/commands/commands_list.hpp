#pragma once
#include <variant>

namespace Reflow::Commands
{

struct StartReflow
{
};

struct StopReflow
{
};

using TCommandContextVariant = std::variant<StartReflow,StopReflow,std::monostate>;

}