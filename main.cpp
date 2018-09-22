#include "UndirectedUnlabeledGraph.h"
#include <set>
#include <vector>
#include <map>
#include <queue>
using mType = long;
using Partition = std::set<long>;
using Parameters = std::map<std::string, std::string>;

void ReadStructures(const std::string& structure_file, std::vector<Partition>& structures);
void SelectHotSpots(std::vector<Partition>& structures, Partition& hotspots, const long count, UndirectedUnlabeledGraph<mType>& graph);
void SortPartitions(std::vector<Partition>& structures);

void DFS(Partition& partition, UndirectedUnlabeledGraph<mType>& graph, const long node_id, const long max_size, Partition& claimed_nodes);
void BFS(Partition& partition, UndirectedUnlabeledGraph<mType>& graph, Partition& claimed_nodes);

void ReadConfig(const std::string& file_path, Parameters& params);
std::string GetParameter(const std::string& key, const Parameters& params, const std::string& def_val);
double GetParameter(const std::string& key, const Parameters& params, const double def_val);

//std::vector<std::string> split(std::string value, const std::string delimiter);

int main(int argc, char *argv[])
{
  /********** variable declarations *******/
  srand(time(NULL));

  /********** file reading ****************/
  // argv[0] is the program name
  // argv[1] is the config file name
  if (argc < 2)
  {
    // if we don't have a file name, we want to quit now
    cout << "Program usage: " << argv[0] <<" <config filename>" << endl;
    return 0;
  }

  Parameters parameters;
  ReadConfig(argv[1], parameters);

  auto graph_file = GetParameter("GraphFilename", parameters, "");
  auto structure_file = GetParameter("StructureFilename", parameters, "");
  auto output_file = GetParameter("OutputFilename", parameters, "");
  long partition_count = GetParameter("PartitionCount", parameters, 1);
  bool use_threading = GetParameter("UseThreading", parameters, 0) != 0;
  bool fill_pool = GetParameter("FillPartitionFromStructure", parameters, 0) != 0;
  std::string graph_delimeter = GetParameter("GraphDelimeter", parameters, " ");

  if (graph_file == "")
  {
    std::cout << "Missing key['GraphFilename'] from configuration file." << std::endl;
    return 0;
  }
  if (structure_file == "")
  {
    std::cout << "Missing key['StructureFilename'] from configuration file." << std::endl;
    return 0;
  }
  if (partition_count <= 0)
  {
    std::cout << "Invalid value for key['PartitionCount']. Value must be greater than zero." << std::endl;
    return 0;
  }


  ifstream file(graph_file);
  if (!file.is_open())
  {
    // on open fail, quit program
    cout << "Could not open file" << endl;
    return 1;
  }

  // create the graph
  std::cout << "Reading graph file" << std::endl;
  UndirectedUnlabeledGraph<mType> graph(1, graph_delimeter);
  try
  {
    file >> graph;
  }
  catch (MatrixErr& err)
  {
    cout << err.what() << endl;
    return 1;
  }
  file.close();

  auto graph_size = graph.GetSize();

  std::cout << "Reading structure file" << std::endl;
  std::vector<Partition> structures;
  ReadStructures(structure_file, structures);  
  
  std::cout << "Selecting hotspots" << std::endl;
  Partition hotspots;
  SelectHotSpots(structures, hotspots, partition_count, graph);

  auto partition_size = graph_size / static_cast<double>(hotspots.size());

  Partition claimed_nodes;
  // the hotspots start the partitions
  std::vector<Partition> partitions;
  for (auto ht : hotspots)
  {
    partitions.push_back({ht});
    if (fill_pool)
    {
      // put the remainder of the hotspot structures in the partitions if they will fit
      for (const auto& st : structures)
      {
        auto itr = st.find(ht);
        if (itr != st.end())
        {
          if (st.size() <= partition_size)
          {
            for (const auto& pt : st)
              partitions.back().insert(pt);
          }
        }
      }
    }

    for (auto& node : partitions.back())
      claimed_nodes.insert(node);
  }

  std::cout << "Partitioning..." << std::endl;
  // ignore the threading parameter for now
  //if (use_threading || !use_threading)
  //{
    //use_threading
  //}
  //else
  {
    //no threading
    // these are still ordered the same as the hotspots, so we don't need to find them again
    long idx = 0;
    for (auto ht : hotspots)
    {
      claimed_nodes.erase(ht);
      DFS(partitions.at(idx++), graph, ht, partition_size, claimed_nodes);
    }

    idx = 0;
    for (auto ht : hotspots)
    {
      BFS(partitions.at(idx++), graph, claimed_nodes);
    }
  }

  if (output_file != "")
  {
    std::ofstream os(output_file.c_str());
    if (os.is_open())
    {
      os << "Partition Count: " << partitions.size() << std::endl;
      for (long i = 0; i < static_cast<long>(partitions.size()); i++)
      {
        os << "  [" << i << "]";
        for (const auto id : partitions.at(i))
          os << "    " << id << std::endl;
      }
      os << "Claimed " << claimed_nodes.size() << " of " << graph.GetSize() - 1 << std::endl;
      os.close();
    }
  }
  else
  {
    std::cout << "Partition Count: " << partitions.size() << std::endl;
    for (long i = 0; i < static_cast<long>(partitions.size()); i++)
    {
      std::cout << "  [" << i << "]" << std::endl;
      for (const auto id : partitions.at(i))
        std::cout << "    " << id << std::endl;
    }
    std::cout << "Claimed " << claimed_nodes.size() << " of " << graph.GetSize() - 1 << std::endl;
  }

  return 0;
}

