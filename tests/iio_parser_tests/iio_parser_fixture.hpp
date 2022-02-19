#ifndef IIO_PARSER_FIXTURE_HPP
#define IIO_PARSER_FIXTURE_HPP

#include <gtest/gtest.h>
#include <iio_format_parser.hpp>

class IioDataParser : public ::testing::Test
{

protected:
    void SetUp() override
    {
        m_formatParser = new Platform::Iio::FormatParser();
    }

protected:
    Platform::Iio::FormatParser::Ptr m_formatParser;
};
#endif // IIO_PARSER_FIXTURE_HPP
