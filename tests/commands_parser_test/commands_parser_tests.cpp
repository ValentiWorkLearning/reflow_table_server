#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "commands_parser_fixture.hpp"


MATCHER_P(StringViewMatcher, otherView, "StringView content equals")
{
    const auto& stringView = std::get<0>(arg);
    return std::equal(otherView.begin(),otherView.end(), stringView.begin(), stringView.end());
}


using ::testing::NiceMock;
TEST_F(CommandsParserTesting, ParseCommand)
{
    /*---------------Setup---------------*/
    auto commandJson = R"(
        {
            "command": "start"
        }
    )"_json;
    /*---------------TestingAction-------*/
    auto parseResult = m_commandsParser->parseCommandContext(commandJson.dump());
    EXPECT_TRUE(parseResult.has_value());
}

TEST_P(CommandsParserTesting, ParseError_InvalidJson)
{
    /*---------------Setup---------------*/
    auto commandJson = GetParam().invalidJson;

    /*---------------TestingAction-------*/
    auto parseResult = m_commandsParser->parseCommandContext(commandJson);
    EXPECT_FALSE(parseResult.has_value());
}

INSTANTIATE_TEST_SUITE_P(
    CommandsParserJson,
    CommandsParserTesting,
    ::testing::Values(
        InvalidJsonParam{
            .invalidJson = R"(
        {
            "command" "start"
        }
    )",
            .expectedErrorString = Reflow::Commands::Messages::kInvalidJson},
        InvalidJsonParam{
            .invalidJson = R"(
        {
            "command":"something"
        }
    )",
            .expectedErrorString = Reflow::Commands::Messages::kInvalidCommand},
        InvalidJsonParam{
            .invalidJson = R"(
        {
            "cmd":"something"
        }
    )",
            .expectedErrorString = Reflow::Commands::Messages::kMissedEntry}));