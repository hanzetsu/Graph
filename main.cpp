#include "graph/Graph.hpp"
#include "graph/GraphGenerator.hpp"
#include "structs/HashTableDictionary.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

using Clock = std::chrono::high_resolution_clock;
using G = IntGraph;

void print_graph(const G& g) {
    std::cout << "Vertices (" << g.get_count_vertex() << "): ";
    for (const auto& v : g.get_vertices()) std::cout << v << " ";
    std::cout << "\nEdges (" << g.get_count_edges() << "):\n";
    for (const auto& e : g.get_edges())
        std::cout << "  " << e.GetFirst() << " -> " << e.GetSecond()
                  << " w=" << g.get_weight_edge(e.GetFirst(), e.GetSecond()) << "\n";
}

void run_dijkstra(G& g) {
    if (g.get_count_vertex() == 0) { std::cout << "Graph empty\n"; return; }
    int s;
    std::cout << "Start vertex: "; std::cin >> s;
    MutableArraySequence<int> prev;
    auto t0 = Clock::now();
    auto d = g.dijkstra(s, prev);
    auto t1 = Clock::now();
    auto vs = g.get_vertices();
    for (std::size_t i = 0; i < vs.GetLength(); ++i) {
        std::cout << "  " << s << " -> " << vs[i] << " = ";
        if (d[i] == std::numeric_limits<int>::max()) std::cout << "INF\n";
        else std::cout << d[i] << "\n";
    }
    std::cout << "Time: " << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms\n";
}

void run_benchmark() {
    std::ofstream csv("results.csv");
    csv << "n,edges,dijkstra_ms\n";
    std::cout << std::fixed << std::setprecision(3);
    for (int n : {100, 200, 500, 1000}) {
        G g;
        gen_random(g, n, n * 2, 100, 7);
        MutableArraySequence<int> p;
        auto t0 = Clock::now();
        g.dijkstra(0, p);
        auto t1 = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::cout << "n=" << n << "  E=" << g.get_count_edges() << "  " << ms << " ms\n";
        csv << n << "," << g.get_count_edges() << "," << ms << "\n";
    }
    csv.close();
    std::cout << "Saved: results.csv\n";
}

int main() {
    G g;
    while (true) {
        std::cout << "\n=== Lab 3 ===\n"
                  << "1. Demo graph\n2. Generate random\n3. Generate tree\n"
                  << "4. Generate complete\n5. Generate DAG\n"
                  << "6. Show graph\n7. Dijkstra\n8. Benchmark->CSV\n0. Exit\n> ";
        int c; if (!(std::cin >> c)) break;
        try {
            switch (c) {
                case 1:
                    g.clear();
                    for (int i = 0; i < 5; ++i) g.add_vertex(i, "v" + std::to_string(i));
                    g.add_undirected_edge(0, 1, 4);
                    g.add_undirected_edge(0, 2, 1);
                    g.add_undirected_edge(2, 1, 2);
                    g.add_undirected_edge(1, 3, 5);
                    g.add_undirected_edge(2, 3, 8);
                    g.add_undirected_edge(3, 4, 3);
                    std::cout << "Loaded demo\n"; break;
                case 2: { int n, m; std::cout << "n m: "; std::cin >> n >> m; gen_random(g, n, m); break; }
                case 3: { int n; std::cout << "n: "; std::cin >> n; gen_tree(g, n); break; }
                case 4: { int n; std::cout << "n: "; std::cin >> n; gen_complete(g, n); break; }
                case 5: { int n, m; std::cout << "n m: "; std::cin >> n >> m; gen_dag(g, n, m); break; }
                case 6: print_graph(g); break;
                case 7: run_dijkstra(g); break;
                case 8: run_benchmark(); break;
                case 0: return 0;
                default: std::cout << "?\n";
            }
        } catch (const std::exception& e) { std::cout << "Error: " << e.what() << "\n"; }
    }
}