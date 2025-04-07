/*++

Module Name:

    BST.cpp

Abstract:

    Binary Search Tree C-ish tutorial.

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <cassert>
#include <deque>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

//
// Definitions.
//

using KEY = uint64_t;

typedef struct _TREENODE TREENODE, *PTREENODE;

struct _TREENODE
{
    KEY Key;

    PTREENODE Left{};
    PTREENODE Right{};

    _TREENODE(KEY Key = 0) noexcept
        : Key(Key)
    {
    }

    virtual ~_TREENODE()
    {
        if (Left != nullptr)
        {
            delete Left;
        }

        if (Right != nullptr)
        {
            delete Right;
        }
    }

    _TREENODE(_TREENODE&&) = delete;
    _TREENODE(const _TREENODE&) = delete;
    _TREENODE& operator=(const _TREENODE&) = delete;
    _TREENODE& operator=(_TREENODE&&) = delete;
};

//
// Find function. Return nullptr on miss.
//

PTREENODE Find(PTREENODE Node, KEY Key)
{
    if (Node == nullptr)
    {
        return nullptr;
    }

    if (Key == Node->Key)
    {
        return Node;
    }

    if (Key < Node->Key)
    {
        return Find(Node->Left, Key);
    }
    else
    {
        return Find(Node->Right, Key);
    }
}

//
// Insert function. Return nullptr on allocation
// failure, or a new node. Duplicates are ignored
// and the existing node is returned instead.
//

PTREENODE Insert(PTREENODE Node, KEY Key)
{
    if (Node == nullptr)
    {
        return nullptr;
    }

    if (Key == Node->Key)
    {
        //
        // Ignore duplicates.
        //

        return Node;
    }

    if (Key < Node->Key)
    {
        if (Node->Left == nullptr)
        {
            //
            // Return nullptr on allocation failure.
            //

            return Node->Left = new (std::nothrow) TREENODE(Key);
        }
        else
        {
            return Insert(Node->Left, Key);
        }
    }
    else
    {
        if (Node->Right == nullptr)
        {
            //
            // Return nullptr on allocation failure.
            //

            return Node->Right = new (std::nothrow) TREENODE(Key);
        }
        else
        {
            return Insert(Node->Right, Key);
        }
    }
}

//
// Helper for the test app.
//

void Search(PTREENODE Node, KEY Key)
{
    const TREENODE* Result{ Find(Node, Key) };

    if (Result != nullptr)
    {
        if (Key != Result->Key)
        {
            std::cout << "    Find(" << Key << ") == " << Result->Key << " -- This is wrong!\n";
        }
    }
    else
    {
        std::cout << "    Find(" << Key << ") returned nullptr (the key was not found)\n";
    }
}

//
// Tree traversal.
//

void DFS_PreOrder(PTREENODE Node)
{
    if (Node == nullptr)
    {
        return;
    }

    std::cout << "        " << Node->Key << " \n";

    DFS_PreOrder(Node->Left);
    DFS_PreOrder(Node->Right);
}

void DFS_InOrder(PTREENODE Node)
{
    if (Node == nullptr)
    {
        return;
    }

    DFS_InOrder(Node->Left);

    std::cout << "        " << Node->Key << " \n";

    DFS_InOrder(Node->Right);
}

void DFS_PostOrder(PTREENODE Node)
{
    if (Node == nullptr)
    {
        return;
    }

    DFS_InOrder(Node->Left);
    DFS_InOrder(Node->Right);

    std::cout << "        " << Node->Key << " \n";
}

void BFS_LevelOrderWorker(std::deque<PTREENODE>& Queue)
{
    size_t Nodes{ Queue.size() };

    assert(Nodes > 0);

    //
    // Consume the nodes at this level from the front
    // of the queue, and add the nodes from the level
    // below at the end of the same queue.
    //

    while (Nodes--)
    {
        PTREENODE Node{ Queue.front() };

        Queue.pop_front();

        std::cout << "        " << Node->Key << " \n";

        if (Node->Left != nullptr)
        {
            Queue.push_back(Node->Left);
        }

        if (Node->Right != nullptr)
        {
            Queue.push_back(Node->Right);
        }
    }

    if (Queue.size() > 0)
    {
        std::cout << "     -------\n";

        BFS_LevelOrderWorker(Queue);
    }
}

void BFS_LevelOrder(PTREENODE Node)
{
    if (Node == nullptr)
    {
        return;
    }

    std::deque<PTREENODE> Queue;

    Queue.push_back(Node);

    BFS_LevelOrderWorker(Queue);
}

int main()
{
    std::cout << "Hello Binary Search Tree\n\n";

    //
    // Create a vector with some keys in random order.
    //

    constexpr size_t KEYS{ 16 };

    std::vector<KEY> Keys(KEYS);
    std::iota(Keys.begin(), Keys.end(), 1);

    std::random_device rd;
    std::mt19937 rng(rd());

    std::shuffle(Keys.begin(), Keys.end(), rng);

    std::cout << "Here are the keys for this run:\n\n";

    for (const KEY& Key : Keys)
    {
        std::cout << "    " << Key << " \n";
    }

    std::cout << "\n";

    //
    // Create the root node of the tree. Initialize the
    // root key with the middle value of the key vector for
    // good measure.
    //

    auto MiddleKey{ Keys[Keys.size() / 2] };
    auto Root{ std::make_unique<TREENODE>(MiddleKey) };

    //
    // Insert keys. Note that any duplicates are ignored.
    //

    std::cout << "Inserting " << Keys.size() << " keys. The root key happens to be " << Root->Key << " today.\n\n";

    for (const KEY& Key : Keys)
    {
        Insert(Root.get(), Key);
    }

    //
    // Search for all keys.
    //

    std::cout << "Searching for each key now. Any misses will be printed.\n\n";

    for (const KEY& Key : Keys)
    {
        Search(Root.get(), Key);
    }

    //
    // For good measure, swarch for a key that we know isn't there.
    //

    std::cout << "Searching for a non-existent key.\n\n";

    Search(Root.get(), KEYS + 1);

    //
    // Traversals.
    //

    std::cout << "\nVarious traversals.\n";

    std::cout << "\n    DFS PreOrder.\n\n";

    DFS_PreOrder(Root.get());

    std::cout << "\n    DFS InOrder.\n\n";

    DFS_InOrder(Root.get());

    std::cout << "\n    DFS PostOrder.\n\n";

    DFS_PostOrder(Root.get());

    std::cout << "\n    BFS LevelOrder.\n\n";

    BFS_LevelOrder(Root.get());

    std::cout << "\nDone.\n";
}
