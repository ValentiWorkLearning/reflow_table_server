#ifndef IIO_FORMAT_PARSER_HPP
#define IIO_FORMAT_PARSER_HPP

#include <boost/endian/arithmetic.hpp>
#include <boost/endian/buffers.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <charconv>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <variant>

namespace Platform::Iio
{

template <typename Stub> struct NativeEndian
{
};

struct SignedTag
{
};

struct UnsignedTag
{
};

template <> struct NativeEndian<SignedTag>
{
    using TBits8 = boost::endian::native_int8_t;
    using TBits16 = boost::endian::native_int16_t;
    using TBits32 = boost::endian::native_int32_t;
};

template <> struct NativeEndian<UnsignedTag>
{
    using TBits8 = boost::endian::native_uint8_t;
    using TBits16 = boost::endian::native_uint16_t;
    using TBits32 = boost::endian::native_uint32_t;
};
struct BigEndianTag
{
    using TBits8 = boost::endian::big_int8_t;
    using TUBits8 = boost::endian::big_uint8_t;

    using TBits16 = boost::endian::big_int16_t;
    using TUBits16 = boost::endian::big_uint16_t;

    using TBits32 = boost::endian::big_int32_t;
    using TUBits32 = boost::endian::big_uint32_t;
};

struct LittleEndianTag
{
    using TBits8 = boost::endian::little_int8_t;
    using TUBits8 = boost::endian::little_uint8_t;

    using TBits16 = boost::endian::little_int16_t;
    using TUBits16 = boost::endian::little_uint16_t;

    using TBits32 = boost::endian::little_int32_t;
    using TUBits32 = boost::endian::little_uint32_t;
};


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

        if (!endian.has_value())
            return;

        formatHolder.endian = endian.value();

        auto isSignedType = isSigned(takeChunk(viewCopy));
        if (!isSignedType)
            return;
        formatHolder.isSigned = isSignedType.value();

        auto valueBits = getValueBits(takeChunk(viewCopy));
        if (!valueBits)
            return;
        formatHolder.valueBits = valueBits.value();

        auto totalBits = getTotalBits(takeChunk(viewCopy));
        if (!totalBits)
            return;
        formatHolder.totalBits = totalBits.value();

        auto bitShift = getBitshift(takeChunk(viewCopy));
        if (!bitShift)
            return;

        formatHolder.shift = bitShift.value();

        m_formatHolder.emplace(std::move(formatHolder));
    }

    std::optional<std::string> parseValue(std::uint32_t value) const
    {
        if (!m_formatHolder)
            return std::nullopt;
        return internalParseImpl(value);
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
        std::uint8_t valueBits;
        std::uint8_t totalBits;
        std::uint8_t shift;
    };

