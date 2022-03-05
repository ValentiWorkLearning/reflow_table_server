#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <presets/presets_holder.hpp>

MATCHER_P(PresetStageMatcher, presetToCheck, "Presets stages are equal")
{
    const auto& presetStage = std::get<0>(arg);

    return presetStage.tempereatureStep == presetToCheck.tempereatureStep &&
           presetStage.stageDuration == presetToCheck.stageDuration;
}