/*++

Module Name:

    MaxHeap.cpp

Abstract:

    MaxHeap C-ish tutorial.

    See Sedgewick 4th Ed. p. 314ss. for a discussion
    about the flat array storage used here.

    CAUTION: The array size must be (Capacity + 1)

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <numeric>
#include <cassert>

//
// Definitions.
//

typedef struct _MAXHEAP_HEADER
{
    uint32_t Count;
    uint32_t Capacity;
}
MAXHEAP_HEADER, *PMAXHEAP_HEADER;

using MAXHEAP = uint64_t;
using PMAXHEAP = uint64_t*;
using KEY = uint64_t;
using PKEY = uint64_t*;

#define DECLARE_MAXHEAP(__Name__, __Capacity__) MAXHEAP __Name__[(__Capacity__) + 1]; \
                                                MaxHeap_Init(__Name__, __Capacity__)

inline uint32_t MaxHeap_Count(PMAXHEAP MaxHeap);

//
// Internal Helpers.
//

inline MAXHEAP_HEADER& MaxHeapPrivate_GetHeader(PMAXHEAP MaxHeap)
{
    assert(MaxHeap != nullptr);

    return *reinterpret_cast<PMAXHEAP_HEADER>(&MaxHeap[0]);
}

static void MaxHeapPrivate_BubbleUp(PMAXHEAP MaxHeap, uint32_t Index)
{
    assert(Index >= 1);
    assert(Index <= MaxHeap_Count(MaxHeap));

    //
    // Bubble the element at MaxHeap[Index] up to its place.
    //

    while (Index > 1)
    {
        //
        // Parent = (Index / 2);
        //

        uint32_t Parent = (Index >> 1);

        if (MaxHeap[Parent] < MaxHeap[Index])
        {
            std::swap(MaxHeap[Parent], MaxHeap[Index]);

            Index = Parent;
        }
        else
        {
            break;
        }
    }
}

static void MaxHeapPrivate_BubbleDown(PMAXHEAP MaxHeap, uint32_t Index)
{
    assert(Index >= 1);

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    //
    // Bubble the element at MaxHeap[Index] down to its place.
    //

    while (Index < Header.Count)
    {
        //
        // Child = (Index * 2);
        //

        uint32_t Child = (Index << 1);

        //
        // Pick the largest of the children and swap on
        // that side.
        //

        if ((Child < Header.Count) && (MaxHeap[Child + 1] > MaxHeap[Child]))
        {
            ++Child;
        }

        if ((Child <= Header.Count) && (MaxHeap[Child] > MaxHeap[Index]))
        {
            std::swap(MaxHeap[Child], MaxHeap[Index]);

            Index = Child;
        }
        else
        {
            break;
        }
    }
}

//
// MaxHeap API.
//

inline void MaxHeap_Init(PMAXHEAP MaxHeap, uint32_t Capacity)
{
    //
    // Make sure the MaxHeap array has one extra element, as in:
    //
    //     MAXHEAP MaxHeap[Capacity + 1];
    //
    // This is crucially important as the array is 1-based!
    //

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    Header.Count = 0;
    Header.Capacity = Capacity;
}

inline uint32_t MaxHeap_Count(PMAXHEAP MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count;
}

inline uint32_t MaxHeap_Capacity(PMAXHEAP MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Capacity;
}

inline bool MaxHeap_IsFull(PMAXHEAP MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count >= Header.Capacity;
}

inline bool MaxHeap_IsEmpty(PMAXHEAP MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count == 0;
}

inline bool MaxHeap_Push(PMAXHEAP MaxHeap, KEY Key)
{
    if (MaxHeap_IsFull(MaxHeap) == true)
    {
        return false;
    }

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    //
    // Append the new key at the next available position
    // at the end of the MaxHeap array.
    //

    MaxHeap[++Header.Count] = Key;

    //
    // Bubble the new key up to its true place in the heap.
    //

    MaxHeapPrivate_BubbleUp(MaxHeap, Header.Count);

    return true;
}

inline bool MaxHeap_Pop(PMAXHEAP MaxHeap, PKEY Key)
{
    if (MaxHeap_IsEmpty(MaxHeap) == true)
    {
        *Key = 0;

        return false;
    }

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    //
    // Grab the maximal element.
    //

    *Key = MaxHeap[1];

    //
    // Copy the last element to the top position.
    //

    MaxHeap[1] = MaxHeap[Header.Count];

    //
    // Reduce the element count.
    //

    --Header.Count;

    //
    // Bubble the new root down to its true place.
    //

    MaxHeapPrivate_BubbleDown(MaxHeap, 1);

    return true;
}

inline bool MaxHeap_Peek(PMAXHEAP MaxHeap, PKEY Key)
{
    if (MaxHeap_IsEmpty(MaxHeap) == true)
    {
        *Key = 0;

        return false;
    }

    //
    // Grab the maximal element.
    //

    *Key = MaxHeap[1];

    return true;
}

//
// Test/Demo.
//

#include <iostream>
#include <vector>
#include <random>

int main()
{
    std::cout << "Hello MaxHeap!\n";

    //
    // Create a vector with some values in random order.
    //

    constexpr size_t NUM_KEYS{ 16 };

    std::vector<KEY> Keys(NUM_KEYS);
    std::iota(Keys.begin(), Keys.end(), 1);

    std::random_device rd;
    std::mt19937 rng(rd());

    std::shuffle(Keys.begin(), Keys.end(), rng);

    std::cout << "\nHere are the keys for this run:\n\n";

    for (const KEY& Key : Keys)
    {
        std::cout << "    " << Key << " \n";
    }

    std::cout << "\n";

    //
    // The MaxHeap is stored in a simple array. By convention,
    // the first element of the array is not used.
    //
    // Here, we cleverly store the MaxHeap maximum capacity
    // and current element count packed as two uint32_t in the
    // first element.
    //
    // Note that this restricts our capacity to 4 billion
    // elements, sorry :/
    //

    DECLARE_MAXHEAP(MaxHeap, NUM_KEYS);

    assert(MaxHeap_IsFull(MaxHeap) == false);
    assert(MaxHeap_IsEmpty(MaxHeap) == true);
    assert(MaxHeap_Count(MaxHeap) == 0);
    assert(MaxHeap_Capacity(MaxHeap) == NUM_KEYS);

    //
    // Push the randomly ordered values.
    //

    for (const KEY& Key : Keys)
    {
        MaxHeap_Push(MaxHeap, Key);
    }

    KEY Key;

    assert(MaxHeap_IsFull(MaxHeap) == true);
    assert(MaxHeap_IsEmpty(MaxHeap) == false);
    assert(MaxHeap_Count(MaxHeap) == NUM_KEYS);
    assert(MaxHeap_Peek(MaxHeap, &Key) == true);
    assert(Key == NUM_KEYS);

    if (MaxHeap_Peek(MaxHeap, &Key) == true)
    {
        std::cout << "Maximum Key = " << Key << "\n\n";
    }

    //
    // Pop all values, hopefully in decreasing order.
    //

    std::cout << "Popping all keys:\n\n";

    for (size_t x = NUM_KEYS; x > 0; x--)
    {

        if (MaxHeap_Pop(MaxHeap, &Key) == false)
        {
            std::cout << "    ----> Error: the MaxHeap is unexpectedly empty! \n";

            break;
        }

        std::cout << "    " << Key << " \n";

        assert(Key == x);
    }

    assert(MaxHeap_Pop(MaxHeap, &Key) == false);
    assert(MaxHeap_IsFull(MaxHeap) == false);
    assert(MaxHeap_IsEmpty(MaxHeap) == true);
    assert(MaxHeap_Count(MaxHeap) == 0);
}
