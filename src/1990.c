/*
 * POJ 1990 MooFest
 * 
 * A natural approach is to calculate the volume of each pair,
 * which costs O(N^2) approximately. Well, I bet that's far
 * too slow to pass POJ's examination.
 * 
 * Notice that the volume produced by `i` and `j` should be
 * `max{v_i, v_j} * (x_i-x_j)`, so if we sort all cows by their
 * volume threshold, we could find that:
 * 
 * max{v_i, v_j} == v_j for all i < j.
 * 
 * Hence, for any `i` in an sorted array of cows:
 * 
 * sum{j=1 to i-1}(volume(i, j)) = i.volume_threshold * sum{j=1 to i-1}(distance(i, j))
 * 
 * So the problem is about calculating sum{j=1 to i-1}(distance(i, j)).
 * (Note that the x_axis of cows are not sorted!)
 * 
 * We could build a BIT of sum(distance(i, j)) from the ground up:
 * since all cows are sorted by volume, each time we update a cow's data
 * into the BIT, there is:
 * 
 * sum{all_j}(distance(i, j)) == sum{j=1 to i-1}(distance(i, j))
 * 
 * So we just add i.volume_threshold * sum{all_j}(distance(i, j)) to sum_value 
 * each time we update the BIT, then sum_value would be the result.
 * 
 * Given constant i:
 * sum{all_j}(distance(i, j)) = sum{x_i<x_j}(distance(i, j)) + sum{x_i>x_j}(distance(i, j))
 * sum{x_i>x_j}(distance(i, j)) = num_j{x_i>x_j} * x_i - prefix_sum(x_i)
 * sum{x_i<x_j}(distance(i, j)) = prefix_sum(MAX) - (i - num_j{x_i>x_j}) * x_i - sum{x_i>x_j}(distance(i, j))
 * 
 * prefix_sum(t) could be easily queried from BIT. We keep two BITs:
 * one for num_j{x_i>x_j}, one for prefix_sum.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_N 20000

/*
 * Standard BIT API
 */

// LST(x) is the lowest 1 in x's binary representation.
#define LST(x) ((x) & (-(x)))

// Update the BIT by adding `x` to `BIT[pos]`
// `range` indicates the size of BIT.
void bit_update(int* bit, int pos, int range, int x) {

  while (pos <= range) {
    bit[pos] += x;
    pos += LST(pos);
  }
}

// Return the total number of apples on forks
// not greater than `pos`.
long long bit_query(int* bit, int pos) {

  long long sum = 0;
  while (pos > 0) {
    sum += bit[pos];
    pos -= LST(pos);
  }

  return sum;
}

/*
 * Cow object
 */

typedef struct {
  int threshold;
  int x_axis;
} cow;

// The comparation function that helps sorting.
int cow_cmp(const void* a, const void* b) {
  return ((cow*)a)->threshold - ((cow*)b)->threshold;
}

/*
 * Global variables
 */

// All cows inputted
cow cows[MAX_N];

// Use two BITs to record:
//
// num_smaller_x[i] records the number of cows
// which has smaller x axis than `i`.
int num_smaller_x[MAX_N];
// sum_smaller_x[i] records the sum of x axes
// which are smaller than `i`'s x axis.
int sum_smaller_x[MAX_N];

int main() {

  int N;
  long long result = 0;
  scanf("%d", &N);
  
  // Get the voice threshold and x axes of all cows.
  for (int i = 0; i < N; i++) {
    scanf("%d%d", &cows[i].threshold, &cows[i].x_axis);
  }

  // Sort the cows.
  qsort((void *)cows, N, sizeof(cow), cow_cmp);

  // Do the calculation as the algorithm describes.
  for (int i = 1; i < N; i++) {
    bit_update(num_smaller_x, cows[i-1].x_axis, MAX_N, 1);
    bit_update(sum_smaller_x, cows[i-1].x_axis, MAX_N, cows[i-1].x_axis);

    long long num = bit_query(num_smaller_x, cows[i].x_axis);
    long long sum_smaller = bit_query(sum_smaller_x, cows[i].x_axis);
    long long sum_higher = bit_query(sum_smaller_x, MAX_N) - sum_smaller;

    result += cows[i].threshold * 
      ((num*cows[i].x_axis - sum_smaller) + 
      (sum_higher - (i-num)*cows[i].x_axis));
  }

  printf("%lld\n", result);
  return 0;

}