/*
 * POJ 1275 Cashier Employment
 * 
 * Another differential constraints problem, this one is a bit
 * harder to construct the graph than POJ 3159. To get our
 * inequalities system, we define three arrays:
 * 
 * - required[i]: the least number of cashier needed at time `i`
 * - applicants[i]: the number of applicants at time `i`
 * - sum[i]: the total number of cashier hired in past `i` time slots
 * 
 * Appearantly `sum[i] >= sum[i-1]`. Besides, since the number of
 * applicants hired at time `i` could not be more than applicants[i],
 * there is `sum[i] - sum[i-1] <= applicants[i]` too. 
 * 
 * When i<7, all cashiers hired today (sum[i]) and some who were
 * hired yesterday (sum[23] - sum[i+16]) are working, so we should
 * ensure that `sum[i] + sum[23] - sum[i+16] >= required[i]`. And
 * when i>=7, all cashiers working are hired today, so we only need
 * to ensure that `sum[i] - sum[i-8] >= required[i]`.
 * 
 * So all constraints are:
 * 
 * - sum[i] >= sum[i-1]
 * - sum[i] - sum[i-1] <= applicants[i]
 * - sum[i] + sum[23] - sum[i+16] >= required[i] (i <= 8)
 * - sum[i] - sum[i-8] >= required[i] (i > 8)
 * 
 * Transform them into standard form, and use SPFA to obtain
 * the longest path.
 * 
 */
 
#include <stdio.h>
#include <string.h>

#define MAX_N 1001
#define MAX_E 1001

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

// Use queue as memory
int queue[MAX_N];
int visited[MAX_N];
int q_head;
int q_tail;

// DP memory
int distance[MAX_N];

// SPFA algo to obtain the longest path.
int SPFA(int start, int n) {
  q_head = q_tail = 0;

  // Record how many times each vertex's distance
  // has been updated too avoid infinite loop.
  int cnt[MAX_N];

  // init the queue
  for (int i = 0; i <= n; i++) {
    if (i == start) {
      queue[q_head] = i;
      q_head = (q_head+1) % MAX_N;
      visited[i] = 1;
      distance[i] = 0;
    } else {
      visited[i] = 0;
      distance[i] = -INF;
    }
    cnt[i] = 0;
  }  

  // DP with a loop
  while (q_head != q_tail) {

    // Pick a vertex out of queue
    int from = queue[q_tail];
    q_tail = (q_tail+1) % MAX_N;
    visited[from] = 0;

    // For each edge starts from this vertex, 
    // update its distance. If a vertex's distance
    // is updated, push it back to queue.
    for (int i = head[from]; i != END; i = edges[i].next) {
      int to = edges[i].to;
      if (distance[to] < distance[from] + edges[i].weight) {
        distance[to] = distance[from] + edges[i].weight;
        if (!visited[to]) {
          visited[to] = 1;
          queue[q_head] = to;
          q_head = (q_head + 1) % MAX_N;
        }

        // Avoid the infinite loop
        if (++cnt[to] > n) return 0;
      }
    }
  }

  return 1;
}

/*
 * Related to problem
 */

int required[MAX_N];
int applicants[MAX_N];

int main() {

  int N, M;

  // There will be N testcases
  scanf("%d", &N);
  for (int i = 0; i < N; i++) {

    // At first we don't have any applicants
    memset(applicants, 0, sizeof(applicants));

    // Get inputted data
    for (int i = 1; i <= 24; i++)
      scanf("%d", &required[i]);
    scanf("%d\n", &M);
    for (int i = 0; i < M; i++) {
      int time;
      scanf("%d", &time);
      applicants[time+1]++;
    }

    // Run binary search on sum[23], try
    // each possible sum[23] until we find
    // a proper one.
    int l = 0, r = M+1, ans = INF;
    while (l <= r) {

      // mid is the expected value of sum[23]
      int mid = (l+r)/2;

      // Don't forget to clean the graph
      init_graph();

      // For constraints without limited range of i,
      // add an edge for each time slot.
      for (int i = 1; i <= 24; i++) {
        add_edge(i-1, i, 0);
        add_edge(i, i-1, -applicants[i]);
      }

      // For constraints with limited i, only add edges
      // for proper time slots.
      for (int i = 1; i <= 8; i++) add_edge(i+16, i, required[i] - mid);
      for (int i = 9; i <= 24; i++) add_edge(i-8, i, required[i]);

      // A proper sum[23] must satisfy this.
      add_edge(0, 24, mid);

      // Run SPFA, if this mid is proper, store its value
      // and shrink the range.
      if (SPFA(0, 24) && distance[24] == mid) {
        ans = mid;
        r = mid-1;
      } else l = mid+1;
    }

    // ans > M indicates that the binary search
    // failed to find a proper solution.
    if (ans <= M) printf("%d\n", ans);
    else printf("No Solution\n");
  }

  return 0;
}