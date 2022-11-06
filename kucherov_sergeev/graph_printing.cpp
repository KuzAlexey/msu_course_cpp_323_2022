#include "graph_printing.hpp"

namespace uni_course_cpp {
namespace printing {
namespace {
std::vector<int> get_vertices_depth_distribution(const Graph& graph) {
  std::vector<int> vertices_depth_distribution = {};
  const auto depth = graph.get_depth();

  for (Graph::Depth i = 0; i <= depth; i++) {
    vertices_depth_distribution.push_back(graph.get_depth_vertex_ids(i).size());
  }

  return vertices_depth_distribution;
}

std::vector<int> get_edges_color_distribution(const Graph& graph) {
  std::vector<int> edges_color_distribution(sizeof(Graph::Edge::Color), 0);

  const auto& edges = graph.get_edges();

  for (const auto& [edge_id, edge] : edges) {
    edges_color_distribution[(int)edge.color()] += 1;
  }

  return edges_color_distribution;
}
}  // namespace

std::string print_edge_color(Graph::Edge::Color color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Green:
      return "green";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    default:
      return "invalid color";
  }
}

std::string print_vertices_info(const Graph& graph) {
  const auto vertices_depth_distribution =
      get_vertices_depth_distribution(graph);
  std::string vertices_string =
      "vertices: {amount: " + std::to_string(graph.get_vertices().size()) +
      ", distribution: [";

  if (vertices_depth_distribution.size() != 0) {
    for (const auto vertices_count : vertices_depth_distribution) {
      vertices_string += std::to_string(vertices_count) + ", ";
    }
    vertices_string.pop_back();
    vertices_string.pop_back();
  }
  vertices_string += "]},";

  return vertices_string;
}

std::string print_edges_info(const Graph& graph) {
  const auto edges_color_distribution = get_edges_color_distribution(graph);
  std::string edges_string =
      "edges: {amount: " + std::to_string(graph.get_edges().size()) +
      ", distribution: {";

  for (int color_number = 0; color_number < sizeof(Graph::Edge::Color);
       color_number++) {
    Graph::Edge::Color color = (Graph::Edge::Color)color_number;
    edges_string += print_edge_color(color) + ": " +
                    std::to_string(edges_color_distribution.at(color_number)) +
                    ", ";
  }

  if (sizeof(Graph::Edge::Color) != 0) {
    edges_string.pop_back();
    edges_string.pop_back();
  }

  edges_string += "}}";

  return edges_string;
}

std::string print_graph(const Graph& graph) {
  std::string depth_string =
      "depth: " + std::to_string(graph.get_depth()) + ",";
  std::string vertices_string = print_vertices_info(graph);
  std::string edges_string = print_edges_info(graph);

  return "{\n\t" + depth_string + "\n\t" + vertices_string + "\n\t" +
         edges_string + "\n}";
}
}  // namespace printing
}  // namespace uni_course_cpp
