#pragma once


namespace Reflow::Controller
{
struct RegulatorParams
{
    float k;
    std::uint32_t hysteresis;
};
}