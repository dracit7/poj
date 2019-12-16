/*
 * POJ 1470 Closest Common Ancestors
 * 
 * A simple application of Tarjian algorithm.
 * 
 * If you are already famaliar with tarjian algo, the
 * only obstacle would be the inputting format, which
 * is kind of disgusting as several random blanks may
 * be added between words.
 * 
 * Fortunately, scanf() is powerful, and it would be
 * much easier to deal with the awful input if we make
 * use of it.
 * 
 */


#include <cstdio>
#include <vector>
#include <cstring>
using std::vector;

#define MAX_N 1000

/*
 * Global variables
 */

// Store the structure of the tree.
// children[i]: all children of node `i`.
vector<int> children[MAX_N];
// Store the queries raised by input.
vector<int> queries[MAX_N];

// Parent of `i` in its uf-set.
int parent[MAX_N];
// If we have visited this node in DFS.
bool visited[MAX_N];

// The number of pair for which i is
// an ancestor.
int cnt[MAX_N];

// Use a mark array to find the root
// of tree.
bool is_child[MAX_N];

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
  parent[find(b)] = find(a);
}

/*
 * Tarjian algo
 */

void tarjian(int x) {

  // Assign a new uf-set to this node
  parent[x] = x;

  // For each child of x, tarjian it and merge it into
  // the uf-set of x.
  for (int i = 0; i < children[x].size(); i++) {
    tarjian(children[x][i]);
    merge(x, children[x][i]);
  }

  // As tarjian algo uses DFS, we visit x after
  // the recursive call.
  visited[x] = true;

  // Respond to queries
  for (int i = 0; i < queries[x].size(); i++) {
    if (!visited[queries[x][i]]) continue;
    cnt[find(queries[x][i])]++;
  }
}

int main() {

  // Number of verticles
  int n;
  // Number of queries
  int num_queries;

  while (scanf("%d", &n) == 1) {

    // Clear the memory of global variables
    memset(parent, 0, sizeof(parent));
    memset(is_child, 0, sizeof(is_child));
    memset(visited, 0, sizeof(visited));
    memset(cnt, 0, sizeof(cnt));
    for (int i = 0; i < MAX_N; i++) {
      children[i].clear();
      queries[i].clear();
    }

    // Read the structure of tree from input
    for (int i = 0; i < n; i++) {
      int node, num_child, child;
      scanf("%d:(%d)", &node, &num_child);

      // Record each child, and mark them as child.
      // After the loop, the not marked node is root.
      for (int j = 1; j <= num_child; j++) {
        scanf("%d", &child);
        children[node].push_back(child);
        is_child[child] = true;
      }
    }

    // Read all queries
    scanf("%d", &num_queries);
    for (int i = 0; i < num_queries; i++) {

      // As the question prompted, the input may be
      // divided by several spaces, so we add a space
      // in the front of formatting string.
      int u, v;
      scanf(" (%d %d)", &u, &v);
      queries[u].push_back(v);
      queries[v].push_back(u);
    }

    // Run tarjian algo on the root node
    for (int i = 1; i <= n; i++) {
      if (!is_child[i]) tarjian(i);
    }

    // Print the answer
    for (int i = 1; i <= n; i++) {
      if (cnt[i]) printf("%d:%d\n", i, cnt[i]);
    }
  }

  return 0;
}