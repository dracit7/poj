/*
 * POJ 1417 True Liars
 * 
 * It could be derived that each statement determines a certain
 * relationship between two people:
 * 
 * - a says that b is divine: a & b are in the same state (devine or not)
 * - a says that b is not divine: a & b are in different states
 * 
 * So it's possible to store each group of people whose relationship
 * could be determined into an uf-set, and use an extra array to mark
 * the relationship between `x` and its root in uf-set, just like we
 * did in POJ 1182.
 * 
 * After partitioning people into several uf-sets, we now have 2*`n`
 * groups of people (`n` is the number of uf-sets): for each uf-set,
 * we could divide it to 2 groups of people considering divinity, but
 * we do *not* know which group is divine and which group is not.
 * 
 * uf-set                     :   1    2    3    4  ...  n
 * number of people in group 0:  a_1  a_2  a_3  a_4 ... a_n
 * number of people in group 1:  b_1  b_2  b_3  b_4 ... b_n
 * 
 * Then the problem is about if we could come up with an *unique* plan
 * to pick a group in each uf-set, making the number of people in total
 * equal to `p1`. e.g. a_1 + b_2 + b_3 + .... + a_n = p1.
 * 
 * Bruteforce would be an exponential algo here, so we use DP to
 * reduce the complexity to polynomial. It works like a knapsack problem.
 *   
 */

#include <stdio.h>
#include <string.h>

#define MAX_N 1000
#define MAX_PEOPLE 300

// Possible relations
enum relation {
  SAME = 0, // same group
  DIFF, // different group
};

/*
 * We put all objects with certain relationship in an uf-set,
 * so there will be an uf-set for each group.
 */

// parent[i]: the parent node of `i` in the uf-set
int parent[MAX_N];
// relation_with_root[i]: `i`'s relation with its root
int relation_with_root[MAX_N];

// The number of discerned groups
int uf_set_num = 0;
// uf_sets[i][SAME|DIFF] represents the number
// of two groups of people in the `i`th uf-set.
int uf_sets[MAX_N][2];
// uf_set_in[i] is the serial of uf-set which `i` locates.
int uf_set_in[MAX_N];

// Number of possible solutions,
// given the number of uf-sets considered
// and the number of p1.
int solution_num[MAX_N][MAX_PEOPLE];

/*
 * uf-set APIs
 */

// find the root of `x` and update all
// relations_with_root in this subtree.
int find_update(int x) {
  if (x == parent[x])
    return x;

  // I have explained the principle of this step
  // in comments of `1182.c`, so no repeat here.
  int px = parent[x];
  parent[x] = find_update(parent[x]);
  relation_with_root[x] = (relation_with_root[x]+relation_with_root[px]) % 2;

  return parent[x];
}

// merge two uf-sets `a` and `b`
void merge(int a, int b) {
  parent[b] = a;
}

int main() {

  int n, p1, p2;

  // Start to handle tasks
  while (1) {
    scanf("%d%d%d", &n, &p1, &p2);
    if (!n && !p1 && !p2)
      return 0;

    // Initialize all global data structures
    for (int i = 1; i < MAX_N; i++) {
      parent[i] = i;
      relation_with_root[i] = SAME;
    }
    memset(uf_set_in, 0, sizeof(uf_set_in));
    memset(uf_sets, 0, sizeof(uf_sets));
    memset(solution_num, 0, sizeof(solution_num));
    uf_set_num = 0;

    // Read all statements and build uf-sets
    for (int i = 1; i <= n; i++) {
      int xi, yi, root_xi, root_yi, same_group;
      char ai[4];
      scanf("%d%d%s", &xi, &yi, ai);

      // Find the uf-sets containing xi and yi
      root_xi = find_update(xi);
      root_yi = find_update(yi);

      // If a said b is divine, a and b belong to the same group.
      // Elsewise, a and b belong to different groups.
      same_group = (ai[0] == 'y');

      // Merge two uf-sets and update part of the relations
      // This part is described in `1182.c`.
      if (root_xi != root_yi) {
        merge(root_xi, root_yi);
        relation_with_root[root_yi] = (relation_with_root[xi] + relation_with_root[yi] + same_group + 1) % 2;
      }
    }

    // Record the relations into `uf_sets`,
    // make it more convenient to apply DP.
    for (int i = 1; i <= p1+p2; i++) {

      // If this uf_set has not been recorded,
      // assign a serial to it.
      int root_i = find_update(i);
      if (uf_set_in[root_i] == 0)
        uf_set_in[root_i] = ++uf_set_num;

      // Increase the number of this kind of people
      // in the uf-set it locates.
      uf_sets[uf_set_in[root_i]][relation_with_root[i]]++;
    }

    // Use DP to find the number of possible solutions
    solution_num[0][0] = 1;
    for (int i = 1; i <= uf_set_num; i++) {
      for (int j = 0; j <= p1; j++) {

        // Both groups in an uf-set could be chosen
        if (j >= uf_sets[i][SAME])
          solution_num[i][j] += solution_num[i-1][j-uf_sets[i][SAME]];
        if (j >= uf_sets[i][DIFF])
          solution_num[i][j] += solution_num[i-1][j-uf_sets[i][DIFF]];
      }
    }

    // If the solution is unique, print people in p1.
    // Elsewise, print "no".
    if (solution_num[uf_set_num][p1] == 1) {

      // Trace back the process of DP, mark each
      // chosen group.
      int j = p1;
      int chosen_group[MAX_N];
      memset(chosen_group, 0, sizeof(chosen_group));

      for (int i = uf_set_num; i >= 1; i--) {
        if (solution_num[i][j] == solution_num[i-1][j-uf_sets[i][SAME]]) {
          chosen_group[i] = SAME;
          j -= uf_sets[i][SAME];
        } else if (solution_num[i][j] == solution_num[i-1][j-uf_sets[i][DIFF]]) {
          chosen_group[i] = DIFF;
          j -= uf_sets[i][DIFF];
        }
      }

      // Print all chosen serials
      for (int i = 1; i <= p1+p2; i++) {
        if (relation_with_root[i] == chosen_group[uf_set_in[find_update(i)]])
          printf("%d\n", i);
      }
      printf("end\n");

    } else {
      printf("no\n");
    }
  }

  return 0;
}