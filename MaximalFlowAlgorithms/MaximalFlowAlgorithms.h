#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <algorithm>
#include <cassert>


const int INF = 1e7;


struct Edge {
    int st, fin, cap, flow, reverseNumber;
};


class Network {
protected:
    const int source_;
    const int target_;
    std::vector<std::vector<Edge>> graph_;

public:
    Network(const int& size, const int& source, const int& target)
        : source_(source), target_(target), graph_(std::vector<std::vector<Edge>>(size)) {}

    Network(const Network&) = default;

    ~Network() {}

    std::size_t verticesNumber() const {
        return graph_.size();
    }

    void addEdge(const int& v, const int& to, const int& cap) {
        assert(v < graph_.size() && to < graph_.size() && cap >= 0);
        graph_[v].push_back(Edge{v, to, cap, 0, (int)graph_[to].size()});
        graph_[to].push_back(Edge{to, v, 0, 0, (int)graph_[v].size() - 1});
    }

    std::vector<Edge>& getIncidentEdges(const int& v) {
        assert(v < graph_.size());
        return graph_[v];
    }

    std::size_t getIncidentEdgesNumber(const int& v) const {
        return graph_[v].size();
    }

    void pushFlow(Edge& e, const int& flow) {
        assert(e.cap - e.flow >= flow);
        e.flow += flow;
        graph_[e.fin][e.reverseNumber].flow -= flow;
    }

    int getSource() const {
        return source_;
    }

    int getTarget() const {
        return target_;
    }

    Edge& getForwardEdge(const Edge& backEdge) {
        return graph_[backEdge.fin][backEdge.reverseNumber];
    }
};


class Dinic {
private:
    Network network_;
    std::vector<int> d_, lastNotUsed_;
    int maximalFlow_;

    bool bfs() {
        std::queue<int> q;
        d_.assign(network_.verticesNumber(), INF);
        d_[network_.getSource()] = 0;

        q.push(network_.getSource());

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (Edge& e : network_.getIncidentEdges(v))
                if (e.flow < e.cap && d_[e.fin] == INF) {
                    d_[e.fin] = d_[v] + 1;
                    q.push(e.fin);
                }
        }

        return d_[network_.getTarget()] < INF;
    }

    int dfs(const int& v, const int& minCapacity) {
        if (v == network_.getTarget() || !minCapacity)
            return minCapacity;

        for (; lastNotUsed_[v] < network_.getIncidentEdgesNumber(v); ++lastNotUsed_[v]) {
            Edge& e = network_.getIncidentEdges(v)[lastNotUsed_[v]];

            if (d_[e.fin] == d_[v] + 1) {
                int delta = dfs(e.fin, std::min(minCapacity, e.cap - e.flow));
                if (delta) {
                    network_.pushFlow(e, delta);
                    return delta;
                }
            }
        }
        return 0;
    }

public:
    Dinic(const Network& network) : network_(network) {}

    ~Dinic() {}

    int run() {
        maximalFlow_ = 0;
        while (bfs()) {
            lastNotUsed_.assign(network_.verticesNumber(), 0);
            while (int blockingFlow = dfs(network_.getSource(), INF))
                maximalFlow_ += blockingFlow;
        }
        return maximalFlow_;
    }

    int getMaximalFlow() const {
        return maximalFlow_;
    }

    Network getNetwork() const {
        return network_;
    }
};


class MalhotraKumarMaheshwari {
private:
    Network network_;
    std::vector<int> d_, p_, pin_, pout_, fixed_;
    int maximalFlow_;

    bool bfs() {
        std::queue<int> q;
        d_.assign(network_.verticesNumber(), INF);
        d_[network_.getSource()] = 0;

        q.push(network_.getSource());

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (Edge& e : network_.getIncidentEdges(v))
                if (e.flow < e.cap && d_[e.fin] == INF) {
                    d_[e.fin] = d_[v] + 1;
                    q.push(e.fin);
                }
        }

