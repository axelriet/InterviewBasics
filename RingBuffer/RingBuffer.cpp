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

#include <memory>
#include <cstdint>
#include <algorithm>

using BYTE = uint8_t;

struct _RING_BUFFER
{
    BYTE* Buffer;
    size_t Capacity;
    size_t ReadIndex;
    size_t WriteIndex;
};

using RINGBUFFER = struct _RING_BUFFER;

size_t InitializeRingBuffer(RINGBUFFER* RingBuffer, size_t Capacity)
{
    memset(RingBuffer, 0, sizeof(RINGBUFFER));

    if (Capacity > 0)
    {
        RingBuffer->Buffer = reinterpret_cast<uint8_t*>(malloc(Capacity));

        if (RingBuffer->Buffer == nullptr)
        {
            return 0;
        }

        RingBuffer->Capacity = Capacity;
    }

    return Capacity;
}

void DestroyRingBuffer(RINGBUFFER* RingBuffer)
{
    free(RingBuffer->Buffer);

    memset(RingBuffer, 0, sizeof(RINGBUFFER));
}

inline size_t CountRingBuffer(RINGBUFFER* RingBuffer)
{
    return (RingBuffer->WriteIndex - RingBuffer->ReadIndex);
}

inline size_t FreeSpaceRingBuffer(RINGBUFFER* RingBuffer)
{
    return (RingBuffer->Capacity - CountRingBuffer(RingBuffer));
}

inline bool IsEmptyRingBuffer(RINGBUFFER* RingBuffer)
{
    return (CountRingBuffer(RingBuffer) == 0);
}

inline bool IsFullRingBuffer(RINGBUFFER* RingBuffer)
{
    return (CountRingBuffer(RingBuffer) == RingBuffer->Capacity);
}

inline size_t WriteByteRingBuffer(RINGBUFFER* RingBuffer, BYTE Data)
{
    if (IsFullRingBuffer(RingBuffer))
    {
        return 0;
    }

    RingBuffer->Buffer[RingBuffer->WriteIndex++ % RingBuffer->Capacity] = Data;

    return 1;
}

size_t WriteRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data, size_t Size)
{
    if (RingBuffer->Capacity == 0)
    {
        return 0;
    }

    const size_t Index{ RingBuffer->WriteIndex % RingBuffer->Capacity };
    const size_t FreeSpace{ FreeSpaceRingBuffer(RingBuffer) };
    const size_t WriteSlack{ RingBuffer->Capacity - Index };

    size_t ToWrite{ std::min(FreeSpace, Size) };

    if (ToWrite == 0)
    {
        return 0;
    }

    if (ToWrite <= WriteSlack)
    {
        memcpy(RingBuffer->Buffer + Index,
               Data,
               ToWrite);
    }
    else
    {
        //
        // Split the write across the buffer boundary.
        //

        memcpy(RingBuffer->Buffer + Index,
               Data,
               WriteSlack);

        memcpy(RingBuffer->Buffer,
               Data + WriteSlack,
               ToWrite - WriteSlack);
    }

    RingBuffer->WriteIndex += ToWrite;

    return ToWrite;
}

inline size_t ReadByteRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data)
{
    if (IsEmptyRingBuffer(RingBuffer))
    {
        return 0;
    }

    *Data = RingBuffer->Buffer[RingBuffer->ReadIndex++ % RingBuffer->Capacity];

    return 1;
}

size_t ReadRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data, size_t Size)
{
    if (RingBuffer->Capacity == 0)
    {
        return 0;
    }

    const size_t Index{ RingBuffer->ReadIndex % RingBuffer->Capacity };
    const size_t Count{ CountRingBuffer(RingBuffer) };
    const size_t ReadSlack{ RingBuffer->Capacity - Index };

    size_t ToRead{ std::min(Count, Size) };

    if (ToRead == 0)
    {
        return 0;
    }

    if (ToRead <= ReadSlack)
    {
        memcpy(Data,
               RingBuffer->Buffer + Index,
               ToRead);
    }
    else
    {
        //
        // Split the read across the buffer boundary.
        //

        memcpy(Data,
               RingBuffer->Buffer + Index,
               ReadSlack);

        memcpy(Data + ReadSlack,
               RingBuffer->Buffer,
               ToRead - ReadSlack);
    }

    RingBuffer->ReadIndex += ToRead;

    return ToRead;
}

