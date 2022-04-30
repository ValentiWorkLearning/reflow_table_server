#include "presets_fixture.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "preset_items_matcher.hpp"

TEST_F(PresetsTesting, EmptyPresetsHolderByDefault)
{
    EXPECT_TRUE(m_presetsHolder->isEmpty());
}

TEST_F(PresetsTesting, AddedNewPreset)
{
    /*---------------Setup---------------*/
    constexpr std::string_view kPresetName = "DefaultPreset";

    /*---------------TestingAction-------*/
    auto presetId = m_presetsHolder->addNewPreset(kPresetName);

    /*---------------Assertions---------------*/
    constexpr std::size_t kPresetsCount = 1;
    EXPECT_EQ(m_presetsHolder->getPresetById(presetId)->presetName(), kPresetName);
    EXPECT_EQ(m_presetsHolder->presetsCount(), kPresetsCount);
    EXPECT_EQ(presetId, std::hash<std::string_view>()(kPresetName));
}

TEST_F(PresetsTesting, UpdatePreset)
{
    /*---------------Setup---------------*/
    using namespace std::chrono_literals;

    constexpr std::string_view kPresetName = "DefaultPreset";

    /*---------------TestingAction-------*/
    auto presetId = m_presetsHolder->addNewPreset(kPresetName);

    auto pPreset = m_presetsHolder->getPresetById(presetId);

    auto kFirstStage =
        Reflow::Presets::Preset::StageItem{.temperatureStep = 10, .stageDuration = 10s};
    pPreset->addStageItem(Reflow::Presets::Preset::StageItem{kFirstStage});

    auto kSecondStage =
        Reflow::Presets::Preset::StageItem{.temperatureStep = 20, .stageDuration = 20s};

    pPreset->addStageItem(Reflow::Presets::Preset::StageItem(kSecondStage));

    /*---------------Assertions---------------*/
    constexpr auto kPresetStagesCount = 2;
    EXPECT_EQ(pPreset->numStages(), kPresetStagesCount);
    EXPECT_EQ(pPreset->getStageItem(0), kFirstStage);
    EXPECT_EQ(pPreset->getStageItem(1), kSecondStage);
}

TEST_F(PresetsTesting, RefreshPresetItems)
{
    /*---------------Setup---------------*/
    using namespace std::chrono_literals;

    constexpr std::string_view kPresetName = "DefaultPreset";

    /*---------------TestingAction-------*/
    auto presetId = m_presetsHolder->addNewPreset(kPresetName);

    auto pPreset = m_presetsHolder->getPresetById(presetId);

    auto kFirstStage =
        Reflow::Presets::Preset::StageItem{.temperatureStep = 10, .stageDuration = 10s};
    pPreset->addStageItem(Reflow::Presets::Preset::StageItem{kFirstStage});

    auto kSecondStage =
        Reflow::Presets::Preset::StageItem{.temperatureStep = 20, .stageDuration = 20s};

    pPreset->replaceStageItem(0, Reflow::Presets::Preset::StageItem(kSecondStage));

    /*---------------Assertions---------------*/
    constexpr auto kPresetStagesCount = 1;
    EXPECT_EQ(pPreset->numStages(), kPresetStagesCount);
    EXPECT_EQ(pPreset->getStageItem(0), kSecondStage);
}