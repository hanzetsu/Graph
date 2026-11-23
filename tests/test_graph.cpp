#include <gtest/gtest.h>
#include <limits>
#include <string>
#include "graph/Graph.hpp"
#include "graph/GraphGenerator.hpp"
#include "structs/HashTableDictionary.hpp"

using G = IntGraph;
constexpr int INF = std::numeric_limits<int>::max();

namespace {

void build_dijkstra_sample(G& g) {
    for (int i = 0; i < 4; ++i) g.add_vertex(i, "v" + std::to_string(i));
    g.add_undirected_edge(0, 1, 4);
    g.add_undirected_edge(0, 2, 1);
    g.add_undirected_edge(2, 1, 2);
    g.add_undirected_edge(1, 3, 5);
}

void build_disconnected(G& g) {
    for (int i = 0; i < 5; ++i) g.add_vertex(i, "");
    g.add_undirected_edge(0, 1, 1);
    g.add_undirected_edge(2, 3, 2);
}

int dist_to(const MutableArraySequence<int>& d,
            const MutableArraySequence<int>& vs, int v) {
    for (std::size_t i = 0; i < vs.GetLength(); ++i)
        if (vs[i] == v) return d[i];
    return -1;
}

int prev_to(const MutableArraySequence<int>& p,
            const MutableArraySequence<int>& vs, int v) {
    for (std::size_t i = 0; i < vs.GetLength(); ++i)
        if (vs[i] == v) return p[i];
    return -1;
}

}

TEST(GraphBasic, NewGraphIsEmpty) {
    G g;
    EXPECT_EQ(g.get_count_vertex(), 0u);
    EXPECT_EQ(g.get_count_edges(), 0u);
    EXPECT_EQ(g.get_vertices().GetLength(), 0u);
    EXPECT_EQ(g.get_edges().GetLength(), 0u);
}

TEST(GraphBasic, AddVertex) {
    G g;
    g.add_vertex(10, "a");
    g.add_vertex(20, "b");
    EXPECT_EQ(g.get_count_vertex(), 2u);
    EXPECT_TRUE(g.contains_vertex(10));
    EXPECT_TRUE(g.contains_vertex(20));
    EXPECT_FALSE(g.contains_vertex(30));
    EXPECT_EQ(g.get_vertex_data(10), "a");
    EXPECT_EQ(g.get_vertex_data(20), "b");
}

TEST(GraphBasic, AddVertexTwiceIsIdempotent) {
    G g;
    g.add_vertex(1, "x");
    g.add_vertex(1, "y");
    EXPECT_EQ(g.get_count_vertex(), 1u);
    EXPECT_EQ(g.get_vertex_data(1), "x");
}

TEST(GraphBasic, AddEdge) {
    G g;
    g.add_vertex(0, "");
    g.add_vertex(1, "");
    g.add_edge(0, 1, 42);
    EXPECT_EQ(g.get_count_edges(), 1u);
    EXPECT_TRUE(g.contains_edge(0, 1));
    EXPECT_FALSE(g.contains_edge(1, 0));
    EXPECT_EQ(g.get_weight_edge(0, 1), 42);
}

TEST(GraphBasic, AddEdgeToMissingVertexIsIgnored) {
    G g;
    g.add_vertex(0, "");
    g.add_edge(0, 99, 5);
    EXPECT_EQ(g.get_count_edges(), 0u);
    EXPECT_FALSE(g.contains_edge(0, 99));
}

TEST(GraphBasic, AddUndirectedEdgeAddsBothDirections) {
    G g;
    g.add_vertex(0, "");
    g.add_vertex(1, "");
    g.add_undirected_edge(0, 1, 7);
    EXPECT_EQ(g.get_count_edges(), 2u);
    EXPECT_TRUE(g.contains_edge(0, 1));
    EXPECT_TRUE(g.contains_edge(1, 0));
    EXPECT_EQ(g.get_weight_edge(0, 1), 7);
    EXPECT_EQ(g.get_weight_edge(1, 0), 7);
}

TEST(GraphBasic, ClearRemovesEverything) {
    G g;
    build_dijkstra_sample(g);
    ASSERT_GT(g.get_count_vertex(), 0u);
    g.clear();
    EXPECT_EQ(g.get_count_vertex(), 0u);
    EXPECT_EQ(g.get_count_edges(), 0u);
}

TEST(Dijkstra, EmptyGraphThrows) {
    G g;
    MutableArraySequence<int> prev;
    EXPECT_THROW(g.dijkstra(0, prev), vertex_not_found);
}

