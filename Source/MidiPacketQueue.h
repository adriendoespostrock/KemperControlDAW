#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>

struct MidiPacket
{
    std::array<unsigned char, 3> bytes{};
    int size = 0;
    std::uint64_t generation = 0;
    double dueMs = 0;
};

// One audio producer and one MIDI worker consumer. No allocation or mutex.
template <std::size_t Capacity>
class MidiPacketQueue
{
public:
    std::size_t freeSpace() const noexcept
    {
        const auto w = write.load(std::memory_order_relaxed);
        const auto r = read.load(std::memory_order_acquire);
        return Capacity - 1 - ((w + Capacity - r) % Capacity);
    }
    bool push(const MidiPacket& packet) noexcept
    {
        const auto w = write.load(std::memory_order_relaxed);
        const auto next = (w + 1) % Capacity;
        if (next == read.load(std::memory_order_acquire)) return false;
        packets[w] = packet;
        write.store(next, std::memory_order_release);
        return true;
    }
    bool peek(MidiPacket& packet) const noexcept
    {
        const auto r = read.load(std::memory_order_relaxed);
        if (r == write.load(std::memory_order_acquire)) return false;
        packet = packets[r];
        return true;
    }
    void pop() noexcept
    {
        read.store((read.load(std::memory_order_relaxed) + 1) % Capacity,
                   std::memory_order_release);
    }
private:
    std::array<MidiPacket, Capacity> packets{};
    std::atomic<std::size_t> write{0}, read{0};
};