void ReadStructures(const std::string& structure_file, std::vector<Partition>& structures)
{
  std::ifstream file(structure_file.c_str());
  if (file.is_open())
  {
    std::string line;
    while (std::getline(file, line))
    {
      auto ids = split(line, " ");
      Partition s;
      for (long i = 1; i < static_cast<long>(ids.size()); i++)
        s.insert(std::atoi(ids.at(i).c_str()));

      if (s.size() > 0)
      {
        bool match = false;
        for (long i = 0; i < static_cast<long>(structures.size()) && !match; i++)
          match = (structures.at(i) == s);

        if (!match)
          structures.push_back(s);
      }
    }
    file.close();
  }
}

void SelectHotSpots(std::vector<Partition>& structures, Partition& hotspots, const long count, UndirectedUnlabeledGraph<mType>& graph)
{
  SortPartitions(structures);

  // from each of the largest structures
  for (long i = 0; hotspots.size() < count && i < static_cast<long>(structures.size()); i++)
  {
    // select the node with highest degree
    long max_node_id = -1;
    long max_node_degree = -1;
    for (const auto node_id : structures.at(structures.size() - i - 1))
    {
      auto node_degree = graph.GetDegree(node_id);
      if (node_degree > max_node_degree)
      {
        max_node_id = node_id;
        max_node_degree = node_degree;
      }
    }

    // and make it a hotspot if it isn't already one
    if (max_node_id != -1)
    {
      auto itr = hotspots.find(max_node_id);
      if (itr == hotspots.end())
        hotspots.insert(max_node_id);
    }
  }
}

void SortPartitions(std::vector<Partition>& structures)
{
  // sort the structures based on size
  for (long i = 1; i < static_cast<long>(structures.size()); i++)
  {
      auto key = structures.at(i);
      auto value = structures.at(i).size();
      
      long j = i - 1;
      while (j >= 0 && structures.at(j).size() > value)
      {
          structures.at(j + 1) = structures.at(j);
          j = j - 1;
      }
      structures.at(j + 1) = key;
  }
}

void DFS(Partition& partition, UndirectedUnlabeledGraph<mType>& graph, const long node_id, const long max_size, Partition& claimed_nodes)
{
  if (static_cast<long>(partition.size()) < max_size)
  {
    auto itr = claimed_nodes.find(node_id);
    if (itr != claimed_nodes.end())
      return;

    partition.insert(node_id);
    claimed_nodes.insert(node_id);
    auto neighbors = graph.GetNeighbors(node_id);

    for (long i = 0; i < static_cast<long>(neighbors.size()); i++)
      DFS(partition, graph, neighbors.at(i), max_size, claimed_nodes);
  }
}

void BFS(Partition& partition, UndirectedUnlabeledGraph<mType>& graph, Partition& claimed_nodes)
{
  std::queue<long> que;
  for (auto& node : partition)
    que.push(node);

  while (static_cast<long>(que.size()) > 0)
  {
    auto node_id = que.front();
    que.pop();

    auto neighbors = graph.GetNeighbors(node_id);
    for (auto& n : neighbors)
    {
      auto itr = claimed_nodes.find(n);
      if (itr == claimed_nodes.end())
      {
        partition.insert(n);
        claimed_nodes.insert(n);
        que.push(n);
      }
    }
  }
}

void ReadConfig(const std::string& file_path, Parameters& params)
{
  std::ifstream file(file_path.c_str());
  if (file.is_open())
  {
    // get each line in the file
    std::string line;
    while (file >> line)
    {
      auto idx = line.find("=");
      if (idx != std::string::npos)
      {
        // split each line by the = character
        // and insert into the mapping
        auto key = line.substr(0, idx);
        auto val = line.substr(idx + 1, line.size());
        params.insert(std::make_pair(key, val));
      }
    }

    file.close();
  }
  else
    std::cout << "Error opening config file." << std::endl;
}

std::string GetParameter(const std::string& key, const Parameters& params, const std::string& def_val)
{
  // find the key in the mapping
  auto itr = params.find(key);
  // if key exists, return the value
  if (itr != params.end())
    return itr->second;
  // else return the default
  return def_val;
}

double GetParameter(const std::string& key, const Parameters& params, const double def_val)
{
  // find the key in the mapping
  auto itr = params.find(key);
  // if key exists, return the value
  if (itr != params.end())
    return atof(itr->second.c_str());
  // else return the default
  return def_val;
}

std::vector<std::string> split(std::string value, const std::string delimiter)
{
  std::vector<std::string> tokens;

  size_t pos = 0;
  while ((pos = value.find(delimiter)) != std::string::npos)
  {
    tokens.push_back(value.substr(0, pos));
    value.erase(0, pos + delimiter.length());
  }
  tokens.push_back(value);

  return std::move(tokens);
}
