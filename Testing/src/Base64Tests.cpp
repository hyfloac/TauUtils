#include <Base64.hpp>
#include <ConPrinter.hpp>
#include <TauUnit.hpp>
#include <cstring>

static void KnownLengthTests()
{
    TAU_UNIT_TEST();

    {
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(1) == 2, "Failed to compute how many sextets were required for 1 byte without padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(2) == 3, "Failed to compute how many sextets were required for 2 bytes without padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(3) == 4, "Failed to compute how many sextets were required for 3 bytes without padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(4) == 6, "Failed to compute how many sextets were required for 4 bytes without padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(5) == 7, "Failed to compute how many sextets were required for 5 bytes without padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithoutPadding(6) == 8, "Failed to compute how many sextets were required for 6 bytes without padding.");
    }

    {
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(1) == 4, "Failed to compute how many sextets were required for 1 byte with padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(2) == 4, "Failed to compute how many sextets were required for 2 bytes with padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(3) == 4, "Failed to compute how many sextets were required for 3 bytes with padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(4) == 8, "Failed to compute how many sextets were required for 4 bytes with padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(5) == 8, "Failed to compute how many sextets were required for 5 bytes with padding.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPadding(6) == 8, "Failed to compute how many sextets were required for 6 bytes with padding.");
    }

    {
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(1) == 4, "Failed to compute how many sextets were required for 1 byte with padding and trailer.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(2) == 4, "Failed to compute how many sextets were required for 2 bytes with padding and trailer.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(3) == 7, "Failed to compute how many sextets were required for 3 bytes with padding and trailer.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(4) == 8, "Failed to compute how many sextets were required for 4 bytes with padding and trailer.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(5) == 8, "Failed to compute how many sextets were required for 5 bytes with padding and trailer.");
        static_assert(DefaultBase64Codec<char>::RequiredLengthWithPaddingAndTrailer(6) == 11, "Failed to compute how many sextets were required for 6 bytes with padding and trailer.");
    }

    {
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(1) == 0, "Failed to compute how many bytes were required for 1 sextet without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(2) == 1, "Failed to compute how many bytes were required for 2 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(3) == 2, "Failed to compute how many bytes were required for 3 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(4) == 3, "Failed to compute how many bytes were required for 4 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(5) == 3, "Failed to compute how many bytes were required for 5 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(6) == 4, "Failed to compute how many bytes were required for 6 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(7) == 5, "Failed to compute how many bytes were required for 7 sextets without padding.");
        static_assert(DefaultBase64Codec<char>::NumBytesWithoutPadding(8) == 6, "Failed to compute how many bytes were required for 8 sextets without padding.");
    }
}

static void KnownDecodeTests()
{
    TAU_UNIT_TEST();

    constexpr const char* DecodeTarget = "Hello World!";
    constexpr const char* DecodeTargetPadded = "Hello, World!";

    {
        const char* encodedString = "SGVsbG8gV29ybGQh";
        char decodedBuf[13];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(encodedString, ::std::strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode non-padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for non-padded string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTarget, decodedBuf), 0, "Decoding non-padded string did not match target: {} != {}", decodedBuf, DecodeTarget);
    }

    {
        const char* encodedString = "SGVsbG8gV29ybGQh===";
        char decodedBuf[13];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = StrictBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to strictly decode trailed string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for strictly trailed string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTarget, decodedBuf), 0, "Decoding strictly trailed string did not match target: {} != {}", decodedBuf, DecodeTarget);
    }

    {
        const char* encodedString = "SGVsbG8sIFdvcmxkIQ==";
        char decodedBuf[14];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for padded string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetPadded, decodedBuf), 0, "Decoding padded string did not match target: {} != {}", decodedBuf, DecodeTargetPadded);
    }

    {
        const char* encodedString = "SGVsbG8sIFdvcmxkIQ==";
        char decodedBuf[14];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = StrictBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to strictly decode padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for strictly padded string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetPadded, decodedBuf), 0, "Decoding strictly padded string did not match target: {} != {}", decodedBuf, DecodeTargetPadded);
    }
}

