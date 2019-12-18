/*
 * POJ 3159 Candies
 * 
 * This problem seems to be a typical application of differential
 * constraints, at least the modeling period is trivial. According
 * to [wls](https://github.com/Alisahhh), SPFA sucks and has a fake
 * complexity, so she recommended Dijkstra & heap instead. That does
 * make sense as SPFA with queue would T in this problem, but I don't
 * like cxx and do not want to use std::priority_queue, so I decided
 * to pick SPFA with stack here.
 * 
 */
 
#include <stdio.h>
#include <string.h>

#define MAX_N 30001
#define MAX_E 150001

/*
 * Graph APIs
 * 
 * This implementation differs from adjacent table/matrix,
 * and I failed to find a English name for it. In Chinese
 * ACM/OI terminologies, it's called "链式前向星".
 * 
 */

#define END -1

// The vertex array.
int head[MAX_N];

// The edge array, a linked list is maintained by this array.
// 
// Each vertex in head[] points to an edge list in edges[],
// which is linked by `next` attribute.
struct {
  int to; // the destination of this edge
  int weight; // the weight of this edge
  int next; // next edge in the linked list
} edges[MAX_E];
// Current number of edges
int num_edges;

// Add an edge to the graph
void add_edge(int from, int to, int weight) {

  // Add to the head of related linked list
  edges[num_edges].next = head[from];
  edges[num_edges].to = to;
  edges[num_edges].weight = weight;

  head[from] = num_edges++;
}

// Clear the graph
void init_graph() {
  num_edges = 0;
  memset(head, END, sizeof(head));
  memset(edges, 0, sizeof(edges));
}

/*
 * SPFA APIs
 */

#define INF 0x3f3f3f3f

// Use stack instead of queue here
int stack[MAX_N];
int visited[MAX_N];
int stack_top = 0;

// DP memory
int distance[MAX_N];

// SPFA algo.
void SPFA(int start, int n) {
  stack_top = 0;

  // init the stack
  for (int i = 1; i <= n; i++) {
    if (i == start) {
      stack[stack_top++] = i;
      visited[i] = 1;
      distance[i] = 0;
    } else {
      visited[i] = 0;
      distance[i] = INF;
    }
  }  

  // DP with a loop
  while (stack_top) {

    // Pick a vertex out of stack
    int from = stack[--stack_top];
    visited[from] = 0;

    // For each edge starts from this vertex, 
    // update its distance. If a vertex's distance
    // is updated, push it back to stack.
    for (int i = head[from]; i != END; i = edges[i].next) {
      int to = edges[i].to;
      if (distance[to] > distance[from] + edges[i].weight) {
        distance[to] = distance[from] + edges[i].weight;
        if (!visited[to]) {
          visited[to] = 1;
          stack[stack_top++] = to;
        }
      }
    }
  }
}

int main() {

  // Get the size of input
  int N, M;
  scanf("%d%d", &N, &M);

  // Init the graph
  init_graph();
  
  // Get all constraints and construct a graph out of them
  for (int i = 0; i < M; i++) {
    int A, B, c;
    scanf("%d%d%d", &A, &B, &c);

    // The problem requires B-A <= c, which is equivalent
    // to B <= A + c, so c should be the weight.
    add_edge(A, B, c);
  }

  // Use SPFA to solve this problem.
  SPFA(1, N);
  printf("%d\n", distance[N]);

  return 0;
}