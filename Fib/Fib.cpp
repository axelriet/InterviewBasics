/*++

Module Name:

    Fib.cpp

Abstract:

    Fibonacci recursive and memoized C-ish tutorial.

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <cassert>
#include <iostream>
#include <unordered_map>

uint64_t fib_recursive(uint64_t n)
{
    assert(n > 0);

    //
    // The Fibonacci sequence starts with 1, 1, 2, 3... so the first
    // two elements are 1 and 1. Take this into accound and return.
    //

    if (n <= 2)
    {
        return 1;
    }
    else
    {
        //
        // Return the sum of fib(n - 1) and fib (n - 2)
        //

        return fib_recursive(n - 1) + fib_recursive(n - 2);
    }
}

//
// Declare shortcut types memo_t and memo_entry_t for
// our key-value memo and its entries.
//

using memo_t = std::unordered_map<uint64_t, uint64_t>;
using memo_entry_t = std::pair<uint64_t, uint64_t>;

uint64_t fib_memoized(uint64_t n, memo_t& memo)
{
    assert(n > 0);

    //
    // The Fibonacci sequence starts with 1, 1, 2, 3... so the first
    // two elements are 1 and 1. Take this into accound and return.
    //

    if (n <= 2)
    {
        return 1;
    }

    //
    // See if we have a partial result already stored in our
    // key-value memo.
    //

    const auto it{ memo.find(n) };

    if (it != memo.end())
    {
        //
        // We do! Return the previously computed value.
        //

        return (*it).second;
    }
    else
    {
        //
        // Compute the sum of fib(n - 1) and fib (n - 2)
        //

        const uint64_t fib{ fib_memoized(n - 1, memo) + fib_memoized(n - 2, memo) };

        //
        // Store the result in our key-value memo.
        //

        memo.insert(memo_entry_t(n, fib));

        //
        // Return the computed result.
        //

        return fib;
    }
}

int main()
{
    std::cout << "Hello Fibonacci Sequence!\n";

    constexpr uint64_t END{ 46 };
    constexpr uint64_t START{ 40 };

    std::cout << "\nRecursive version: Time -> O(2^N); Space -> O(N)\n\n";

    //
    // Iterate from START to END using the recursive variant.
    //

    for (uint64_t x = START; x <= END; x++)
    {
        std::cout << "fib_recursive(" << x << ") = " << fib_recursive(x) << " \n";
    }

    std::cout << "\nMemoized version: O(N); Space -> O(N)\n\n";

    //
    // Iterate from START to (END * 2) using the memoized variant.
    //

    memo_t memo;

    for (uint64_t x = START; x <= END * 2; x++)
    {
        std::cout << "fib_memoized(" << x << ") = " << fib_memoized(x, memo) << " \n";
    }
}