static void KnownUrlDecodeTests()
{
    TAU_UNIT_TEST();

    constexpr const char* DecodeTargetSlash = "Hi?";
    constexpr const char* DecodeTargetPlus = "</>";

    {
        const char* encodedString = "SGk/";
        char decodedBuf[4];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode non-url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for non-url string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetSlash, decodedBuf), 0, "Decoding non-url string did not match target: {} != {}", decodedBuf, DecodeTargetSlash);
    }

    {
        const char* encodedString = "SGk_";
        char decodedBuf[4];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = UrlBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for url string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetSlash, decodedBuf), 0, "Decoding url string did not match target: {} != {}", decodedBuf, DecodeTargetSlash);
    }

    {
        const char* encodedString = "PC8+";
        char decodedBuf[4];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode non-url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for non-url string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetPlus, decodedBuf), 0, "Decoding non-url string did not match target: {} != {}", decodedBuf, DecodeTargetPlus);
    }

    {
        const char* encodedString = "PC8-";
        char decodedBuf[4];
        decodedBuf[sizeof(decodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = UrlBase64Codec<char>::Transform(encodedString, strlen(encodedString), reinterpret_cast<u8*>(decodedBuf), sizeof(decodedBuf) - 1, &writeLength);

        TAU_UNIT_EQ(retCode, 0, "Failed to decode url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(decodedBuf) - 1, "Decode length did not match for url string. {} != {}", writeLength, sizeof(decodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(DecodeTargetPlus, decodedBuf), 0, "Decoding url string did not match target: {} != {}", decodedBuf, DecodeTargetPlus);
    }
}

static void KnownEncodeTests()
{
    TAU_UNIT_TEST();

    constexpr const char* RawString = "Hello World!";
    constexpr const char* RawStringPadded = "Hello, World!";

    constexpr const char* EncodeTarget = "SGVsbG8gV29ybGQh";
    constexpr const char* EncodeTargetTrailed = "SGVsbG8gV29ybGQh===";
    constexpr const char* EncodeTargetPadded = "SGVsbG8sIFdvcmxkIQ==";

    {
        char encodedBuf[17];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawString), strlen(RawString), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, false, false);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode non-padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for non-padded string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTarget, encodedBuf), 0, "Encoding non-padded string did not match target: {} != {}", encodedBuf, EncodeTarget);
    }

    {
        char encodedBuf[20];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = StrictBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawString), strlen(RawString), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, true, true);

        TAU_UNIT_EQ(retCode, 0, "Failed to strictly encode trailed string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for strictly trailed string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetTrailed, encodedBuf), 0, "Encoding strictly trailed string did not match target: {} != {}", encodedBuf, EncodeTargetTrailed);
    }

    {
        char encodedBuf[21];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringPadded), strlen(RawStringPadded), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, true, true);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for padded string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetPadded, encodedBuf), 0, "Encoding padded string did not match target: {} != {}", encodedBuf, EncodeTargetPadded);
    }

    {
        char encodedBuf[21];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = StrictBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringPadded), strlen(RawStringPadded), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, true, true);

        TAU_UNIT_EQ(retCode, 0, "Failed to strictly encode padded string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for strictly padded string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetPadded, encodedBuf), 0, "Encoding strictly padded string did not match target: {} != {}", encodedBuf, EncodeTargetPadded);
    }
}

static void KnownUrlEncodeTests()
{
    TAU_UNIT_TEST();

    constexpr const char* RawStringSlash = "Hi?";
    constexpr const char* RawStringPlus = "</>";

    constexpr const char* EncodeTargetSlash = "SGk/";
    constexpr const char* EncodeTargetUnderscore = "SGk_";
    constexpr const char* EncodeTargetPlus = "PC8+";
    constexpr const char* EncodeTargetHyphen = "PC8-";

    {
        char encodedBuf[5];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringSlash), strlen(RawStringSlash), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, false, false);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode non-url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for non-url string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetSlash, encodedBuf), 0, "Encoding non-url string did not match target: {} != {}", encodedBuf, EncodeTargetSlash);
    }

    {
        char encodedBuf[5];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = UrlBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringSlash), strlen(RawStringSlash), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, false, false);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for url string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetUnderscore, encodedBuf), 0, "Encoding url string did not match target: {} != {}", encodedBuf, EncodeTargetUnderscore);
    }

    {
        char encodedBuf[5];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = DefaultBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringPlus), strlen(RawStringPlus), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, false, false);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode non-url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for non-url string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetPlus, encodedBuf), 0, "Encoding non-url string did not match target: {} != {}", encodedBuf, EncodeTargetPlus);
    }

    {
        char encodedBuf[5];
        encodedBuf[sizeof(encodedBuf) - 1] = '\0';
        uSys writeLength;
        const int retCode = UrlBase64Codec<char>::Transform(reinterpret_cast<const u8*>(RawStringPlus), strlen(RawStringPlus), encodedBuf, sizeof(encodedBuf) - 1, &writeLength, false, false);

        TAU_UNIT_EQ(retCode, 0, "Failed to encode url string. {}", retCode);
        TAU_UNIT_EQ(writeLength, sizeof(encodedBuf) - 1, "Encode length did not match for url string. {} != {}", writeLength, sizeof(encodedBuf) - 1);
        TAU_UNIT_EQ(::std::strcmp(EncodeTargetHyphen, encodedBuf), 0, "Encoding url string did not match target: {} != {}", encodedBuf, EncodeTargetHyphen);
    }
}

void Base64Tests()
{
    KnownLengthTests();
    KnownDecodeTests();
    KnownUrlDecodeTests();
    KnownEncodeTests();
    KnownUrlEncodeTests();
}
