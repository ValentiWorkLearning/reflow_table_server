#pragma once
#include <gtest/gtest.h>
#include <presets/presets_holder.hpp>

class PresetsTesting : public ::testing::Test
{

protected:
    void SetUp() override
    {
        m_presetsHolder = new Reflow::Presets::PresetsHolder();
    }

protected:
    Reflow::Presets::PresetsHolder::Ptr m_presetsHolder;
};