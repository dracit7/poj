/*
 * POJ 3277 City Horizon
 * 
 * A natural approach is sort the x axis of all buildings,
 * for each pair of adjacent x axises, calculate the area
 * of the horizon square between them and sum all areas up.
 * 
 * We could use a segment tree to maintain all x axises, and
 * do an update operation for each inputted building: for all
 * segments covered by this building, if the current height
 * of this segment is less than the height of building, update
 * the height. After all these operation, use a divide & conquer
 * algo to sum all segments' areas up.
 * 
 * However, this problem is harder than it appears to be, since
 * the range of x axis or height inputted are both [0, 1000000000],
 * which is far too large for us to maintain a segment tree
 * (which would cost 1000000000^2 space units).
 * 
 * Fortunately, the max number of buildings is only 40000, so a
 * simple discretization could help. we use an array of map to
 * map each x axis to an unique serial, and maintain the segment
 * tree of all serials, so the size of segment tree would not be
 * larger than 1000000000*40000 space units, which is acceptable.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_N 50000

// Use an array to map the range in segment tree
// to x axises.
long long map[MAX_N*2];
// Record the height of each building.
long long height[MAX_N];

int cmp(const void* a, const void* b) {
  return *((long long*)a) - *((long long*)b);
}

// Self-implemented unique() based on Cpp reference.
long long* unique(long long *first, long long* last) {
  if (first == last) return last;

  long long* result = first;
  while (++first != last) {
    if (!(*result == *first))
      *(++result)=*first;
  }
  return ++result;
}

/*
 * Segment tree APIs
 */

// Max height of horizon in a certain range.
//
// This array should be the body of
// segment tree, all operations affect
// this array. It could have different
// meanings, in this case it's colors.
struct {
  int l_index, r_index;
  long long height;
} seg_tree[MAX_N*8];

// User interface of segment tree.
// For different problems, the node of segment
// tree may have different meanings, this function
// is used to merge the value of two children to
// their parent.
int merge(int a, int b) {
  return a+b;
}

// Init the segment tree.
void init_tree(int root, int l, int r) {

  // The whole tree is a map from a segment
  // of index to a segment of x axis.
  seg_tree[root].l_index = l;
  seg_tree[root].r_index = r;
  seg_tree[root].height = 0;

  // Exit from recursion.
  if (l+1 == r) {
    return;
  }

  // Typical divide & conquer.
  int mid = (l+r)/2;
  init_tree(root*2, l, mid);
  init_tree(root*2+1, mid, r);
}

// Point update.
// Add a building's height to the segment tree.
//
// l and r in this function is not indexes in the segment tree
// but real x axises of a building. So we use map[i] to map the
// indexes stored in the segment tree to real x axises.
void update_tree(int building, long long l, long long r, int root) {

  // Segment cover, update the height of segment if necessary.
  if (map[seg_tree[root].l_index] == l && map[seg_tree[root].r_index] == r) {
    if (height[building] > seg_tree[root].height)
      seg_tree[root].height = height[building];
    return;
  }

  // Typical divide & conquer.
  int mid = map[(seg_tree[root].l_index + seg_tree[root].r_index)/2];
  if (r <= mid)
    update_tree(building, l, r, root*2);
  else if (l >= mid)
    update_tree(building, l, r, root*2+1);
  else {
    update_tree(building, l, mid, root*2);
    update_tree(building, mid, r, root*2+1);
  }
}

// solve the problem.
//
// Use divide & conquer to sum all squares' areas up.
long long solve(int height, int root) {

  // Parent is higher than child, so update the child's height.
  if (seg_tree[root].height < height)
    seg_tree[root].height = height;

  // Calculate each unit square's area
  if (seg_tree[root].l_index + 1 == seg_tree[root].r_index)
    return (map[seg_tree[root].r_index] - map[seg_tree[root].l_index]) * seg_tree[root].height;
  
  // Sum up
  return solve(seg_tree[root].height, root*2) + solve(seg_tree[root].height, root*2+1);
}

int main() {

  int n; // Number of inputs
  int ptr = 0; // Temporary variable to help discretization
  long long l_axis[MAX_N], r_axis[MAX_N];

  // Get all inputs and discretize them.
  scanf("%d", &n);
  for (int i = 1; i <= n; i++) {
    scanf("%d%d%d", &l_axis[i], &r_axis[i], &height[i]);
    map[++ptr] = l_axis[i];
    map[++ptr] = r_axis[i];
  }

  // Sort all x axises.
  qsort((void*)(map+1), ptr, sizeof(long long), cmp);

  // Sift out repeated axises to optimize the size
  // of segmented tree.
  int sifted_len = unique(map+1, map+ptr+1) - (map+1);

  // Construct the segment tree
  init_tree(1, 1, sifted_len);
  for (int i = 1; i <= n; i++) {
    update_tree(i, l_axis[i], r_axis[i], 1);
  }

  // Print the answer
  printf("%lld\n", solve(0, 1));

  return 0;
}