        return d_[network_.getTarget()] < INF;
    }

    bool isPathFromSourceToTarget() {
        std::vector<int> used(network_.verticesNumber(), 0);
        std::queue<int> q;
        q.push(network_.getSource());

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            if (v == network_.getTarget())
                return true;

            used[v] = 1;

            for (Edge& e : network_.getIncidentEdges(v))
                if (e.flow < e.cap && d_[e.fin] == d_[v] + 1 && !used[e.fin])
                    q.push(e.fin);
        }

        return false;
    }

    void updatePotential(const int& v) {
        p_[v] = (v == network_.getSource() ? pout_[v] : (v == network_.getTarget() ? pin_[v] : std::min(pin_[v], pout_[v])));
    }

    void dfs(const int& v, const int& toPush, const bool& forward) {
        int toPushMore = toPush;
        if ((v == network_.getTarget() && forward) || (v == network_.getSource() && !forward) || !toPush)
            return;
        for (Edge& e : network_.getIncidentEdges(v))
            if (forward && d_[e.fin] == d_[v] + 1 && e.cap - e.flow > 0 && p_[e.fin]) {
                int delta = std::min(e.cap - e.flow, toPushMore);

                network_.pushFlow(e, delta);
                pout_[v] -= delta;
                pin_[e.fin] -= delta;

                updatePotential(v);
                updatePotential(e.fin);

                dfs(e.fin, delta, forward);

                toPushMore -= delta;
            }
            else if (!forward && d_[e.fin] == d_[v] - 1 && p_[e.fin]) {
                Edge& e1 = network_.getForwardEdge(e);
                if (e1.cap - e1.flow > 0) {
                    int delta = std::min(e1.cap - e1.flow, toPushMore);

                    network_.pushFlow(e1, delta);
                    pin_[v] -= delta;
                    pout_[e1.st] -= delta;

                    updatePotential(v);
                    updatePotential(e1.st);

                    dfs(e1.st, delta, forward);

                    toPushMore -= delta;
                }
            }
    }

    void fixPotentials(const int& v) {
        if (fixed_[v])
            return;
        fixed_[v] = 1;
        for (Edge& e : network_.getIncidentEdges(v))
            if (d_[e.fin] == d_[v] + 1 && !fixed_[e.fin]) {
                pin_[e.fin] -= (e.cap - e.flow);
                updatePotential(e.fin);

                if (p_[e.fin] == 0)
                    fixPotentials(e.fin);
            }
            else if (d_[e.fin] + 1 == d_[v] && !fixed_[e.fin]) {
                Edge& e1 = network_.getForwardEdge(e);
                pout_[e1.st] -= (e1.cap - e1.flow);
                updatePotential(e1.st);

                if (p_[e1.st] == 0)
                    fixPotentials(e1.st);
            }
    }

    void countPotentials() {
        pout_.assign(network_.verticesNumber(), 0);
        pin_.assign(network_.verticesNumber(), 0);
        p_.assign(network_.verticesNumber(), 0);

        for (int i = 0; i < network_.verticesNumber(); ++i)
            for (Edge& e : network_.getIncidentEdges(i))
                if (d_[e.fin] == d_[i] + 1) {
                    pout_[i] += e.cap - e.flow;
                    pin_[e.fin] += e.cap - e.flow;
                }

        for (int i = 0; i < network_.verticesNumber(); ++i)
            updatePotential(i);
    }

public:
    MalhotraKumarMaheshwari(const Network& network) : network_(network) {}

    ~MalhotraKumarMaheshwari() {}

    Network getNetwork() const {
        return network_;
    }

    int getMaximalFlow() const {
        return maximalFlow_;
    }

    int run() {
        int maximalFlow_ = 0;
        while (bfs()) {
            fixed_.assign(network_.verticesNumber(), 0);
            countPotentials();

            std::list<int> vertices;
            for (int i = 0; i < network_.verticesNumber(); ++i)
                vertices.push_back(i);

            while (!vertices.empty() && isPathFromSourceToTarget()) {
                auto it = std::min_element(vertices.begin(), vertices.end(), [&p = p_](const int& a, const int& b) {
                                                                                    return p[a] < p[b];
                                                                                });
            
                int tmp = p_[*it];
                maximalFlow_ += tmp;
                dfs(*it, tmp, true);
                dfs(*it, tmp, false);
                fixPotentials(*it);
                vertices.erase(it);
            }
        }
        return maximalFlow_;
    }
};


class PreFlowPush {
private:
    Network network_;
    std::vector<int> extra_, h_, maxh_;
    int maxhSize_;
    int maximalFlow_;

    void push(Edge& e) {
        int delta = std::min(extra_[e.st], e.cap - e.flow);
        network_.pushFlow(e, delta);
        extra_[e.st] -= delta;
        extra_[e.fin] += delta;
    }

    void relable(const int& v) {
        int d = INF;

        for (Edge& e : network_.getIncidentEdges(v)) 
            if (e.cap - e.flow > 0)
                d = std::min(d, h_[e.fin]);

        if (d == INF)
            return;

        h_[v] = d + 1;
    }

public:
    PreFlowPush(const Network& network) : network_(network) {}

    ~PreFlowPush() {}

    int run() {
        extra_.assign(network_.verticesNumber(), 0);
        h_.assign(network_.verticesNumber(), 0);
        maxh_.assign(network_.verticesNumber(), 0);

        maxhSize_ = 0;
        h_[network_.getSource()] = network_.verticesNumber();

        for (Edge& e : network_.getIncidentEdges(network_.getSource()))
            if (e.cap) {
                network_.pushFlow(e, e.cap);
                extra_[e.fin] = e.flow;
            }

        while (true) {
            if (!maxhSize_) 
                for (int i = 0; i < network_.verticesNumber(); ++i)
                    if (i == network_.getSource() || i == network_.getTarget())
                        continue; 
                    else if (extra_[i] > 0) {
                        if (maxhSize_ && h_[i] > h_[maxh_[network_.getSource()]])
                            maxhSize_ = 0;
                        if (!maxhSize_ || h_[i] == h_[maxh_[network_.getSource()]])
                            maxh_[maxhSize_++] = i;
                    }
            
            if (!maxhSize_)
                break;
            
            while (maxhSize_) {
                int i = maxh_[maxhSize_ - 1];
                bool pushed = false;
                
                for (Edge& e : network_.getIncidentEdges(i)) {
                    if (!extra_[i])
                        break;
                    if (e.cap - e.flow > 0 && h_[i] == h_[e.fin] + 1) {
                        push(e);
                        if (!extra_[i])
                            --maxhSize_;
                        pushed = true;
                        break;
                    }
                }

                if (!pushed)
                    relable(i);
            }
        }

        maximalFlow_ = 0;

        for (Edge& e : network_.getIncidentEdges(network_.getSource()))
            if (e.cap)
                maximalFlow_ += e.flow;

        return std::max(maximalFlow_, 0);
    }

    int getMaximalFlow() const {
        return maximalFlow_;
    }

    Network getNetwork() const {
        return network_;
    }
};
