#include "testlib.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

template <typename T, typename U = T>
typename std::enable_if<std::is_arithmetic<T>::value &&
                            std::is_arithmetic<U>::value,
                        std::vector<T>>::type
genArray(int n, U Min = 0, U Max = 1000000000, int t = 0) {

    /*
     * Generates a vector of random values.
     *
     * @return      A vector of type T containing n random elements, where T is
     * a numeric type.
     * @param n     The size of the array (number of elements to generate).
     * @param Min   The lower bound of the random values (inclusive).
     * @param Max   The upper bound of the random values (inclusive).
     * @param t     The expectation control parameter. Affects the distribution
     * of the generated values:
     *              - t > 0 will shift the expected value towards the upper
     * range.
     *              - t < 0 will shift the expected value towards the lower
     * range.
     *              - t = 0 will generate values uniformly within the given
     * range [Min, Max].
     */

    std::vector<int> arr(n);
    for (int i = 0; i < n; ++i)
        arr[i] = rnd.wnext(Min, Max, t);
    return arr;
}

template <typename T>
typename std::enable_if<
    std::is_same<T, std::vector<typename T::value_type>>::value ||
        std::is_same<T, std::string>::value,
    T>::type
sample(const T &container, int length, int num) {
    std::vector<int> vec = rnd.partition(num, length, 1);

    /*
     * Samples a specified number of elements from a container of length
     * `length`. The function selects `num` random elements from the container
     * and returns them in a new container.
     *
     * @tparam T    The container type, which must be either a `std::vector` or
     * a `std::string`.
     *
     * @param container  The input container (either a vector or string) from
     * which elements are to be sampled.
     * @param length     The length of the container, which should match the
     * actual size of `container`.
     * @param num        The number of elements to sample from the container.
     *
     * @return           A container of the same type `T` containing the `num`
     * sampled elements. The sampled elements are selected randomly and shuffled
     * in the result.
     */

    T result;
    int tmp = 0;
    for (int i = 0; i < vec.size(); ++i) {
        result.push_back(container[rnd.next(tmp, tmp + vec[i] - 1)]);
        tmp += vec[i];
    }
    shuffle(result.begin(), result.end());
    return result;
}

std::vector<std::pair<int, int>> genTree(int n, int t = 0) {
    /*
     * Generates a random tree with `n` nodes.
     * 
     * The tree structure is determined by the parameter `t`:
     * - A smaller value of `t` makes the tree more like a "bush" (i.e., many branches, less linear).
     * - A larger value of `t` makes the tree more like a "chain" (i.e., it becomes more linear with fewer branches).
     * 
     * @param n     The number of nodes in the tree.
     * @param t     The parameter that controls the tree structure:
     *              - When `t` is small, the tree is more "bushy" with more branching(菊花图).
     *              - When `t` is large, the tree is more "linear", resembling a chain.
     *              - Default value is `t = 0`.(expected height is log(n))
     * 
     * @return      A vector of pairs representing the edges of the tree, where each pair is a connection between two nodes.
     *              The edges are randomly shuffled after generation.
     */
    std::vector<int> p(n);
    for (int i = 1; i < n; i++)
        p[i] = rnd.wnext(i, t);
    std::vector<int> perm = rnd.perm(n);

    std::vector<std::pair<int, int>> edges;
    for (int i = 1; i < n; i++)
        if (rnd.next(2))
            edges.push_back(std::make_pair(perm[i], perm[p[i]]));
        else
            edges.push_back(std::make_pair(perm[p[i]], perm[i]));

    shuffle(edges.begin(), edges.end());
    return edges;
}

std::vector<std::pair<int, int>> genConnectedDirectedGraph(int n, int m, int &s,
                                                           int t = 0) {
    /*
     * Generates a connected directed graph with `n` nodes and `m` edges.
     * 
     * The graph is constructed such that:
     * - The graph has `n` nodes and `m` edges, with `m >= n - 1` to ensure connectivity.
     * - The graph is connected, meaning there's a path from the starting node `s` to all other nodes.
     * - The starting node `s` is assigned the first node in the permuted list of nodes.
     * - The edges are directed, and the structure is influenced by the parameter `t`, where smaller values of `t` 
     *   make the graph more "bushy" (many branching connections), and larger values of `t` make it more "linear" (chain-like).
     * 
     * @param n     The number of nodes in the graph.
     * @param m     The number of edges in the graph. It must be at least `n - 1` to ensure connectivity.
     * @param s     The reference to an integer which will be assigned the starting node for the graph (ensuring connectivity).
     * @param t     The parameter controlling the tree structure (see `genTree` for details).
     * 
     * @return      A vector of pairs representing the directed edges of the graph, where each pair represents a directed edge.
     *              The edges are randomly shuffled after generation.
     */
    assert(m >= n - 1);
    std::vector<int> p(n);
    for (int i = 1; i < n; i++)
        p[i] = rnd.wnext(i, t);
    std::vector<int> perm = rnd.perm(n);

    std::vector<std::pair<int, int>> edges;
    for (int i = 1; i < n; i++)
        edges.push_back(std::make_pair(perm[p[i]], perm[i]));
    s = perm[0];
    for (int i = n; i <= m; ++i) {
        std::vector<int> node = sample(perm, n, 2);
        edges.push_back(std::make_pair(node[0], node[1]));
    }
    shuffle(edges.begin(), edges.end());
    return edges;
}

std::vector<int> getLeaves(const std::vector<std::pair<int, int>> &edges) {
    /*
     * Returns the leaf nodes of a tree.
     * 
     * The leaf nodes are identified by the fact that their degree (the number of edges connected to them) is 1.
     * 
     * @param edges  A vector of pairs representing the edges of the tree. Each pair is a directed edge (u, v) where u and v are nodes.
     * 
     * @return      A vector of integers representing the leaf nodes of the tree.
     *              These are the nodes with a degree of 1, i.e., nodes that are only connected to one other node.
     */
    int n = edges.size() + 1;
    std::vector<int> deg(n);
    for (int i = 0; i < n - 1; ++i) {
        deg[edges[i].first]++;
        deg[edges[i].second]++;
    }
    std::vector<int> leaf;
    for (int i = 0; i < n; ++i) {
        if (deg[i] == 1) leaf.push_back(i);
    }
    std::cerr << "number of leaf is " << leaf.size() << '\n';
    return leaf;
}

void genTestCase() {
    // Output the contents of the `.in` file here
}

void writeTest(int test) {
    // generage testcase $test
    startTest(test);

    genTestCase();
}

int main(int argc, char *argv[]) {
    /*
     * usage:
     *  compile gen.cpp
     *  ./gen $l $r
     *  then generate $l.in to $r.in
     */
    registerGen(argc, argv, 1);
    int l = opt<int>(1);
    int r = opt<int>(2);
    assert(l <= r);
    for (int i = l; i <= r; ++i) {
        writeTest(i);
    }
}
