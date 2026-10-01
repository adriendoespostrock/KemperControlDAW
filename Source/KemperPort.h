#pragma once
#include <JuceHeader.h>
#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include "KemperProtocol.h"

// MIDI in/out by endpoint name, using CoreMIDI directly: JUCE 8's SysEx output
// does not reach the Profiler.
class KemperPort final
{
public:
    using Callback = std::function<void(const std::uint8_t*, std::size_t)>;

    KemperPort() = default;
    ~KemperPort();

    static bool exists(const juce::String& name);
    bool open(const juce::String& name, Callback onMessage);
    void close();
    bool isOpen() const noexcept { return outPort != 0 && inPort != 0; }
    void send(const std::vector<std::uint8_t>& message);

    void handleBytes(const std::uint8_t* data, std::size_t size);

private:
    std::uint32_t client = 0, inPort = 0, outPort = 0, destination = 0;
    std::mutex callbackLock;
    Callback callback;
    kemper::StreamParser parser;

    JUCE_DECLARE_NON_COPYABLE(KemperPort)
};
