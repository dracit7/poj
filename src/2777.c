/*
 * POJ 2777 Count Color
 * 
 * Typical application of segment tree. For this problem,
 * since the type of color is limited (<=30), we can use
 * bit operations to optimize our solution.
 * 
 * Instead of using an integer to represent the number of
 * colors in a segment, we could represent each color with
 * a certain bit, and the merge() operation would be a bitor(|)
 * between two child segments' color attribute.
 * 
 * What's more, there may be many segment updates in this
 * problem's testcases, so a solution without lazy propagation
 * would not be fast enough.
 * 
 */

#include <stdio.h>

// MAX_L = MAX_O = MAX_N
#define MAX_N 100002

#define swap(x, y) do {x^=y; y^=x; x^=y;} while(0)

/*
 * Segment tree APIs
 */

// Colors in this node's segment, 
// Each bit represents an unique color.
//
// This array should be the body of
// segment tree, all operations affect
// this array. It could have different
// meanings, in this case it's colors.
int seg_tree[MAX_N*4];

// The mark for lazy propagation.
//
// If dirty[i] = x, this node has taken some
// change that may affect its children, so
// we need to pass this change to children
// when we pass through this node to visit
// its children.
//
// The value of dirty[i] should satisfy:
// value[child] = merge(value[child], dirty[parent])
int dirty[MAX_N*4];

// User interface of segment tree.
// For different problems, the node of segment
// tree may have different meaniings, this function
// is used to merge the value of two children to
// their parent.
int merge(int a, int b) {
  return a|b;
}

// Init the segment tree.
void init_tree(int root, int l, int r) {

  // Mark all nodes as not dirty.
  dirty[root] = 0;

  // Exit from recursion.
  // Since each node is painted by color 1
  // at first, we record this information in
  // leaves of the tree.
  if (l == r) {
    seg_tree[root] = 1<<1;
    return;
  }

  // Typical divide & conquer.
  int mid = (l+r)/2;
  init_tree(root*2, l, mid);
  init_tree(root*2+1, mid+1, r);

  seg_tree[root] = merge(seg_tree[root*2], seg_tree[root*2+1]);
}

// Push the dirty mark down to children.
// Standard segtree API in templates.
void push_down(int root) {
  if (dirty[root]) {

    // Update children's value.
    //
    // Note that once a segment is painted by another
    // color, the former color disappears. So we should
    // not merge here, but replace instead.
    seg_tree[root*2] = seg_tree[root*2+1] = dirty[root];

    // Propagate dirty mark to children.
    dirty[root*2] = dirty[root*2+1] = dirty[root];

    // Current node is clean now.
    dirty[root] = 0;
  }
}

// Segment update.
// Standard segtree API in templates.
//
// @param l_up|r_up: the range to update
// @param l_trav|r_trav: the range we're in now
// @param root: the current tree node
// @param val: the value to update
void update_tree(int l_up, int r_up, int l_trav, int r_trav, int root, int val) {

  // Update the segment and return right away if
  // we've found the proper segment, as we use
  // lazy propagation.
  if (l_up <= l_trav && r_up >= r_trav) {
    seg_tree[root] = dirty[root] = val;
    return;
  }

  // If we are going to pass through this node
  // to its children, we need to do push_down().
  push_down(root);

  // Typical divide & conquer.
  int mid = (l_trav+r_trav)/2;
  if (l_up <= mid)
    update_tree(l_up, r_up, l_trav, mid, root*2, val);
  if (r_up > mid)
    update_tree(l_up, r_up, mid+1, r_trav, root*2+1, val);

  seg_tree[root] = merge(seg_tree[root*2], seg_tree[root*2+1]);
}

// Segment query.
// Standard segtree API in templates.
//
// @param l_query|r_query: the range to query
// @param l_trav|r_trav: the range we're in now
// @param root: the current tree node
int query_tree(int l_query, int r_query, int l_trav, int r_trav, int root) {

  // Exit from recursion.
  if (l_query <= l_trav && r_query >= r_trav) {
    return seg_tree[root];
  }

  // If we are going to pass through this node
  // to its children, we need to do push_down().
  push_down(root);

  // Typical divide & conquer.
  int mid = (l_trav+r_trav)/2;
  int ret = 0;
  if (l_query <= mid)
    ret = merge(ret, query_tree(l_query, r_query, l_trav, mid, root*2));
  if (r_query > mid)
    ret = merge(ret, query_tree(l_query, r_query, mid+1, r_trav, root*2+1));
  return ret;
}

// Return the number of 1 in `x`'s binary representation.
//
// We could use bit mask algorithm to implement a O(logN)
// function here, but there's no need in this case.
int bitcnt(int x) {
  int cnt = 0;
  while (x) {
    if (x & 1) cnt++;
    x >>= 1;
  }

  return cnt;
}

int main() {

  int L, T, O; // Variables defined by topic
  char ope[2]; // Operation
  int l, r, color; // Range and color
  scanf("%d%d%d", &L, &T, &O);

  // Build up the segment tree.
  init_tree(1, 1, L);

  // Get operations
  for (int i = 0; i < O; i++) {
    scanf("%s", ope);

    // Query
    if (ope[0] == 'P') {
      scanf("%d%d", &l, &r);
      if (l > r) swap(l, r); // The input is poisonous, this step is essential.
      printf("%d\n", bitcnt(query_tree(l, r, 1, L, 1)));

    // Paint
    } else {
      scanf("%d%d%d", &l, &r, &color);
      if (l > r) swap(l, r); // The input is poisonous, this step is essential.
      update_tree(l, r, 1, L, 1, 1<<color);
    }
  }

  return 0;
}