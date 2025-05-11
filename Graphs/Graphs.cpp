/*++

Module Name:

    Graphs.cpp

Abstract:

    Some C++ graph functions.

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <map>
#include <set>
#include <string>
#include <vector>
#include <deque>

using Vertex = std::string;
using Edge = std::pair<std::string, std::string>;
using AdjacencyList = std::map<Vertex, std::set<Vertex>>;

struct Graph
{
    std::vector<Edge> Edges;
    std::set<Vertex> Vertices;

    AdjacencyList Neighbors;
    std::set<Vertex> Visited;

    bool Dirty{ false };

    void Clear()
    {
        Edges.clear();
        Vertices.clear();
        Neighbors.clear();
        Visited.clear();

        Dirty = false;
    }

    void AddVertex(const std::string& name)
    {
        Vertices.insert(name);

        Dirty = true;
    }

    void AddDirectedEdge(const std::string& first, const std::string& second)
    {
        AddVertex(first);
        AddVertex(second);
    
        Edges.push_back(Edge(first, second));
    }

    void AddUndirectedEdge(const std::string& first, const std::string& second)
    {
        AddDirectedEdge(first, second);
        AddDirectedEdge(second, first);
    }

    void BuildAdjacencyList()
    {
        //
        // Walk the edges and construct the neighbors list.
        //

        for (Edge edge : Edges)
        {
            auto it = Neighbors.find(edge.first);

            if (it != Neighbors.end())
            {
                //
                // Add new neighbor.
                //

                (*it).second.insert(edge.second);
            }
            else
            {
                //
                // Add new vertex and neighbor.
                //

                Neighbors.insert(std::pair<Vertex, std::set<Vertex>>(edge.first, std::set<Vertex>({ edge.second })));
            }
        }

        //
        // Insert any lonely vertex.
        //

        for (Vertex vertex : Vertices)
        {
            auto it = Neighbors.find(vertex);

            if (it == Neighbors.end())
            {
                Neighbors.insert(std::pair<Vertex, std::set<Vertex>>(vertex, std::set<Vertex>()));
            }
        }

        Dirty = false;
    }

    void PreWalk()
    {
        Visited.clear();

        if (Dirty)
        {
            BuildAdjacencyList();
        }
    }

    typedef bool (*WalkCallback)(const Vertex& Name, [[maybe_unused]] void* Context);

    bool DfsWalkWorker(Vertex Name, unsigned int* ComponentSize = nullptr, WalkCallback Callback = nullptr, void* Context = nullptr)
    {
        if (Visited.find(Name) != Visited.end())
        {
            return false;
        }

        if (ComponentSize)
        {
            ++(*ComponentSize);
        }

        Visited.insert(Name);

        if (Callback)
        {
            if (!Callback(Name, Context))
            {
                return true;
            }
        }

        auto it = Neighbors.find(Name);

        if (it != Neighbors.end())
        {
            for (Vertex neighbor : (*it).second)
            {
                DfsWalkWorker(neighbor, ComponentSize, Callback, Context);
            }
        }

        return true;
    }

    bool DfsWalk(Vertex Name, unsigned int* ComponentSize = nullptr, WalkCallback Callback = nullptr, void* Context = nullptr)
    {
        PreWalk();

        if (ComponentSize)
        {
            (*ComponentSize) = 0;
        }

        return DfsWalkWorker(Name, ComponentSize, Callback, Context);
    }

    bool BfsWalkWorker(Vertex Name, unsigned int* ComponentSize = nullptr, WalkCallback Callback = nullptr, void* Context = nullptr)
    {
        if (Visited.find(Name) != Visited.end())
        {
            return false;
        }

        const size_t Initial{ Visited.size() };

        std::deque<Vertex> Queue;

        Visited.insert(Name);
        Queue.push_back(Name);

        while (Queue.size())
        {
            Vertex vertex{ Queue.front() };

            Queue.pop_front();

            if (ComponentSize)
            {
                ++(*ComponentSize);
            }

            if (Callback)
            {
                if (!Callback(vertex, Context))
                {
                    break;
                }
            }

            auto it = Neighbors.find(vertex);

            if (it != Neighbors.end())
            {
                for (Vertex neighbor : (*it).second)
                {
                    if (Visited.find(neighbor) == Visited.end())
                    {
                        Visited.insert(neighbor);
                        Queue.push_back(neighbor);
                    }
                }
            }
        }

        return (Visited.size() > Initial);
    }

    bool BfsWalk(Vertex Name, unsigned int* ComponentSize = nullptr, WalkCallback Callback = nullptr)
    {
        PreWalk();

        if (ComponentSize)
        {
            (*ComponentSize) = 0;
        }

        return BfsWalkWorker(Name, ComponentSize, Callback);
    }

    unsigned int ConnectedComponents(unsigned int& SmallestComponent, unsigned int& LargestComponent)
    {
        PreWalk();

        unsigned componentCount{ 0 };

        LargestComponent = 0;
        SmallestComponent = UINT_MAX;

        for (Vertex vertex : Vertices)
        {
            unsigned int componentSize{ 0 };

            if (DfsWalkWorker(vertex, &componentSize))
            {
                ++componentCount;

                LargestComponent = std::max(LargestComponent, componentSize);
                SmallestComponent = std::min(SmallestComponent, componentSize);
            }
        }

        return componentCount;
    }
};

#include <iostream>

void DumpAdjacencyList(Graph& g)
{
    g.PreWalk();

    if (g.Neighbors.empty())
    {
        std::cout << "The adjacency list is empty.\n";
    }

    for (auto vertex : g.Neighbors)
    {
        bool first{ true };

        std::cout << vertex.first << ": [";

        for (Vertex neighbor : vertex.second)
        {
            if (!first)
            {
                std::cout << ", ";
            }
            else
            {
                first = false;
            }

            std::cout << neighbor;
        }

        std::cout << "]\n";
    }

    std::cout << "\n";
}

bool PrintVertex(const Vertex& Name, [[maybe_unused]] void* Context)
{
    std::cout << Name << "\n";

    return true; // Continue walk
}

int main()
{
    std::cout << "Hello Graphs!\n\n";

    Graph g;

    g.AddUndirectedEdge("1", "2");
    g.AddVertex("3");
    g.AddUndirectedEdge("4", "6");
    g.AddUndirectedEdge("5", "6");
    g.AddUndirectedEdge("7", "6");
    g.AddUndirectedEdge("8", "6");

    DumpAdjacencyList(g);

    unsigned int LargestComponent;
    unsigned int SmallestComponent;

    unsigned int ConnectedComponents{ g.ConnectedComponents(SmallestComponent, LargestComponent) };
 
    if (ConnectedComponents)
    {
        std::cout << "Connected components: " << ConnectedComponents << "\n";
        std::cout << "Smallest component: " << SmallestComponent << "\n";
        std::cout << "Largest component: " << LargestComponent << "\n\n";
    }

    g.Clear();

    g.AddUndirectedEdge("w", "x");
    g.AddUndirectedEdge("x", "y");
    g.AddUndirectedEdge("z", "y");
    g.AddUndirectedEdge("z", "v");
    g.AddUndirectedEdge("w", "v");

    DumpAdjacencyList(g);

    ConnectedComponents = g.ConnectedComponents(SmallestComponent, LargestComponent);
 
    if (ConnectedComponents)
    {
        std::cout << "Connected components: " << ConnectedComponents << "\n";
        std::cout << "Smallest component: " << SmallestComponent << "\n";
        std::cout << "Largest component: " << LargestComponent << "\n\n";
    }

    std::cout << "DFS walk:\n\n";

    g.DfsWalk("w", nullptr, PrintVertex);

    std::cout << "\nBFS walk:\n\n";

    g.BfsWalk("w", nullptr, PrintVertex);

    return (0);
}
