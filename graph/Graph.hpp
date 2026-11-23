#pragma once
#include <limits>
#include <type_traits>
#include <cstddef>
#include "../structs/MutableArraySequence.hpp"
#include "../structs/IDictionary.hpp"
#include "../libs/Option.hpp"
#include "../libs/Pair.hpp"
#include "graph_exceptions.hpp"

template<typename Index, typename VertexData, typename Weight,
         template<typename, typename> class Container>
class Graph {
public:
    using EdgeKey = Pair<Index, Index>;

private:
    Container<Index, VertexData> vertexs_;
    Container<EdgeKey, Weight> edges_;

    Option<std::size_t> find_position(const Index& v, const MutableArraySequence<Index>& s) const {
        for (std::size_t i = 0; i < s.GetLength(); ++i)
            if (s[i] == v) return Option<std::size_t>(i);
        return Option<std::size_t>();
    }

    void relax(const Index& cur, const Index& nb, MutableArraySequence<Weight>& dist,
               const MutableArraySequence<Index>& verts, MutableArraySequence<Index>& prev) {
        auto pu = find_position(cur, verts);
        auto pv = find_position(nb, verts);
        if (!pu.HasValue() || !pv.HasValue()) return;
        std::size_t iu = pu.GetValue(), iv = pv.GetValue();
        if (dist[iu] == std::numeric_limits<Weight>::max()) return;
        Weight nd = dist[iu] + get_weight_edge(cur, nb);
        if (nd < dist[iv]) { dist[iv] = nd; prev[iv] = cur; }
    }

    Option<Index> find_min(const MutableArraySequence<Index>& unvisited,
                           const MutableArraySequence<Weight>& dist,
                           const MutableArraySequence<Index>& verts) const {
        if (unvisited.GetLength() == 0) return Option<Index>();
        Weight md = std::numeric_limits<Weight>::max();
        Index mv = unvisited[0];
        bool found = false;
        for (const auto& v : unvisited) {
            auto p = find_position(v, verts);
            if (!p.HasValue()) continue;
            Weight d = dist[p.GetValue()];
            if (d < md) { md = d; mv = v; found = true; }
        }
        return found ? Option<Index>(mv) : Option<Index>();
    }

    bool contains_vertex(const MutableArraySequence<Index>& s, const Index& v) const {
        for (const auto& x : s) if (x == v) return true;
        return false;
    }
    void remove_from_seq(MutableArraySequence<Index>& s, const Index& v) {
        auto p = find_position(v, s);
        if (p.HasValue()) s.RemoveAt(p.GetValue());
    }

public:
    void add_vertex(const Index& i, const VertexData& d) {
        if (!vertexs_.contains_key(i)) vertexs_.add(i, d);
    }
    void add_edge(const Index& from, const Index& to, const Weight& w) {
        if (!vertexs_.contains_key(from) || !vertexs_.contains_key(to)) return;
        EdgeKey k(from, to);
        if (!edges_.contains_key(k)) edges_.add(k, w);
    }
    void add_undirected_edge(const Index& a, const Index& b, const Weight& w) {
        add_edge(a, b, w);
        add_edge(b, a, w);
    }
    MutableArraySequence<Index> get_vertices() const { return vertexs_.get_keys(); }
    MutableArraySequence<EdgeKey> get_edges() const { return edges_.get_keys(); }
    bool contains_vertex(const Index& i) const { return vertexs_.contains_key(i); }
    bool contains_edge(const Index& a, const Index& b) const { return edges_.contains_key(EdgeKey(a, b)); }
    VertexData get_vertex_data(const Index& i) const { return vertexs_.get_elem_by_key(i); }
    Weight get_weight_edge(const Index& a, const Index& b) const {
        return edges_.get_elem_by_key(EdgeKey(a, b));
    }
    unsigned int get_count_vertex() const { return vertexs_.get_count(); }
    unsigned int get_count_edges() const { return edges_.get_count(); }

    void clear() {
        auto vs = vertexs_.get_keys();
        for (const auto& v : vs) {
            auto es = edges_.get_keys();
            for (const auto& e : es)
                if (e.GetFirst() == v || e.GetSecond() == v) edges_.remove_by_key(e);
            vertexs_.remove_by_key(v);
        }
    }

    MutableArraySequence<Weight> dijkstra(const Index& start,
                                          MutableArraySequence<Index>& previous) {
        static_assert(std::is_arithmetic<Weight>::value,
                      "Dijkstra requires numeric Weight");
        auto verts = vertexs_.get_keys();
        if (!vertexs_.contains_key(start))
            throw vertex_not_found("dijkstra: start vertex not found");

        MutableArraySequence<Weight> dist;
        previous = MutableArraySequence<Index>();
        const Weight INF = std::numeric_limits<Weight>::max();
        for (std::size_t i = 0; i < verts.GetLength(); ++i) {
            dist.Append(INF);
            previous.Append(Index{});
        }
        std::size_t sp = find_position(start, verts).GetValue();
        dist[sp] = Weight{};
        previous[sp] = start;

        MutableArraySequence<Index> unvisited = verts;
        MutableArraySequence<Index> visited;
        auto edges = edges_.get_keys();

        while (unvisited.GetLength() > 0) {
            auto cur = find_min(unvisited, dist, verts);
            if (!cur.HasValue()) break;
            Index c = cur.GetValue();
            remove_from_seq(unvisited, c);
            visited.Append(c);
            for (const auto& e : edges) {
                if (e.GetFirst() != c) continue;
                Index nb = e.GetSecond();
                if (contains_vertex(visited, nb)) continue;
                relax(c, nb, dist, verts, previous);
            }
        }
        return dist;
    }
};