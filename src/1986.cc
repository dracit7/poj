/*
 * POJ 1986 Distance Queries
 * 
 * Like POJ 1470, this is another application of tarjian
 * algorithm. Logically this is not a hard problem, a bit
 * modification on the solution to POJ 1470 is enough.
 * 
 * However, the time limit of this one is exceedingly tight, so
 * it is important to choose the data structure you use wisely.
 * At first I pushed all queries into a single vector and traverse
 * it each time I want to find a query, thus got a T on POJ.
 * 
 * Time Limit Exceeded is more common in this problem. If you
 * encountered one, check the size of your arrays first. If the
 * size of your arrays are large enough, redesign your algorithm
 * or data structure to reduce the time complexity.
 * 
 */

#include <cstdio>
#include <vector>
#include <cstring>
using std::vector;

#define MAX_NUM 40001
#define MAX_LEN 1001
#define MAX_QUERIES 10001

// Use a structure to record the
// weight of edges
typedef struct {
  int id;
  int weight;
} node;

// Queries
typedef struct {
  int id; // Helps record the order of answers
  int from;
  int to;
} query;

/*
 * Global variables
 */

// Store the structure of the tree.
// children[i]: all children of node `i`.
vector<node> children[MAX_NUM];
// Store the queries raised by input.
vector<query> queries[MAX_NUM];

// Parent of `i` in its uf-set.
int parent[MAX_NUM];
// If we have visited this node in DFS.
bool visited[MAX_NUM];

// The distance between root and this node.
int distance[MAX_NUM];

int answers[MAX_NUM];

/*
 * uf-set APIs
 */

// We do not need find_update() here
// because no extra operation is needed.
//
// Note that we applied a path compression
// with parent[x] = find(parent[x]).
int find(int x) {
  return (x == parent[x]) ? x : (parent[x] = find(parent[x]));
}

// merge two uf-sets `a` and `b`.
// Attention that we have put find() into
// this function.
void merge(int a, int b) {
  parent[b] = a;
}

/*
 * Tarjian algo
 */

void tarjian(int x) {

  // Assign a new uf-set to this node
  parent[x] = x;

  // This time we're facing a graph with cycle instead
  // of tree, so put this ahead of the recursive call.
  visited[x] = true;

  // For each child of x, tarjian it and merge it into
  // the uf-set of x.
  // We also calculate the distance to this child here.
  for (int i = 0; i < children[x].size(); i++) {

    // Only visit unvisited nodes to avoid infinite call.
    if (!visited[children[x][i].id]) {
      distance[children[x][i].id] = distance[x] + children[x][i].weight;
      tarjian(children[x][i].id);
      merge(x, children[x][i].id);
    }
  }

  // Respond to queries
  for (int i = 0; i < queries[x].size(); i++) {
    if (!visited[queries[x][i].to]) continue;

    // Dist(u，v) = Dist(1，u) + Dist(1，v) - 2*Dist(1，LCA(u，v))
    //
    // Synchronize the answer between i->j and j->i as
    // queries[x][i] == queries[x][i^1].
    answers[queries[x][i].id] = 
      distance[x] + distance[queries[x][i].to] - 2*distance[find(queries[x][i].to)];
  }
}

int main() {

  int num_farms;
  int num_roads;
  int num_queries;

  while (scanf("%d%d", &num_farms, &num_roads) == 2) {

    // Clear the memory of global variables
    memset(parent, 0, sizeof(parent));
    memset(visited, 0, sizeof(visited));
    memset(distance, 0, sizeof(distance));
    memset(queries, 0, sizeof(queries));
    for (int i = 0; i < MAX_NUM; i++) {
      children[i].clear();
    }

    // Read the structure of graph from input
    for (int i = 0; i < num_roads; i++) {
      int start, end, length, direction;
      scanf("%d%d%d %c", &start, &end, &length, &direction);

      // Record each child.
      // The road is directless, so we record
      // a edge for both directions.
      //
      // (Puny POJ doesn't even support struct literal,
      // I have to use this dumb way to initialize them)
      node tmp;
      tmp.id = end, tmp.weight = length;
      children[start].push_back(tmp);
      tmp.id = start, tmp.weight = length;
      children[end].push_back(tmp);
    }

    // Read all queries
    scanf("%d", &num_queries);
    for (int i = 0; i < num_queries; i++) {

      // As the question prompted, the input may be
      // divided by several spaces, so we add a space
      // in the front of formatting string.
      int u, v;
      scanf("%d%d", &u, &v);

      // For each query, we need to add two symmetric
      // queries, because we don't know which node would
      // be visited first. Once one of two nodes is
      // visited, the answer would be synchronized between
      // symmetric queries.
      query tmp;
      tmp.from = u, tmp.to = v, tmp.id = i;
      queries[u].push_back(tmp);
      tmp.from = v, tmp.to = u, tmp.id = i;
      queries[v].push_back(tmp);
    }

    // Run tarjian algo on the root node
    tarjian(1);

    // Print the answer.
    // Since adjacent queries are symmetric, we only need
    // to print the answer of one of them.
    for (int i = 0; i < num_queries; i++) {
      printf("%d\n", answers[i]);
    }
  }

  return 0;
}