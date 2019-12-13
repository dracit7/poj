/*
 * POJ 3321 Apple Tree
 * 
 * Seems like a typical application of binary indexed tree.
 * After figuring out the principle of BIT, this question
 * is not that hard.
 * 
 * Firstly, we record the range of each fork by start[x]
 * and end[x], so the total number of apple on `x` should
 * be num(start[x])+num(start[x]+1)+...+num(end[x])), which
 * is prefix_sum(end[x]) - prefix_sum(start[x]-1). With BIT,
 * it is faster to calculate prefix_sum(t).
 * 
 * The problem is the 2-dim array is too large for the online
 * compiler, and I do not want to implement a vector myself
 * just for a quiz, so I wrote my solution in C++ (C with
 * class, strictly speaking).
 * 
 */

#include <cstdio>
#include <vector>
using std::vector;

#define MAX_N 100001

// LST(x) is the lowest 1 in x's binary representation.
#define LST(x) ((x) & (-(x)))

// Define a 2-dim array to store the relations
// of branches inputed.
//
// tree[i][j]: fork `i` and fork `j` are connected by a branch.
vector<vector<int>> tree(MAX_N);

// The binary indexed tree (BIT).
int binary_indexed_tree[MAX_N];
// have_apple[i] is equal to 1 if there is an apple on fork `i`,
// -1 otherwise.
int have_apple[MAX_N];

// start[i] -> end[i]: the range of fork `i` containing its subforks.
int start[MAX_N];
int end[MAX_N];

// pos_ptr helps transform the tree form of forks to range form.
int pos_ptr;

// This recursive function transforms the tree form of forks
// to range form, making it easier to be maintained by BIT.
void tree_to_range(int fork) {

  // Mark the start position of `fork`
  start[fork] = pos_ptr;
  // Transform each subfork.
  for (int i = 0; i < tree[fork].size(); i++) {
    pos_ptr++;
    tree_to_range(tree[fork][i]);
  }
  // Mark the end position of `fork`
  end[fork] = pos_ptr;
}

// Update the BIT by adding `x` to `BIT[pos]`
// `range` indicates the size of BIT.
void bit_update(int pos, int range, int x) {

  // Considering the attribute of BIT, we need to
  // update all relevant bits.
  while (pos <= range) {
    binary_indexed_tree[pos] += x;
    pos += LST(pos);
  }
}

// Return the total number of apples on forks
// not greater than `pos`.
int bit_query(int pos) {

  // Considering the attribute of BIT, we need to
  // add all ancestor forks' value together.
  int sum = 0;
  while (pos > 0) {
    sum += binary_indexed_tree[pos];
    pos -= LST(pos);
  }

  return sum;
}

int main() {

  // All variables to be inputted defined in
  // the question.
  int N, M, u, v, x;
  char ope;

  // Build the tree defined by the input.
  scanf("%d", &N);
  for (int i = 1; i < N; i++) {
    scanf("%d%d", &u, &v);
    tree[u].push_back(v);
  }

  // Transform the tree to range form.
  pos_ptr = 1;
  tree_to_range(1);

  // Construct the BIT
  for (int i = 1; i <= N; i++) {
    bit_update(start[i], N, 1);
    have_apple[i] = 1;
  }

  // Operations
  scanf("%d", &M);
  for (int i = 0; i < M; i++) {
    scanf(" %c%d", &ope, &x);

    // Change the state of fork x,
    // and update the BIT.
    if (ope == 'C') {

      // If x has apple, now the apple is picked,
      // so reduce the apple number in BIT;
      // Elsewise, a new apple appears, so increase
      // the apple number in BIT.
      bit_update(start[x], N, -have_apple[x]);
      have_apple[x] = -have_apple[x];
    
    // Query the BIT
    } else {
      printf("%d\n", bit_query(end[x]) - bit_query(start[x]-1));
    }
  }

  return 0;
}