#include <iostream>

int main()
{
    std::cout << "Hello RingBuffer!\n\n";

    RINGBUFFER RingBuffer;

    //
    // Note: the tests below depends on the
    // capacity to be 15.
    //

    constexpr size_t CAPACITY{ 15 };

    size_t Size = InitializeRingBuffer(&RingBuffer, CAPACITY);

    if (Size == 0)
    {
        std::cout << "Could not initialize the ring buffer!\n";

        return -1;
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

    if (Written != CAPACITY)
    {
        std::cout << "Write error!\n";
    }

    if (CountRingBuffer(&RingBuffer) != CAPACITY)
    {
        std::cout << "Count error!\n";
    }

    BYTE Result[15]{};

    if (ReadRingBuffer(&RingBuffer, Result, 14) == 14)
    {
        std::cout << Result;
    }

    if (CountRingBuffer(&RingBuffer) != 1)
    {
        std::cout << "Count error!\n";
    }

    //
    // One more byte before empty.
    //

    if (ReadRingBuffer(&RingBuffer, Result, 100) != 1)
    {
        std::cout << "Read error!\n";
    }

    if (Result[0] != '*')
    {
        std::cout << "Read error!\n";
    }

    //
    // Byte-by-byte
    //

    WriteByteRingBuffer(&RingBuffer, 'H');
    WriteByteRingBuffer(&RingBuffer, 'e');
    WriteByteRingBuffer(&RingBuffer, 'l');
    WriteByteRingBuffer(&RingBuffer, 'l');
    WriteByteRingBuffer(&RingBuffer, 'o');
    WriteByteRingBuffer(&RingBuffer, ',');
    WriteByteRingBuffer(&RingBuffer, ' ');
    WriteByteRingBuffer(&RingBuffer, 'W');
    WriteByteRingBuffer(&RingBuffer, 'o');
    WriteByteRingBuffer(&RingBuffer, 'r');
    WriteByteRingBuffer(&RingBuffer, 'l');
    WriteByteRingBuffer(&RingBuffer, 'd');
    WriteByteRingBuffer(&RingBuffer, '!');
    WriteByteRingBuffer(&RingBuffer, '\n');

    while (ReadByteRingBuffer(&RingBuffer, &Byte))
    {
        std::cout << Byte;
    }

    //
    // More tests...
    //

    WriteByteRingBuffer(&RingBuffer, 'H');
    WriteRingBuffer(&RingBuffer, (BYTE*)"el",  2);
    WriteRingBuffer(&RingBuffer, (BYTE*)"lo,", 3);
    WriteByteRingBuffer(&RingBuffer, ' ');
    WriteRingBuffer(&RingBuffer, (BYTE*)"Wo", 2);
    WriteRingBuffer(&RingBuffer, (BYTE*)"rld", 3);
    WriteRingBuffer(&RingBuffer, (BYTE*)"!", 1);
    WriteByteRingBuffer(&RingBuffer, '\n');

    while (ReadByteRingBuffer(&RingBuffer, &Byte))
    {
        std::cout << Byte;
    }

    WriteRingBuffer(&RingBuffer, (BYTE*)"He",  2);
    WriteByteRingBuffer(&RingBuffer, 'l');
    WriteRingBuffer(&RingBuffer, (BYTE*)"lo", 2);
    WriteByteRingBuffer(&RingBuffer, ',');
    WriteByteRingBuffer(&RingBuffer, ' ');
    WriteRingBuffer(&RingBuffer, (BYTE*)"Wor", 3);
    WriteRingBuffer(&RingBuffer, (BYTE*)"l", 1);
    WriteRingBuffer(&RingBuffer, (BYTE*)"d!\n", 3);

    while (ReadRingBuffer(&RingBuffer, &Byte, 1))
    {
        std::cout << Byte;
    }

    DestroyRingBuffer(&RingBuffer);

    //
    // Test idiot-proofing.
    //

    InitializeRingBuffer(&RingBuffer, 0);
    WriteRingBuffer(&RingBuffer, (BYTE*)"Hello, World!", 13);
    WriteByteRingBuffer(&RingBuffer, '\n');
    ReadByteRingBuffer(&RingBuffer, &Byte);
    ReadRingBuffer(&RingBuffer, Result, 256);
    DestroyRingBuffer(&RingBuffer);

    return 0;
}
