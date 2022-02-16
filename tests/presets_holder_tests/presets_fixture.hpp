#pragma once
#include <gtest/gtest.h>
#include <presets/presets_holder.hpp>

class PresetsTesting : public ::testing::Test
{

protected:
    void SetUp() override
    {
        m_presetsHolder = std::make_unique<Reflow::Presets::PresetsHolder>();
    }

protected:
    std::unique_ptr<Reflow::Presets::PresetsHolder> m_presetsHolder;
};