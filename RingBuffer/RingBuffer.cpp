/*++

Module Name:

    RingBuffer.cpp

Abstract:

    RingBuffer C-ish tutorial.

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <iostream>

struct _RING_BUFFER
{
    size_t ReadIndex;
    size_t WriteIndex;
    uint8_t* Buffer;
    size_t Capacity;
};

using BYTE = uint8_t;
using RINGBUFFER = struct _RING_BUFFER;
using PRINGBUFFER = struct _RING_BUFFER*;

size_t InitializeRingBuffer(PRINGBUFFER RingBuffer, size_t Capacity)
{
    memset(RingBuffer, 0, sizeof(RINGBUFFER));

    RingBuffer->Buffer = reinterpret_cast<uint8_t*>(calloc(Capacity, sizeof(BYTE)));

    if (RingBuffer->Buffer == nullptr)
    {
        return 0;
    }

    RingBuffer->Capacity = Capacity;

    return Capacity;
}

void DestroyRingBuffer(PRINGBUFFER RingBuffer)
{
    free(RingBuffer->Buffer);

    memset(RingBuffer, 0, sizeof(RINGBUFFER));
}

inline size_t FreeSpaceRingBuffer(PRINGBUFFER RingBuffer)
{
    return (RingBuffer->Capacity - RingBuffer->WriteIndex + RingBuffer->ReadIndex);
}

inline size_t CountRingBuffer(PRINGBUFFER RingBuffer)
{
    return (RingBuffer->WriteIndex - RingBuffer->ReadIndex);
}

inline bool IsEmptyRingBuffer(PRINGBUFFER RingBuffer)
{
    return (CountRingBuffer(RingBuffer) == 0);
}

inline bool IsFullRingBuffer(PRINGBUFFER RingBuffer)
{
    return (FreeSpaceRingBuffer(RingBuffer) == 0);
}

size_t WriteRingBuffer(PRINGBUFFER RingBuffer, BYTE* Data, size_t Size)
{
    //
    // Slow version (byte-by-byte)
    //

    const BYTE* Start{ Data };

    while (!IsFullRingBuffer(RingBuffer) && Size--)
    {
        RingBuffer->Buffer[RingBuffer->WriteIndex++ % RingBuffer->Capacity] = *(Data++);
    }

    return Data - Start;
}

size_t ReadRingBuffer(PRINGBUFFER RingBuffer, BYTE* Data, size_t Size)
{
    //
    // Slow version (byte-by-byte)
    //

    const BYTE* Start{ Data };

    while (!IsEmptyRingBuffer(RingBuffer) && Size--)
    {
        *(Data++) = RingBuffer->Buffer[RingBuffer->ReadIndex++ % RingBuffer->Capacity];
    }

    return Data - Start;
}

int main()
{
    std::cout << "Hello RingBuffer!\n\n";

    RINGBUFFER RingBuffer;

    size_t Size = InitializeRingBuffer(&RingBuffer, 16);

    if (Size == 0)
    {
        std::cout << "Could not initialize the ring buffer!\n";
    }

    size_t Written  = WriteRingBuffer(&RingBuffer, (BYTE*)"Hello, World!\n", 14);

    if (Written != 14 || CountRingBuffer(&RingBuffer) != 14)
    {
        std::cout << "Write error!\n";
    }

    uint8_t Byte;
    size_t Read{};

    while (ReadRingBuffer(&RingBuffer, &Byte, 1) == 1)
    {
        std::cout << Byte;
    }

    //
    // There was two unused bytes in the buffer. This
    // second write will wrap around.
    //

    Written  = WriteRingBuffer(&RingBuffer, (BYTE*)"Hello, World!\n", 14);

    if (Written != 14)
    {
        std::cout << "Write error!\n";
    }

    while (ReadRingBuffer(&RingBuffer, &Byte, 1) == 1)
    {
        std::cout << Byte;
    }

    if (CountRingBuffer(&RingBuffer) != 0)
    {
        std::cout << "Count error!\n";
    }

    //
    // Write overflow
    //

    Written  = WriteRingBuffer(&RingBuffer, (BYTE*)"Hello, World!\n******", 20);

    if (Written != 16)
    {
        std::cout << "Write error!\n";
    }

    if (CountRingBuffer(&RingBuffer) != 16)
    {
        std::cout << "Count error!\n";
    }

    BYTE Result[15]{};

    if (ReadRingBuffer(&RingBuffer, Result, 14) == 14)
    {
        std::cout << Result;
    }

    if (CountRingBuffer(&RingBuffer) != 2)
    {
        std::cout << "Count error!\n";
    }

    //
    // Two more bytes before empty.
    //

    if (ReadRingBuffer(&RingBuffer, Result, 3) != 2)
    {
        std::cout << "Read error!\n";
    }

    if (Result[0] != Result[1] || Result[0] != '*')
    {
        std::cout << "Read error!\n";
    }

    DestroyRingBuffer(&RingBuffer);
}
