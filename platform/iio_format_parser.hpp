#ifndef IIO_FORMAT_PARSER_HPP
#define IIO_FORMAT_PARSER_HPP


#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/endian/buffers.hpp>

#include <string>
#include <optional>
#include <variant>
#include <charconv>

namespace Platform::Iio
{
class FormatParser : public boost::intrusive_ref_counter<FormatParser>
{
public:
    using Ptr = boost::intrusive_ptr<FormatParser>;

public:
    void setFormat(std::string_view currentFormat)
    {
        m_formatHolder.reset();
        m_parserPosition = 0;

        InternalFormatHolder formatHolder;

        auto viewCopy = currentFormat;
        auto endian = getEndian(takeChunk(viewCopy));

        if(!endian.has_value())
            return;

        formatHolder.endian = endian.value();

        auto isSignedType = isSigned(takeChunk(viewCopy));
        if(!isSignedType)
            return;
        formatHolder.isSigned = isSignedType.value();

        auto valueBits = getValueBits(takeChunk(viewCopy));
        if(!valueBits)
            return;

        auto totalBits = getTotalBits(takeChunk(viewCopy));
        if(!totalBits)
            return;

        auto bitShift = getBitshift(takeChunk(viewCopy));
        if(!bitShift)
            return;
    }

    std::optional<std::string> parseValue(std::uint32_t value) const
    {
        return std::nullopt;
    }
private:
    enum class Endian
    {
        Big,
        Little
    };
    struct InternalFormatHolder
    {
        Endian endian;
        bool isSigned = false;
    };
private:
    std::optional<Endian> getEndian(std::string_view sourceString)
    {
        auto it = sourceString.begin();

        if(it== sourceString.end())
            return std::nullopt;

        if(*it == 'b' || *it =='l')
        {
            auto currentToken = *it;
            if(++it == sourceString.end())
                return std::nullopt;

            if(*it !='e')
                return std::nullopt;

            m_parserPosition =  std::distance(sourceString.begin(),it);

            return (currentToken == 'b')? Endian::Big : Endian::Little;
        }
        return std::nullopt;
    }

    std::optional<bool> isSigned( std::string_view sourceString)
    {
        auto it = sourceString.begin();
        if(it ==  sourceString.end())
            return std::nullopt;

          ++it; //skip :
        if(it == sourceString.end())
            return std::nullopt;
        ++it; // here we go;
        if(it == sourceString.end())
            return std::nullopt;

        m_parserPosition += std::distance(sourceString.begin(),it);

        return *it == 's';
    }

    std::optional<std::uint8_t> getValueBits(std::string_view sourceString)
    {

        auto bitsSubstrIdx = sourceString.find_first_of('/');
        if(bitsSubstrIdx == std::string::npos)
            return std::nullopt;

        auto bitsSubstr = sourceString.substr(1,bitsSubstrIdx); // skip s/u from the previous stage
        std::uint8_t result{};
        auto [ptr, ec] { std::from_chars(bitsSubstr.data(), bitsSubstr.data() + bitsSubstr.size(), result) };
        if(ec != std::errc())
            return std::nullopt;

        m_parserPosition += bitsSubstrIdx;
        return result;
    }


    std::optional<std::uint8_t> getTotalBits(std::string_view sourceString)
    {
        auto bitsSubstrIdx = sourceString.find_first_of('>');
        if(bitsSubstrIdx == std::string::npos)
            return std::nullopt;

        auto bitsSubstr = sourceString.substr(0,bitsSubstrIdx);
        std::uint8_t result{};
        auto [ptr, ec] { std::from_chars(bitsSubstr.data(), bitsSubstr.data() + bitsSubstr.size(), result) };
        if(ec != std::errc())
            return std::nullopt;

        m_parserPosition = bitsSubstrIdx;
        return result;
    }


    std::optional<std::uint8_t> getBitshift(std::string_view sourceString)
    {
        auto bitshiftit = sourceString.begin();
        if(bitshiftit == sourceString.end())
            return std::nullopt;

        ++bitshiftit;
        if(bitshiftit == sourceString.end())
            return std::nullopt;
        if(*bitshiftit!= '>')
            return std::nullopt;

        ++bitshiftit;
        if(bitshiftit == sourceString.end())
            return std::nullopt;

        auto distance = std::distance(bitshiftit,sourceString.end());

        std::uint8_t result{};
        auto [ptr, ec] { std::from_chars(sourceString.data()+distance, sourceString.data() + sourceString.size() - distance, result) };
        if(ec != std::errc())
            return std::nullopt;

        return result;
    }

    std::string_view takeChunk(std::string_view toOffset)
    {
        return toOffset.substr(m_parserPosition);
    }
private:
    std::size_t m_parserPosition;
    std::optional<InternalFormatHolder> m_formatHolder;
};
} // namespace Reflow::Commands

#endif // IIO_FORMAT_PARSER_HPP
