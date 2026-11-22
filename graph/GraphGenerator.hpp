#pragma once
#include "Graph.hpp"
#include "../structs/HashTableDictionary.hpp"
#include <random>
#include <string>

using IntGraph = Graph<int, std::string, int, HashTableDictionary>;

inline void fill_vertices(IntGraph& g, int n) {
    for (int i = 0; i < n; ++i) g.add_vertex(i, "v" + std::to_string(i));
}

inline void gen_random(IntGraph& g, int n, int m, int wmax = 100, unsigned seed = 42) {
    g.clear();
    fill_vertices(g, n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> vd(0, n - 1), wd(1, wmax);
    for (int i = 0; i < m; ++i) {
        int a = vd(rng), b = vd(rng);
        if (a == b) { --i; continue; }
        g.add_undirected_edge(a, b, wd(rng));
    }
}

inline void gen_tree(IntGraph& g, int n, int wmax = 100, unsigned seed = 42) {
    g.clear();
    fill_vertices(g, n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> wd(1, wmax);
    for (int i = 1; i < n; ++i) {
        std::uniform_int_distribution<int> pd(0, i - 1);
        g.add_undirected_edge(pd(rng), i, wd(rng));
    }
}

inline void gen_complete(IntGraph& g, int n, int wmax = 100, unsigned seed = 42) {
    g.clear();
    fill_vertices(g, n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> wd(1, wmax);
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            g.add_undirected_edge(i, j, wd(rng));
}

inline void gen_dag(IntGraph& g, int n, int m, int wmax = 100, unsigned seed = 42) {
    g.clear();
    fill_vertices(g, n);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> wd(1, wmax);
    int added = 0, attempts = 0;
    while (added < m && attempts < m * 20) {
        std::uniform_int_distribution<int> vd(0, n - 1);
        int a = vd(rng), b = vd(rng);
        ++attempts;
        if (a >= b) continue;
        if (g.contains_edge(a, b)) continue;
        g.add_edge(a, b, wd(rng));
        ++added;
    }
}