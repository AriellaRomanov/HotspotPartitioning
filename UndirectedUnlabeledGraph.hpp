template <typename T>
UndirectedUnlabeledGraph<T>::UndirectedUnlabeledGraph(const long size)
: UndirectedGraph<T>(size)
{
}

template <typename T>
UndirectedUnlabeledGraph<T>::UndirectedUnlabeledGraph(const UndirectedUnlabeledGraph<T>& copy)
: UndirectedGraph<T>(copy)
{ 
}

template <typename T>
UndirectedUnlabeledGraph<T>::UndirectedUnlabeledGraph(UndirectedUnlabeledGraph<T>&& source)
: UndirectedGraph<T>(source)
{
}

template <typename T>
UndirectedUnlabeledGraph<T>::~UndirectedUnlabeledGraph()
{
}