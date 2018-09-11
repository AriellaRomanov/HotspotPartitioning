#pragma once
#include "UndirectedGraph.h"

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
      is >> graph.graph_size;

      graph.matrix = SymMatrix<T>(graph.graph_size);
      for (long row = 0; row < graph.graph_size; row++)
      {
        for (long col = 0; col < graph.graph_size; col++)
        {
          T value(0);
          is >> value;
          graph.matrix(row, col, value);
        }
      }

      return is;
    }
};

#include "UndirectedUnlabeledGraph.hpp"