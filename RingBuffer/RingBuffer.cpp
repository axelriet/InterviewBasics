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
using SIZE_T = size_t;

struct _RING_BUFFER
{
    BYTE* Buffer;
    SIZE_T Capacity;
    SIZE_T ReadIndex;
    SIZE_T WriteIndex;
};

using RINGBUFFER = struct _RING_BUFFER;

SIZE_T InitializeRingBuffer(RINGBUFFER* RingBuffer, SIZE_T Capacity)
{
    memset(RingBuffer, 0, sizeof(RINGBUFFER));

    if (Capacity > 0)
    {
        //
        // Allocate space for the buffer. Note that the ring buffer functions
        // do not mandate allocating the storage here. You could just as well
        // declare a RINGBUFFER struct, zero it, and make Buffer point to any 
        // memory location of your choosing. Initialize the "Capacity" member
        // to the proper value and you are good to go! In this case, there is
        // no need to call this initialization function at all, and of course
        // neither the DestroyRingBuffer() function! Welcome to the beauty of
        // the C Programming Language.
        //

        RingBuffer->Buffer = static_cast<BYTE*>(malloc(Capacity));

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

inline SIZE_T CountRingBuffer(RINGBUFFER* RingBuffer)
{
    return (RingBuffer->WriteIndex - RingBuffer->ReadIndex);
}

inline SIZE_T FreeSpaceRingBuffer(RINGBUFFER* RingBuffer)
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

inline SIZE_T WriteByteRingBufferUnchecked(RINGBUFFER* RingBuffer, BYTE Data)
{
    RingBuffer->Buffer[RingBuffer->WriteIndex++ % RingBuffer->Capacity] = Data;

    return 1;
}

inline SIZE_T WriteByteRingBuffer(RINGBUFFER* RingBuffer, BYTE Data)
{
    if (IsFullRingBuffer(RingBuffer))
    {
        return 0;
    }

    return WriteByteRingBufferUnchecked(RingBuffer, Data);
}

SIZE_T WriteRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data, SIZE_T Size)
{
    if (RingBuffer->Capacity == 0)
    {
        return 0;
    }

    const SIZE_T Index{ RingBuffer->WriteIndex % RingBuffer->Capacity };
    const SIZE_T Slack{ RingBuffer->Capacity - Index };
    const SIZE_T Space{ FreeSpaceRingBuffer(RingBuffer) };

    const SIZE_T ToWrite{ std::min(Space, Size) };

    if (ToWrite == 0)
    {
        return 0;
    }

    if (ToWrite <= Slack)
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
               Slack);

        memcpy(RingBuffer->Buffer,
               Data + Slack,
               ToWrite - Slack);
    }

    RingBuffer->WriteIndex += ToWrite;

    return ToWrite;
}

inline SIZE_T ReadByteRingBufferUnchecked(RINGBUFFER* RingBuffer, BYTE* Data)
{
    *Data = RingBuffer->Buffer[RingBuffer->ReadIndex++ % RingBuffer->Capacity];

    return 1;
}

inline SIZE_T ReadByteRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data)
{
    if (IsEmptyRingBuffer(RingBuffer))
    {
        return 0;
    }

    return ReadByteRingBufferUnchecked(RingBuffer, Data);
}

SIZE_T ReadRingBuffer(RINGBUFFER* RingBuffer, BYTE* Data, SIZE_T Size)
{
    if (RingBuffer->Capacity == 0)
    {
        return 0;
    }

    const SIZE_T Index{ RingBuffer->ReadIndex % RingBuffer->Capacity };
    const SIZE_T Slack{ RingBuffer->Capacity - Index };
    const SIZE_T Count{ CountRingBuffer(RingBuffer) };

    const SIZE_T ToRead{ std::min(Count, Size) };

    if (ToRead == 0)
    {
        return 0;
    }

    if (ToRead <= Slack)
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
               Slack);

        memcpy(Data + Slack,
               RingBuffer->Buffer,
               ToRead - Slack);
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

    constexpr SIZE_T CAPACITY{ 15 };

    SIZE_T Size = InitializeRingBuffer(&RingBuffer, CAPACITY);

    if (Size == 0)
    {
        std::cout << "Could not initialize the ring buffer!\n";

        return -1;
    }

    SIZE_T Written  = WriteRingBuffer(&RingBuffer, (BYTE*)"Hello, World!\n", 14);

    if (Written != 14 || CountRingBuffer(&RingBuffer) != 14)
    {
        std::cout << "Write error!\n";
    }

    BYTE Byte;
    SIZE_T Read{};

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
