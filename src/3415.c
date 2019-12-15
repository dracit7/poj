/*
 * POJ 3415 Common substrings
 * 
 * Logically, this quiz resembles POJ 3294, and even easier
 * since it only involves two inputted strings. However, the
 * running time would exceed the limit if we check each pair
 * of suffixes, so it's necessary to optimize the algorithm.
 * 
 * Before optimizing, we could just concatenate string A and 
 * string B with a splitting character, and build a height 
 * array based on the concatenated string. After comparing each
 * pair of suffixes' height with K, we could obtain the number
 * of legal substrings. This algorithm costs O(n^2).
 * 
 * To optimize this algorithm, we need the help of *monotonic
 * stack*. This is a kind of stack with certain monotonicity.
 * 
 * We will see how the algorithm works in comments of solve()
 * function.
 * 
 */

#include <stdio.h>

#define ARR_SIZE 200002

/*
 * Suffix Array API
 */

// A simple swap function
void swap(int** x, int** y) {
  int *tmp = *x;
  *x = *y;
  *y = tmp;
}

// suffix: suffix array
// 
// Both suffix[i] = j and rank[j] = i indicates that
// the `i`th suffix in the sorted array is `j`.
int suffix[ARR_SIZE];
int rank[ARR_SIZE];

// height[i]: length of the longest common prefix
// between rank[i] and rank[i-1].
int height[ARR_SIZE];

// We use two keys iteratively in doubling process.
int fst_buf[ARR_SIZE], snd_buf[ARR_SIZE];
// Use bucket sort to reduce time complexity.
int bucket[ARR_SIZE];

// Build suffix[], rank[] and height[] in this function.
// 
// @param str: the string to build sa
// @param len: length of string
// @param range: possible range of characters in `str`.
//
// This version of implementation is based on doubling algorithm
// and bucket sort, so the time complexity is reduced to O(NlogN).
void init_suffix_array(char* str, int len, int range) {

  // We need to swap fst_key and snd_key later,
  // but the array's head pointer is readonly,
  // so a transformation is necessary here.
  int *fst_key = fst_buf;
  int *snd_key = snd_buf;

  // Init the bucket
  for (int i = 0; i < range; i++) bucket[i] = 0;

  // Set the first key
  for (int i = 0; i < len; i++) {
    fst_key[i]=str[i];
    ++bucket[str[i]];
  }

  // Bucket sort on suffix[] based on the first key
  for (int i = 1; i < range; i++) bucket[i] += bucket[i-1];
  for (int i = len-1; i >= 0; i--) suffix[--bucket[fst_key[i]]] = i;

  // Use a temporary value `p` to store the next
  // serial to be assigned.
  int p = 1;

  // Doubling period.
  // `j` is the doubling step.
  for (int j = 1; p < len; j *= 2) {
    p = 0;

    // Handle the gap between n*j and len.
    // This part of value do not have the second key.
    for (int i = len-j; i < len; i++) snd_key[p++] = i;

    // Calculate all `snd_key`s.
    for (int i = 0; i < len; i++)
      if (suffix[i] >= j) snd_key[p++] = suffix[i]-j;
    
    // Bucket sort based on fst_key and snd_key.
    for (int i = 0; i < range; i++) bucket[i] = 0;
    for (int i = 0; i < len; i++) ++bucket[fst_key[snd_key[i]]];
    for (int i = 1; i < range; i++) bucket[i] += bucket[i-1];
    for (int i = len-1; i >= 0; i--) suffix[--bucket[fst_key[snd_key[i]]]] = snd_key[i];

    // Iterate: generate new fst_key
    swap(&fst_key, &snd_key);
    fst_key[suffix[0]] = 0; 
    p = 1;
    for (int i = 1; i < len; i++)
      if (snd_key[suffix[i]] == snd_key[suffix[i-1]] &&
          snd_key[suffix[i]+j] == snd_key[suffix[i-1]+j])
        fst_key[suffix[i]] = p-1;
      else fst_key[suffix[i]] = p++;

    // The range of serials has updated
    range = p;
  }
 
  // Build rank[] and height[]
  for (int i = 1; i < len; i++) rank[suffix[i]] = i;
  p = 0;
  for (int i = 0; i < len-1; height[rank[i++]] = p) {
    if (p) --p;
    for (int j = suffix[rank[i]-1]; str[i+p] == str[j+p]; p++) ;
  }
}

/*
 * Monotonic stack API
 */

struct {
  // How many layers in stack have been
  // merged into this layer.
  int cnt; 
  // The minimum height of merged layers,
  // which is the length of this substring.
  int height;
} mono_stack[ARR_SIZE];

// The current size of stack.
int stack_size;

/*
 * Relevant to the question
 */

// Since there're only 2 inputted strings, there's no
// need to maintain an belongingness array. Keep A's
// length is enough to judge the belongingness.
int len_of_A;
// Inputs
int str_len;
char str[ARR_SIZE];

