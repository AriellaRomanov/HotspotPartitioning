#pragma once
#include "UndirectedGraph.h"
#include <set>
#include <tuple>

std::vector<std::string> split(std::string value, const std::string delimiter);

template <typename T>
class UndirectedUnlabeledGraph : public UndirectedGraph<T>
{
  public:
    UndirectedUnlabeledGraph(const long size = 1);
    UndirectedUnlabeledGraph(const UndirectedUnlabeledGraph<T>& copy);
    UndirectedUnlabeledGraph(UndirectedUnlabeledGraph<T>&& source);
    virtual ~UndirectedUnlabeledGraph();

    friend ostream& operator<<(ostream& os, const UndirectedUnlabeledGraph& graph)
    {
      os << graph.matrix << std::endl;
      return os;
    }

    friend ifstream& operator>>(ifstream& is, UndirectedUnlabeledGraph<T>& graph)
    {
      std::set<std::pair<long, long>> edges;
      long max_id = -1;

      std::string line;
      while (std::getline(is, line))
      {
        auto ids = split(line, ",");
        auto a = atoi(ids.at(0).c_str());
        auto b = atoi(ids.at(1).c_str());
        auto w = atoi(ids.at(2).c_str());

        if (w > 0)
          edges.insert(std::make_pair(a, b));
        if (a > max_id)
          max_id = a;
        if (b > max_id)
          max_id = b;
      }

      graph.graph_size = max_id + 1;
      graph.matrix = SymMatrix<T>(graph.graph_size);

      for (const auto& itr : edges)
        graph.matrix(itr.first, itr.second, 1);

      return is;
    }
};

#include "UndirectedUnlabeledGraph.hpp"
