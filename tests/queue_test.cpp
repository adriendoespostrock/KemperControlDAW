#include "../Source/MidiPacketQueue.h"
#include <cassert>
#include <thread>
#include <iostream>
int main()
{
    MidiPacketQueue<8> queue;
    MidiPacket p, received;
    assert(!queue.peek(received));
    for (int i = 0; i < 7; ++i) { p.generation = i; assert(queue.push(p)); }
    assert(!queue.push(p));
    assert(queue.freeSpace() == 0);
    for (int i = 0; i < 7; ++i)
    { assert(queue.peek(received)); assert(received.generation == static_cast<unsigned>(i)); queue.pop(); }
    assert(queue.freeSpace() == 7);
    std::thread producer([&]
    {
        for (int i = 0; i < 100000; ++i)
        {
            MidiPacket packet; packet.generation = static_cast<unsigned>(i);
            packet.size = 3; packet.bytes = {0xb0, 17, static_cast<unsigned char>(i % 128)};
            while (!queue.push(packet)) std::this_thread::yield();
        }
    });
    for (int i = 0; i < 100000; ++i)
    {
        while (!queue.peek(received)) std::this_thread::yield();
        assert(received.generation == static_cast<unsigned>(i));
        assert(received.bytes[2] == i % 128);
        queue.pop();
    }
    producer.join();
    assert(!queue.peek(received));
    std::cout << "Queue capacity, wraparound, FIFO ordering and 100000 concurrent messages: PASS\n";
}
