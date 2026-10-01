#if defined(__APPLE__)
 #include <CoreMIDI/CoreMIDI.h>
#endif
#include "KemperPort.h"

void KemperPort::handleBytes(const std::uint8_t* data, std::size_t size)
{
    const std::lock_guard<std::mutex> lock(callbackLock);
    if (callback) parser.feed(data, size, callback);
}

#if JUCE_MAC
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wdeprecated-declarations")

namespace
{
    juce::String endpointName(MIDIEndpointRef endpoint, CFStringRef property)
    {
        CFStringRef value = nullptr;
        if (MIDIObjectGetStringProperty(endpoint, property, &value) != noErr || value == nullptr)
            return {};
        const auto result = juce::String::fromCFString(value);
        CFRelease(value);
        return result;
    }

    MIDIEndpointRef findEndpoint(const juce::String& name, bool source)
    {
        const auto count = source ? MIDIGetNumberOfSources() : MIDIGetNumberOfDestinations();
        for (ItemCount i = 0; i < count; ++i)
        {
            const auto endpoint = source ? MIDIGetSource(i) : MIDIGetDestination(i);
            if (endpointName(endpoint, kMIDIPropertyDisplayName) == name
                || endpointName(endpoint, kMIDIPropertyName) == name)
                return endpoint;
        }
        return 0;
    }

    void readProc(const MIDIPacketList* list, void* refCon, void*)
    {
        auto& port = *static_cast<KemperPort*>(refCon);
        const auto* packet = &list->packet[0];
        for (UInt32 i = 0; i < list->numPackets; ++i)
        {
            port.handleBytes(packet->data, packet->length);
            packet = MIDIPacketNext(packet);
        }
    }
}

KemperPort::~KemperPort()
{
    close();
    if (client != 0) MIDIClientDispose(client);
}

bool KemperPort::exists(const juce::String& name)
{
    return findEndpoint(name, true) != 0 && findEndpoint(name, false) != 0;
}

bool KemperPort::open(const juce::String& name, Callback onMessage)
{
    close();
    const auto source = findEndpoint(name, true);
    destination = findEndpoint(name, false);
    if (source == 0 || destination == 0) return false;
    {
        const std::lock_guard<std::mutex> lock(callbackLock);
        callback = std::move(onMessage);
    }
    MIDIClientRef newClient = client;
    MIDIPortRef in = 0, out = 0;
    if (newClient == 0
        && MIDIClientCreate(CFSTR("Kemper Control Link"), nullptr, nullptr, &newClient) != noErr)
        return false;
    client = newClient;
    const bool ok = MIDIOutputPortCreate(client, CFSTR("Kemper Link Out"), &out) == noErr
                 && MIDIInputPortCreate(client, CFSTR("Kemper Link In"), readProc, this, &in) == noErr
                 && MIDIPortConnectSource(in, source, nullptr) == noErr;
    inPort = in;
    outPort = out;
    if (!ok) close();
    return ok;
}

void KemperPort::close()
{
    // The lock waits for a read callback still in flight.
    if (inPort != 0) MIDIPortDispose(inPort);
    if (outPort != 0) MIDIPortDispose(outPort);
    inPort = outPort = destination = 0;
    const std::lock_guard<std::mutex> lock(callbackLock);
    callback = nullptr;
    parser.reset();
}

void KemperPort::send(const std::vector<std::uint8_t>& message)
{
    if (outPort == 0 || message.empty() || message.size() > 512) return;
    alignas(MIDIPacketList) std::uint8_t buffer[1024];
    auto* list = reinterpret_cast<MIDIPacketList*>(buffer);
    auto* packet = MIDIPacketListInit(list);
    packet = MIDIPacketListAdd(list, sizeof(buffer), packet, 0, message.size(), message.data());
    if (packet != nullptr) MIDISend(outPort, destination, list);
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#else
KemperPort::~KemperPort() = default;
bool KemperPort::exists(const juce::String&) { return false; }
bool KemperPort::open(const juce::String&, Callback) { return false; }
void KemperPort::close() {}
void KemperPort::send(const std::vector<std::uint8_t>&) {}
#endif