private:
    std::optional<Endian> getEndian(std::string_view sourceString)
    {
        auto it = sourceString.begin();

        if (it == sourceString.end())
            return std::nullopt;

        if (*it == 'b' || *it == 'l')
        {
            auto currentToken = *it;
            if (++it == sourceString.end())
                return std::nullopt;

            if (*it != 'e')
                return std::nullopt;

            m_parserPosition = std::distance(sourceString.begin(), it);

            return (currentToken == 'b') ? Endian::Big : Endian::Little;
        }
        return std::nullopt;
    }

    std::optional<bool> isSigned(std::string_view sourceString)
    {
        auto it = sourceString.begin();
        if (it == sourceString.end())
            return std::nullopt;

        ++it; // skip :
        if (it == sourceString.end())
            return std::nullopt;
        ++it; // here we go;
        if (it == sourceString.end())
            return std::nullopt;

        m_parserPosition += std::distance(sourceString.begin(), it);

        return *it == 's';
    }

    std::optional<std::uint8_t> getValueBits(std::string_view sourceString)
    {

        auto bitsSubstrIdx = sourceString.find_first_of('/');
        if (bitsSubstrIdx == std::string::npos)
            return std::nullopt;

        auto bitsSubstr = sourceString.substr(1, bitsSubstrIdx); // skip s/u from the previous stage
        std::uint8_t result{};
        auto [ptr, ec]{
            std::from_chars(bitsSubstr.data(), bitsSubstr.data() + bitsSubstr.size(), result)};
        if (ec != std::errc())
            return std::nullopt;

        m_parserPosition += bitsSubstrIdx;
        return result;
    }

    std::optional<std::uint8_t> getTotalBits(std::string_view sourceString)
    {
        auto bitsSubstrIdx = sourceString.find_first_of('>');
        if (bitsSubstrIdx == std::string::npos)
            return std::nullopt;

        auto bitsSubstr = sourceString.substr(1, bitsSubstrIdx); // skip / from the previous stage
        m_parserPosition += bitsSubstrIdx;

        return extractValueFromSubstring(bitsSubstr);
    }

    std::optional<std::uint8_t> getBitshift(std::string_view sourceString)
    {
        auto bitshiftit = sourceString.begin();
        if (bitshiftit == sourceString.end())
            return std::nullopt;

        auto pos = sourceString.find_last_not_of('>');

        if (pos == std::string::npos)
            return std::nullopt;

        auto expectedSubstr = sourceString.substr(pos);

        return extractValueFromSubstring(expectedSubstr);
    }

    std::optional<std::uint8_t> extractValueFromSubstring(std::string_view substringPart)
    {
        std::uint8_t result{};
        auto [ptr, ec]{std::from_chars(
            substringPart.data(), substringPart.data() + substringPart.size(), result)};
        if (ec != std::errc())
            return std::nullopt;
        return result;
    }

    std::string_view takeChunk(std::string_view toOffset)
    {
        return toOffset.substr(m_parserPosition);
    }

    std::optional<std::string> internalParseImpl(std::uint32_t value) const
    {
        switch (m_formatHolder->endian)
        {
        case Endian::Big:
            return processEndian<BigEndianTag>(value);
            break;

        case Endian::Little:
            return processEndian<LittleEndianTag>(value);
            break;
        }
        return std::nullopt;
    }

    template <typename EndianTag>
    std::optional<std::string> processEndian(std::uint32_t value) const
    {
        if (m_formatHolder->isSigned)
        {
            switch (m_formatHolder->totalBits)
            {
            case 8:
                return processConcreteTag<
                    typename EndianTag::TBits8,
                    typename NativeEndian<SignedTag>::TBits8>(value);
                break;
            case 16:
                return processConcreteTag<
                    typename EndianTag::TBits16,
                    typename NativeEndian<SignedTag>::TBits16>(value);
                break;
            case 32:
                return processConcreteTag<
                    typename EndianTag::TBits32,
                    typename NativeEndian<SignedTag>::TBits32>(value);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (m_formatHolder->totalBits)
            {
            case 8:
                return processConcreteTag<
                    typename EndianTag::TUBits8,
                    typename NativeEndian<UnsignedTag>::TBits8>(value);
                break;
            case 16:
                return processConcreteTag<
                    typename EndianTag::TUBits16,
                    typename NativeEndian<UnsignedTag>::TBits16>(value);
                break;
            case 32:
                return processConcreteTag<
                    typename EndianTag::TUBits32,
                    typename NativeEndian<UnsignedTag>::TBits32>(value);
                break;
            default:
                break;
            }
        }

        return std::nullopt;
    }

    template <typename TConcreteEndianType, typename TNativeEndianType>
    std::optional<std::string> processConcreteTag(std::uint32_t value) const
    {
        TConcreteEndianType bitValue{static_cast<TConcreteEndianType>(value)};
        bitValue &= createBitmask(m_formatHolder->valueBits);
        bitValue = bitValue >> m_formatHolder->shift;
        TNativeEndianType native{bitValue};
        return fmt::format("{}", native);
    }

    constexpr std::uint32_t createBitmask(std::uint8_t valueBits) const
    {
        std::uint32_t bitmask{};
        for (std::uint8_t mask{}; mask < valueBits; ++mask)
        {
            bitmask |= 1 << mask;
        }
        return bitmask;
    }

private:
    std::size_t m_parserPosition;
    std::optional<InternalFormatHolder> m_formatHolder;
};
} // namespace Platform::Iio

#endif // IIO_FORMAT_PARSER_HPP
