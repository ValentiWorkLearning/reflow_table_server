#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands_parser.hpp>

struct InvalidJsonParam
{
    std::string_view invalidJson;
    std::string_view expectedErrorString;
};

class CommandsParserTesting
    : public ::testing::Test
    , public ::testing::WithParamInterface<InvalidJsonParam>
{

protected:
    void SetUp() override
    {
        m_commandsParser = new Reflow::Commands::CommandsParser();
    }

protected:
    Reflow::Commands::CommandsParser::Ptr m_commandsParser;
};