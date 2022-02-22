#include "iio_parser_fixture.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

TEST_F(IioDataParser, DraftFormattingTest)
{
    m_formatParser->setFormat("be:s13/16>>3");

    constexpr auto kThermocoupleData = 172;

    auto result = m_formatParser->parseValue(kThermocoupleData);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(),"21");
}

TEST_F(IioDataParser, EmptyString)
{
    m_formatParser->setFormat("");

    auto result = m_formatParser->parseValue(42);

    EXPECT_FALSE(result.has_value());
}
