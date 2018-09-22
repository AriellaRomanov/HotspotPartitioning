template <typename T>
UndirectedUnlabeledGraph<T>::UndirectedUnlabeledGraph(const long size, const std::string& read_delim)
: UndirectedGraph<T>(size), read_delimeter(read_delim)
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
