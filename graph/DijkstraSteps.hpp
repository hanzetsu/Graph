#pragma once
#include <limits>
#include "Graph.hpp"

template<typename I, typename W>
struct DijkstraStep {
    enum Kind { Select = 0, Relax = 1, Reject = 2, Done = 3 } kind;
    I current;
    I neighbor;
    W old_dist;
    W new_dist;
};

template<typename Index, typename VertexData, typename Weight,
         template<typename, typename> class Container>
MutableArraySequence<DijkstraStep<Index, Weight>> dijkstra_steps(
    const Graph<Index, VertexData, Weight, Container>& g,
    const Index& start,
    MutableArraySequence<Index>& previous,
    bool has_target, const Index& target)
{
    using Step = DijkstraStep<Index, Weight>;
    using Lim = std::numeric_limits<Weight>;

    MutableArraySequence<Step> steps;
    MutableArraySequence<Index> verts = g.get_vertices();
    if (!g.contains_vertex(start))
        throw vertex_not_found("dijkstra_steps: start not found");

    auto find_pos = [&](const Index& v) -> long {
        for (std::size_t i = 0; i < verts.GetLength(); ++i)
            if (verts[i] == v) return (long)i;
        return -1;
    };

    MutableArraySequence<Weight> dist;
    previous = MutableArraySequence<Index>();
    for (std::size_t i = 0; i < verts.GetLength(); ++i) {
        dist.Append(Lim::max());
        previous.Append(Index{});
    }
    long sp = find_pos(start);
    dist[sp] = Weight{};
    previous[sp] = start;

    MutableArraySequence<Index> unvisited = verts;
    MutableArraySequence<Index> visited;
    auto edges = g.get_edges();

    while (unvisited.GetLength() > 0) {
        Weight md = Lim::max();
        long best = -1;
        for (std::size_t i = 0; i < unvisited.GetLength(); ++i) {
            long p = find_pos(unvisited[i]);
            if (p >= 0 && dist[p] < md) { md = dist[p]; best = (long)i; }
        }
        if (best < 0 || md == Lim::max()) break;

        Index cur = unvisited[best];
        long cp = find_pos(cur);
        unvisited.RemoveAt(best);
        visited.Append(cur);
        steps.Append(Step{Step::Select, cur, cur, dist[cp], dist[cp]});

        if (has_target && cur == target) break;

        for (const auto& e : edges) {
            if (e.GetFirst() != cur) continue;
            Index nb = e.GetSecond();
            bool was_visited = false;
            for (std::size_t i = 0; i < visited.GetLength(); ++i)
                if (visited[i] == nb) { was_visited = true; break; }
            if (was_visited) continue;

            long ip = find_pos(cur), inb = find_pos(nb);
            if (ip < 0 || inb < 0) continue;
            if (dist[ip] == Lim::max()) continue;

            Weight nd = dist[ip] + g.get_weight_edge(cur, nb);
            if (nd < dist[inb]) {
                Weight old = dist[inb];
                dist[inb] = nd;
                previous[inb] = cur;
                steps.Append(Step{Step::Relax, cur, nb, old, nd});
            } else {
                steps.Append(Step{Step::Reject, cur, nb, dist[inb], nd});
            }
        }
    }
    steps.Append(Step{Step::Done, start, start, Weight{}, Weight{}});
    return steps;
}