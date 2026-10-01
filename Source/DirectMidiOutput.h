#pragma once
#include <JuceHeader.h>
#include "MidiPacketQueue.h"

// JUCE 8 MIDI devices must be opened on the message thread; the worker only sends.
class DirectMidiOutput final : private juce::Thread, private juce::Timer
{
public:
    struct Settings { juce::String id, name; int channel = 1; };
    DirectMidiOutput() : Thread("Kemper direct MIDI") { startThread(); startTimerHz(20); }
    ~DirectMidiOutput() override
    {
        stopTimer();
        signalThreadShouldExit();
        notify();
        stopThread(-1);
        const juce::ScopedLock lock(outputLock);
        output.reset();
    }
    Settings getSettings() const
    {
        const juce::ScopedLock lock(settingsLock);
        return settings;
    }
    void configure(Settings next)
    {
        const juce::ScopedLock lock(settingsLock);
        next.channel = juce::jlimit(1, 16, next.channel);
        settings = std::move(next);
        ready.store(0);
        requested.fetch_add(1);
        notify();
    }
    juce::String getStatus() const
    {
        const juce::ScopedLock lock(settingsLock);
        return status + (dropped.load() > 0 ? " | File MIDI saturee : "
            + juce::String(static_cast<int>(dropped.load())) + " blocs ignores" : "");
    }
    std::uint64_t readyGeneration() const noexcept { return ready.load(); }
    int channel() const noexcept { return activeChannel.load(); }
    // Called by processBlock only. A full block is accepted or rejected so
    // NRPN sequences cannot be truncated by queue overflow.
    void enqueue(const juce::MidiBuffer& messages, double sampleRate,
                 std::uint64_t generation) noexcept
    {
        if (generation == 0 || ready.load() != generation) return;
        const auto count = static_cast<std::size_t>(messages.getNumEvents());
        if (queue.freeSpace() < count) { dropped.fetch_add(1); return; }
        const auto now = juce::Time::getMillisecondCounterHiRes();
        for (const auto event : messages)
        {
            // This controller currently generates short CC/PC/NRPN messages.
            if (event.numBytes < 1 || event.numBytes > 3) continue;
            MidiPacket packet;
            packet.size = event.numBytes;
            packet.generation = generation;
            packet.dueMs = now + 1000.0 * event.samplePosition / sampleRate;
            for (int i = 0; i < packet.size; ++i)
                packet.bytes[static_cast<std::size_t>(i)] = event.data[i];
            if (packet.bytes[0] >= 0x80 && packet.bytes[0] < 0xf0)
                packet.bytes[0] = static_cast<unsigned char>(
                    (packet.bytes[0] & 0xf0) | (activeChannel.load() - 1));
            queue.push(packet);
        }
    }
    void setRealtime(bool enabled) noexcept { realtime.store(enabled); }
private:
    void setStatus(const juce::String& text)
    {
        const juce::ScopedLock lock(settingsLock);
        status = text;
    }
    void timerCallback() override
    {
        const auto now = juce::Time::getMillisecondCounterHiRes();
        const auto desired = requested.load();
        if (desired != applied.load())
        {
            ready.store(0);
            {
                const juce::ScopedLock lock(outputLock);
                output.reset();
            }
            current = getSettings();
            auto next = current.id.isNotEmpty() ? juce::MidiOutput::openDevice(current.id)
                                                : nullptr;
            const bool opened = next != nullptr;
            {
                const juce::ScopedLock lock(outputLock);
                output = std::move(next);
                activeChannel.store(current.channel);
                applied.store(desired);
            }
            lastDeviceCheck = now;
            if (opened && requested.load() == desired)
            {
                ready.store(desired);
                setStatus("Connecte : " + current.name);
            }
            else
                setStatus(current.id.isEmpty() ? "Choisir une sortie MIDI"
                                             : "Port indisponible : cliquer Actualiser");
            return;
        }
        if (ready.load() != 0 && now - lastDeviceCheck > 1000.0)
        {
            lastDeviceCheck = now;
            bool exists = false;
            for (const auto& device : juce::MidiOutput::getAvailableDevices())
                if (device.identifier == current.id) exists = true;
            if (!exists)
            {
                ready.store(0);
                const juce::ScopedLock lock(outputLock);
                output.reset();
                setStatus("Port deconnecte : cliquer Actualiser apres reconnexion");
            }
        }
    }
    void run() override
    {
        while (!threadShouldExit())
        {
            {
                const juce::ScopedLock lock(outputLock);
                const auto generation = applied.load();
                MidiPacket packet;
                while (queue.peek(packet))
                {
                    if (!output || !realtime.load() || packet.generation != generation
                        || packet.generation != requested.load())
                    {
                        queue.pop();
                        continue;
                    }
                    if (packet.dueMs > juce::Time::getMillisecondCounterHiRes()) break;
                    output->sendMessageNow(juce::MidiMessage(packet.bytes.data(), packet.size));
                    queue.pop();
                }
            }
            wait(1);
        }
    }
    juce::CriticalSection outputLock;
    std::unique_ptr<juce::MidiOutput> output;
    Settings current;
    double lastDeviceCheck = 0;
    mutable juce::CriticalSection settingsLock;
    Settings settings;
    juce::String status = "Choisir une sortie MIDI";
    std::atomic<std::uint64_t> requested{1}, applied{0}, ready{0}, dropped{0};
    std::atomic<int> activeChannel{1};
    std::atomic<bool> realtime{true};
    MidiPacketQueue<8192> queue;
};
