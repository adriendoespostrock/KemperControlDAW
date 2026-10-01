#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Kemper Profiler SysEx protocol, see tools/KEMPER_PROTOCOL.md.
namespace kemper
{
    // A, B, C, D, X, MOD, DLY, REV
    constexpr std::array<std::uint8_t, 8> effectPages =
        { 0x32, 0x33, 0x34, 0x35, 0x38, 0x3a, 0x3c, 0x3d };

    constexpr std::uint8_t beaconInit = 0x01;   // full state dump
    constexpr std::uint8_t beaconFlags = 0x02;
    constexpr std::uint8_t beaconLeaseSeconds = 5;

    struct EffectState
    {
        int type = -1;  // -1 unknown, 0 empty
        int on = -1;    // -1 unknown, 0 off, 1 on
        std::string name;
    };

    struct RigState
    {
        bool responding = false;
        std::string rigName, performanceName;
        std::array<std::string, 5> slotNames;
        std::array<int, 5> slotEnabled { -1, -1, -1, -1, -1 };
        int bankLsb = 0;
        int performance = 0;  // 1-based, 0 unknown
        int slot = 0;         // 1-5, 0 unknown
        std::array<EffectState, 8> effects;
    };

    enum class Result { ignored, changed, beaconAck };

    // Reassembles complete messages from a raw MIDI 1.0 byte stream.
    class StreamParser
    {
    public:
        template <typename OnMessage>
        void feed(const std::uint8_t* data, std::size_t size, OnMessage&& onMessage)
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                const auto byte = data[i];
                if (byte >= 0xf8) continue;
                if (byte == 0xf0)
                {
                    inSysex = true;
                    sysex.assign(1, byte);
                    continue;
                }
                if (inSysex)
                {
                    if (byte == 0xf7)
                    {
                        sysex.push_back(byte);
                        onMessage(sysex.data(), sysex.size());
                        inSysex = false;
                        continue;
                    }
                    if (byte < 0x80)
                    {
                        if (sysex.size() < maxSysexSize) sysex.push_back(byte);
                        continue;
                    }
                    inSysex = false;
                }
                if (byte >= 0xf0)
                {
                    pending.clear();
                    continue;
                }
                if (byte >= 0x80) pending.assign(1, byte);
                else if (!pending.empty()) pending.push_back(byte);
                else continue;
                if (pending.size() == channelMessageLength(pending[0]))
                {
                    onMessage(pending.data(), pending.size());
                    pending.resize(1);  // running status
                }
            }
        }
        void reset()
        {
            inSysex = false;
            sysex.clear();
            pending.clear();
        }
    private:
        static constexpr std::size_t maxSysexSize = 4096;
        static std::size_t channelMessageLength(std::uint8_t status) noexcept
        {
            const auto type = status & 0xf0;
            return type == 0xc0 || type == 0xd0 ? 2 : 3;
        }
        bool inSysex = false;
        std::vector<std::uint8_t> sysex, pending;
    };

    inline std::vector<std::uint8_t> beacon(std::uint8_t flags, std::uint8_t lease)
    {
        return { 0xf0, 0x00, 0x20, 0x33, 0x02, 0x7f, 0x7e, 0x00, 0x40, 0x02,
                 flags, lease, 0xf7 };
    }

    inline std::vector<std::uint8_t> requestParameter(std::uint8_t page, std::uint8_t param)
    {
        return { 0xf0, 0x00, 0x20, 0x33, 0x02, 0x7f, 0x41, 0x00, page, param, 0xf7 };
    }

    // The beacon does not push DLY/REV on/off.
    constexpr std::array<std::size_t, 2> polledEffects = { 6, 7 };

    namespace detail
    {
        inline std::string readString(const std::uint8_t* p, const std::uint8_t* end)
        {
            std::string text;
            while (p < end && *p != 0) text.push_back(static_cast<char>(*p++));
            return text;
        }

        inline int effectIndex(std::uint8_t page)
        {
            for (std::size_t i = 0; i < effectPages.size(); ++i)
                if (effectPages[i] == page) return static_cast<int>(i);
            return -1;
        }

        // 00 00 01 00 <n>: performance (0) or slot 1-5.
        inline int performanceSlotIndex(const std::uint8_t* c)
        {
            if (c[0] != 0 || c[1] != 0 || c[2] != 1 || c[3] != 0 || c[4] > 5) return -1;
            return c[4];
        }

        template <typename T>
        bool assign(T& target, const T& value)
        {
            if (target == value) return false;
            target = value;
            return true;
        }
    }

    // `data` is one complete MIDI message.
    inline Result apply(RigState& state, const std::uint8_t* data, std::size_t size, int channel)
    {
        using namespace detail;
        if (size == 0) return Result::ignored;
        const auto status = data[0];

        if (status >= 0x80 && status < 0xf0)
        {
            if ((status & 0x0f) != channel - 1 || size < 2) return Result::ignored;
            if ((status & 0xf0) == 0xb0 && size >= 3 && data[1] == 32)
            {
                state.bankLsb = data[2];
                return Result::ignored;
            }
            if ((status & 0xf0) == 0xc0)
            {
                const int absolute = state.bankLsb * 128 + data[1];
                const bool a = assign(state.performance, absolute / 5 + 1);
                const bool b = assign(state.slot, absolute % 5 + 1);
                return a || b ? Result::changed : Result::ignored;
            }
            return Result::ignored;
        }

        if (size < 9 || status != 0xf0 || data[1] != 0x00 || data[2] != 0x20
            || data[3] != 0x33 || data[size - 1] != 0xf7)
            return Result::ignored;

        const auto function = data[6];
        const auto* end = data + size - 1;
        const auto length = static_cast<std::size_t>(end - (data + 7));

        switch (function)
        {
            case 0x7e:
                return length >= 2 && data[8] == 0x7f ? Result::beaconAck : Result::ignored;

            case 0x03:  // string
                if (length >= 3 && data[8] == 0x00 && data[9] == 0x01)
                    return assign(state.rigName, readString(data + 10, end))
                        ? Result::changed : Result::ignored;
                return Result::ignored;

            case 0x07:  // extended string
            {
                if (length < 6) return Result::ignored;
                const int n = performanceSlotIndex(data + 8);
                if (n < 0) return Result::ignored;
                auto& target = n == 0 ? state.performanceName
                                      : state.slotNames[static_cast<std::size_t>(n - 1)];
                return assign(target, readString(data + 13, end))
                    ? Result::changed : Result::ignored;
            }

            case 0x06:  // extended parameter
            {
                if (length < 11) return Result::ignored;
                const int n = performanceSlotIndex(data + 8);
                if (n < 1) return Result::ignored;
                return assign(state.slotEnabled[static_cast<std::size_t>(n - 1)],
                              data[17] != 0 ? 1 : 0)
                    ? Result::changed : Result::ignored;
            }

            case 0x01:  // single parameter
            {
                if (length < 5) return Result::ignored;
                const int index = effectIndex(data[8]);
                if (index < 0) return Result::ignored;
                auto& effect = state.effects[static_cast<std::size_t>(index)];
                const int value = (data[10] << 7) | data[11];
                if (data[9] == 0x00)
                    return assign(effect.type, value) ? Result::changed : Result::ignored;
                if (data[9] == 0x03)
                    return assign(effect.on, value != 0 ? 1 : 0)
                        ? Result::changed : Result::ignored;
                return Result::ignored;
            }

            case 0x3c:  // effect short name
            {
                if (length < 6 || data[9] != 0x00) return Result::ignored;
                const int index = effectIndex(data[8]);
                if (index < 0) return Result::ignored;
                auto& effect = state.effects[static_cast<std::size_t>(index)];
                const bool a = assign(effect.type, (data[10] << 7) | data[11]);
                const bool b = assign(effect.name, readString(data + 12, end));
                return a || b ? Result::changed : Result::ignored;
            }

            default:
                return Result::ignored;
        }
    }
}