TEST(Dijkstra, MissingStartVertexThrows) {
    G g;
    build_dijkstra_sample(g);
    MutableArraySequence<int> prev;
    EXPECT_THROW(g.dijkstra(42, prev), vertex_not_found);
}

TEST(Dijkstra, SingleVertexDistanceIsZero) {
    G g;
    g.add_vertex(0, "a");
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(0, prev);
    ASSERT_EQ(d.GetLength(), 1u);
    EXPECT_EQ(d[0], 0);
    EXPECT_EQ(prev[0], 0);
}

TEST(Dijkstra, SampleDistances) {
    G g;
    build_dijkstra_sample(g);
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(0, prev);
    auto vs = g.get_vertices();
    ASSERT_EQ(vs.GetLength(), 4u);

    EXPECT_EQ(dist_to(d, vs, 0), 0);
    EXPECT_EQ(dist_to(d, vs, 1), 3);
    EXPECT_EQ(dist_to(d, vs, 2), 1);
    EXPECT_EQ(dist_to(d, vs, 3), 8);
}

TEST(Dijkstra, PreviousReconstructsPath) {
    G g;
    build_dijkstra_sample(g);
    MutableArraySequence<int> prev;
    g.dijkstra(0, prev);
    auto vs = g.get_vertices();

    EXPECT_EQ(prev_to(prev, vs, 1), 2);
    EXPECT_EQ(prev_to(prev, vs, 2), 0);
    EXPECT_EQ(prev_to(prev, vs, 3), 1);
}

TEST(Dijkstra, UnreachableIsInfinity) {
    G g;
    build_disconnected(g);
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(0, prev);
    auto vs = g.get_vertices();

    EXPECT_EQ(dist_to(d, vs, 0), 0);
    EXPECT_EQ(dist_to(d, vs, 1), 1);
    EXPECT_EQ(dist_to(d, vs, 2), INF);
    EXPECT_EQ(dist_to(d, vs, 3), INF);
    EXPECT_EQ(dist_to(d, vs, 4), INF);
}

TEST(Dijkstra, StartVertexItselfHasZero) {
    G g;
    build_dijkstra_sample(g);
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(2, prev);
    auto vs = g.get_vertices();
    EXPECT_EQ(dist_to(d, vs, 2), 0);
}

TEST(Generators, RandomVertexCount) {
    G g;
    gen_random(g, 100, 300);
    EXPECT_EQ(g.get_count_vertex(), 100u);
    EXPECT_GT(g.get_count_edges(), 0u);
}

TEST(Generators, RandomIsReproducibleWithSameSeed) {
    G a, b;
    gen_random(a, 50, 100, 100, 123);
    gen_random(b, 50, 100, 100, 123);
    EXPECT_EQ(a.get_count_edges(), b.get_count_edges());
    EXPECT_EQ(a.get_count_vertex(), b.get_count_vertex());
}

TEST(Generators, TreeHasCorrectStructure) {
    G g;
    gen_tree(g, 50);
    EXPECT_EQ(g.get_count_vertex(), 50u);
    EXPECT_EQ(g.get_count_edges(), 2u * 49u);
}

TEST(Generators, CompleteGraphK10) {
    G g;
    gen_complete(g, 10);
    EXPECT_EQ(g.get_count_vertex(), 10u);
    EXPECT_EQ(g.get_count_edges(), 2u * 45u);
}

TEST(Generators, CompleteGraphIsFullyConnected) {
    G g;
    gen_complete(g, 5);
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
            if (i != j) EXPECT_TRUE(g.contains_edge(i, j));
}

TEST(Generators, DagRespectsEdgeDirection) {
    G g;
    gen_dag(g, 30, 40);
    EXPECT_EQ(g.get_count_vertex(), 30u);
    EXPECT_LE(g.get_count_edges(), 40u);
    for (const auto& e : g.get_edges())
        EXPECT_LT(e.GetFirst(), e.GetSecond());
}

TEST(Generators, DijkstraOnTreeIsFinite) {
    G g;
    gen_tree(g, 100, 100, 11);
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(0, prev);
    int reachable = 0;
    for (std::size_t i = 0; i < d.GetLength(); ++i)
        if (d[i] != INF) ++reachable;
    EXPECT_EQ(reachable, 100);
}

TEST(Generators, DijkstraOnCompleteGraphIsDirectEdge) {
    G g;
    gen_complete(g, 20, 100, 5);
    MutableArraySequence<int> prev;
    auto d = g.dijkstra(0, prev);
    auto vs = g.get_vertices();
    for (std::size_t i = 0; i < vs.GetLength(); ++i) {
        if (vs[i] == 0) continue;
        EXPECT_LE(d[i], g.get_weight_edge(0, vs[i]));
    }
}