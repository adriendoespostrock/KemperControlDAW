#pragma once
#include <JuceHeader.h>
#include "DirectMidiOutput.h"
#include "KemperPort.h"
#include "KemperProtocol.h"

// Live copy of the Profiler's state via the SysEx beacon. Runs on the message thread.
class KemperLink final : private juce::Timer
{
public:
    explicit KemperLink(DirectMidiOutput& output) : directOutput(output) { startTimerHz(10); }
    ~KemperLink() override
    {
        stopTimer();
        port.close();
    }
    kemper::RigState snapshot() const
    {
        const juce::ScopedLock lock(stateLock);
        return state;
    }
    std::uint64_t version() const noexcept { return stateVersion.load(); }
    void pollSoon() { pollAt = juce::Time::getMillisecondCounterHiRes() + 300.0; }
    std::function<void(const kemper::RigState&)> onChange;
private:
    static constexpr double ackTimeoutMs = 6000.0;

    void handleMessage(const std::uint8_t* data, std::size_t size)
    {
        const auto result = [&]
        {
            const juce::ScopedLock lock(stateLock);
            return kemper::apply(state, data, size, channel.load());
        }();
        if (result == kemper::Result::beaconAck)
            lastAckMs.store(juce::Time::getMillisecondCounterHiRes());
        else if (result == kemper::Result::changed)
            stateVersion.fetch_add(1);
    }
    void resetState()
    {
        {
            const juce::ScopedLock lock(stateLock);
            state = {};
        }
        lastAckMs.store(-ackTimeoutMs);
        stateVersion.fetch_add(1);
    }
    void timerCallback() override
    {
        const auto settings = directOutput.getSettings();
        channel.store(settings.channel);
        const auto now = juce::Time::getMillisecondCounterHiRes();
        bool reopen = settings.name != openedName;
        if (!reopen && now - lastDeviceCheck > 1000.0 && settings.name.isNotEmpty())
        {
            lastDeviceCheck = now;
            reopen = port.isOpen() != KemperPort::exists(settings.name);
        }
        if (reopen)
        {
            port.close();
            resetState();
            openedName = settings.name;
            if (settings.name.isNotEmpty())
                port.open(settings.name, [this] (const std::uint8_t* data, std::size_t size)
                          { handleMessage(data, size); });
            nextBeacon = 0;
        }

        const bool responding = now - lastAckMs.load() < ackTimeoutMs;
        {
            const juce::ScopedLock lock(stateLock);
            if (state.responding != responding)
            {
                state.responding = responding;
                stateVersion.fetch_add(1);
            }
        }
        if (port.isOpen() && now >= nextBeacon)
        {
            // Full dump until acknowledged, then changes only.
            const auto flags = static_cast<std::uint8_t>(
                kemper::beaconFlags | (responding ? 0 : kemper::beaconInit));
            port.send(kemper::beacon(flags, kemper::beaconLeaseSeconds));
            nextBeacon = now + kemper::beaconLeaseSeconds * 500.0;
            pollEffects = true;
        }
        if (pollAt > 0 && now >= pollAt)
        {
            pollAt = 0;
            pollEffects = true;
        }

        const auto version = stateVersion.load();
        if (version != notifiedVersion)
        {
            notifiedVersion = version;
            const auto current = snapshot();
            if (current.rigName != lastRigName) pollEffects = true;
            lastRigName = current.rigName;
            if (onChange) onChange(current);
        }
        if (pollEffects && port.isOpen())
        {
            pollEffects = false;
            for (const auto index : kemper::polledEffects)
                port.send(kemper::requestParameter(kemper::effectPages[index], 0x03));
        }
    }

    DirectMidiOutput& directOutput;
    KemperPort port;
    juce::String openedName;
    double nextBeacon = 0, lastDeviceCheck = 0;
    bool pollEffects = false;
    double pollAt = 0;
    std::uint64_t notifiedVersion = 0;
    std::string lastRigName;
    mutable juce::CriticalSection stateLock;
    kemper::RigState state;
    std::atomic<std::uint64_t> stateVersion{0};
    std::atomic<double> lastAckMs{-ackTimeoutMs};
    std::atomic<int> channel{1};
};