// Solve the problem.
long long solve(int len) {

  // Use a temporary variable to store the `substr_num`
  // contributed by height[i] in each iteration.
  long long substr_num = 0;
  // Store the answer
  long long ans = 0;

  // Init the monotonic stack.
  stack_size = 0;

  // Find all common substrings from B to A.
  for (int i = 3; i < str_len; i++) {
    
    // The longest common prefix between suffix[i]
    // and suffix[i-1] should be longer than `len`.
    if (height[i] >= len) {

      // How many layers would be merged into this layer.
      int layer_num = 0;

      // height[i] = len(longest_common_prefix(suffix[i], suffix[i-1]))
      //
      // If suffix[i-1] is in string A and suffix[i] is in string B,
      // this would be a legal common substring and be recorded.
      // 
      // If both suffix[i-1] and suffix[i] are in string A, maybe
      // same substring appeared more than once in string A and still
      // be possible to appear in string B as suffix[i+1], so we still
      // have to push a layer into the stack.
      //
      // To conclude, if suffix[i-1] > len_of_A + 1, we push a new layer
      // into the stack (layer_num++), and count its contribution to
      // the number of common substrings into `substr_num`.
      //
      // Attention that we may count more than we should here. The
      // contribution of several consistent suffixes should be the
      // minimum `height[i]` among them, but we recorded the first
      // `height[i]` we met here. We will fix this problem later.
      if (suffix[i-1] > len_of_A + 1) {
        layer_num++;
        // If height[i] is legal, height[i-1] is legal too
        // as long as it is greater than `len`. So we just
        // count all legal suffixes started by suffix[i][0] here.
        substr_num += height[i] - len + 1;
      }

      // If height[i] is going to break the monotone of stack,
      // it means that the minimum common height of some consistent
      // suffixes is lower than we counted, so we pop all incorrect
      // layers out of the stack, delete miscounted substring num
      // from `substr_num`, and merge incorrect layers into one.
      while (stack_size && height[i] <= mono_stack[stack_size].height) {
        // Maybe one of miscounted layers is also a merged layer,
        // so we need to reduce `substr_num` for `cnt` times.
				substr_num -= mono_stack[stack_size].cnt * (mono_stack[stack_size].height - height[i]);
        // Calculate the new cnt of this layer.
				layer_num += mono_stack[stack_size--].cnt;
			}

      // Push the merged layer into the monotonic stack.
      stack_size++;
			mono_stack[stack_size].cnt = layer_num;
			mono_stack[stack_size].height = height[i];
      
      // If this is a legal common substring, add the contribution
      // of this substring into the answer. 
			if (suffix[i] <= len_of_A) ans += substr_num;

    // This suffix has no common prefix with the former
    // one, so the former substring has been picked out
    // and counted (if exists), thus the stack could be
    // emptied.
    } else {

      stack_size = 0;
      substr_num = 0;
    }
  }
  
  // Do it again, from A to B this time.
  stack_size = 0;
  substr_num = 0;

  for (int i = 3; i < str_len; i++) {
    if (height[i] >= len) {

      // The check order reverses.
      int layer_num = 0;
      if (suffix[i-1] <= len_of_A) {
        layer_num++;
        substr_num += height[i] - len + 1;
      }

      while (stack_size && height[i] <= mono_stack[stack_size].height) {
				substr_num -= mono_stack[stack_size].cnt * (mono_stack[stack_size].height - height[i]);
				layer_num += mono_stack[stack_size--].cnt;
			}

      // Push the merged layer into the monotonic stack.
      stack_size++;
			mono_stack[stack_size].cnt = layer_num;
			mono_stack[stack_size].height = height[i];
      
			if (suffix[i] > len_of_A + 1) ans += substr_num;

    } else {

      stack_size = 0;
      substr_num = 0;
    }
  }

  return ans;
}

int main() {

  int K;
  char tmp_str[ARR_SIZE/2];
  str_len = 1;

  while (scanf("%d", &K) && K != 0) {

    // Get inputted strings and concatenate them.
    // This step resembles POJ 3294.
    scanf("%s", tmp_str);
    for (int j = 0; tmp_str[j] != 0; j++) {
      str[str_len++] = tmp_str[j];
    }

    // Record the length of string A.
    len_of_A = str_len - 1;
    // The concatenation character.
    str[str_len++] = '@';

    // Get string B.
    scanf("%s", tmp_str);
    for (int j = 0; tmp_str[j] != 0; j++) {
      str[str_len++] = tmp_str[j];
    }
    str[str_len++] = 0;

    // Set suffix[], rank[] and height[].
    init_suffix_array(str, str_len, 127);

    // Print the answer.
    printf("%lld\n", solve(K));

    // Reset `str`
    str_len = 1;
  }

  return 0;
}