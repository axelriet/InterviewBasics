/*++

Module Name:

    MaxHeap.cpp

Abstract:

    MaxHeap C-ish tutorial.

    See Sedgewick 4th Ed. p. 314ss. for a discussion
    about the flat array storage used here.

    CAUTION: The array size must be Capacity + 1

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <iostream>
#include <vector>
#include <random>
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

inline uint32_t MaxHeap_Count(uint64_t* MaxHeap);

//
// Internal Helpers.
//

inline MAXHEAP_HEADER& MaxHeapPrivate_GetHeader(uint64_t* MaxHeap)
{
    assert(MaxHeap != nullptr);

    return *reinterpret_cast<PMAXHEAP_HEADER>(&MaxHeap[0]);
}

static void MaxHeapPrivate_BubbleUp(uint64_t* MaxHeap, uint32_t Index)
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

static void MaxHeapPrivate_BubbleDown(uint64_t* MaxHeap, uint32_t Index)
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

inline void MaxHeap_Init(uint64_t* MaxHeap, uint32_t Capacity)
{
    //
    // Make sure the MaxHeap array has one extra element, as in:
    //
    //     uint64_t MaxHeap[Capacity + 1];
    //
    // This is crucially important as the array is 1-based!
    //

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    Header.Count = 0;
    Header.Capacity = Capacity;
}

inline uint32_t MaxHeap_Count(uint64_t* MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count;
}

inline uint32_t MaxHeap_Capacity(uint64_t* MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Capacity;
}

inline bool MaxHeap_IsFull(uint64_t* MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count >= Header.Capacity;
}

inline bool MaxHeap_IsEmpty(uint64_t* MaxHeap)
{
    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    return Header.Count == 0;
}

inline bool MaxHeap_Push(uint64_t* MaxHeap, uint64_t Value)
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

    MaxHeap[++Header.Count] = Value;

    //
    // Bubble the new key up to its true place in the heap.
    //

    MaxHeapPrivate_BubbleUp(MaxHeap, Header.Count);

    return true;
}

inline bool MaxHeap_Pop(uint64_t* MaxHeap, uint64_t* Value)
{
    if (MaxHeap_IsEmpty(MaxHeap) == true)
    {
        *Value = 0;

        return false;
    }

    MAXHEAP_HEADER& Header{ MaxHeapPrivate_GetHeader(MaxHeap) };

    //
    // Grab the maximal element.
    //

    *Value = MaxHeap[1];

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

inline bool MaxHeap_Peek(uint64_t* MaxHeap, uint64_t* Value)
{
    if (MaxHeap_IsEmpty(MaxHeap) == true)
    {
        *Value = 0;

        return false;
    }

    //
    // Grab the maximal element.
    //

    *Value = MaxHeap[1];

    return true;
}

int main()
{
    std::cout << "Hello MaxHeap!\n";

    //
    // Create a vector with some values in random order.
    //

    constexpr size_t NUM_VALUES{ 16 };

    std::vector<uint64_t> Values(NUM_VALUES);
    std::iota(Values.begin(), Values.end(), 1);

    std::random_device rd;
    std::mt19937 rng(rd());

    std::shuffle(Values.begin(), Values.end(), rng);

    std::cout << "\nHere are the values for this run:\n\n";

    for (const uint64_t& Value : Values)
    {
        std::cout << "    " << Value << " \n";
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

    uint64_t MaxHeap[NUM_VALUES + 1]; // CAUTION: 1-based therefore +1

    MaxHeap_Init(MaxHeap, NUM_VALUES);

    assert(MaxHeap_IsFull(MaxHeap) == false);
    assert(MaxHeap_IsEmpty(MaxHeap) == true);
    assert(MaxHeap_Count(MaxHeap) == 0);
    assert(MaxHeap_Capacity(MaxHeap) == NUM_VALUES);

    //
    // Push the randomly ordered values.
    //

    for (const uint64_t& Value : Values)
    {
        MaxHeap_Push(MaxHeap, Value);
    }

    uint64_t Value;

    assert(MaxHeap_IsFull(MaxHeap) == true);
    assert(MaxHeap_IsEmpty(MaxHeap) == false);
    assert(MaxHeap_Count(MaxHeap) == NUM_VALUES);
    assert(MaxHeap_Peek(MaxHeap, &Value) == true);
    assert(Value == NUM_VALUES);

    //
    // Pop all values, hopefully in decreasing order.
    //

    std::cout << "Popping all keys:\n\n";

    for (size_t x = NUM_VALUES; x > 0; x--)
    {

        if (MaxHeap_Pop(MaxHeap, &Value) == false)
        {
            std::cout << "    ----> Error: the MaxHeap is unexpectedly empty! \n";

            break;
        }

        std::cout << "    " << Value << " \n";

        assert(Value == x);
    }

    assert(MaxHeap_Pop(MaxHeap, &Value) == false);
    assert(MaxHeap_IsFull(MaxHeap) == false);
    assert(MaxHeap_IsEmpty(MaxHeap) == true);
    assert(MaxHeap_Count(MaxHeap) == 0);
}
