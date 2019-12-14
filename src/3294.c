/*
 * POJ 3294 Life Forms
 * 
 * Notice that all substrings could be represented by the prefix
 * of a suffix string. So if we concatenate all strings together
 * and build a suffix array of it, all common substrings could be
 * found in this array, like:
 * 
 * Input strings:
 * abcd
 * cdef
 * 
 * Concatenated string:
 * abcd@cdef
 * 
 * Sorted suffixes in the suffix array:
 * 1 abcd@cdef
 * 2 bcd@cdef
 * 3 cdef
 * 4 cd@cdef
 * 5 def
 * 6 d@cdef
 * 7 ef
 * 8 f
 * 9 @
 * 
 * Note that 3&4's longest common prefix and 5&6's longest
 * common prefix are both common substrings between input
 * strings. So all we need are:
 * 
 * - an array indicating which input string a certain
 *   character belongs to
 * - an array indicating the longest common prefix
 *   between adjacent suffixes in sorted suffix array.
 * 
 * The algorithm runs as follow:
 * 
 * 1. Concatenate all input strings to string `str`, recording
 *    the belongingness of each character in `str` by an array;
 * 2. Obtain the suffix array of `str`
 * 3. Build the height array of `str` by the suffix array (height
 *    array is the array recording the length of longest common
 *    prefix of two adjacent suffixes in the sorted array).
 * 4. From 0 to MAX_LEN, try the longest possible length of common substring
 *    using a binary search. We may judge if a length `len` is possible
 *    by checking if there is a common prefix with length `len` between
 *    a set of sorted suffixes, and head elements of these suffixes
 *    cover more than a half of inputted strings.
 * 5. As we have found the longest length, we may print all common
 *    substrings with this length in order.
 * 
 */ 

#include <stdio.h>
#include <string.h>

#define MAX_N 101
#define MAX_LEN 1001
#define ARR_SIZE (MAX_N*MAX_LEN)

// A simple swap function
void swap(int** x, int** y) {
  int *tmp = *x;
  *x = *y;
  *y = tmp;
}

/*
 * Suffix Array API
 */

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
 * Relevant to the question
 */

// belong[i] is the serial of the string containing `i`. 
int belong[ARR_SIZE];
// Inputs
int n, str_len;
char str[ARR_SIZE];

// Returns 1 if there exists any possible common substring
// with length `len`, 0 elsewise.
int check(int len) {

  // Record the appearance of current picked
  // prefix in all suffixes.
  int match_cnt = 0;
  char visited[MAX_N] = {0};

  for (int i = 2; i < str_len; i++) {
    
    // The longest common prefix between suffix[i]
    // and suffix[i-1] should be longer than `len`.
    //
    // If all `height`s from suffix i to j are
    // not smaller than `len`, it could be asserted
    // that a common prefix with length not smaller
    // than `len` exists.
    if (height[i] >= len) {

      // For each element in height[], two suffixes
      // are involved, so visit both of them.
      //
      // Note that each suffix[i] represents the location
      // of an element in `str`.
      if (visited[belong[suffix[i]]] == 0) {
        visited[belong[suffix[i]]] = 1;
        match_cnt++;
      }
      if (visited[belong[suffix[i-1]]] == 0) {
        visited[belong[suffix[i-1]]] = 1;
        match_cnt++;
      }
    } else {

      // Current common prefix has appeared in
      // more than half of suffixes, so this
      // prefix fits our requirements.
      if (match_cnt > n/2) return 1;

      // Current common prefix is not satisfying,
      // so clear the match record to find a new one.
      memset(visited, 0, sizeof(visited));
      match_cnt = 0;
    }
  }

  return 0;
}

// Print all substrings with length `len`.
//
// The logic resembles check().
void print(int len) {

  // Record the appearance of current picked
  // prefix in all suffixes.
  int match_cnt = 0, match_base;
  char visited[MAX_N] = {0};

  for (int i = 2; i <= str_len; i++) {
    if (height[i] >= len) {

      // Find the start location of the possible
      // common substring.
      match_base = suffix[i];

      if (visited[belong[suffix[i]]] == 0) {
        visited[belong[suffix[i]]] = 1;
        match_cnt++;
      }
      if (visited[belong[suffix[i-1]]] == 0) {
        visited[belong[suffix[i-1]]] = 1;
        match_cnt++;
      }
    } else {

      // Current common prefix has appeared in
      // more than half of suffixes, so this
      // prefix fits our requirements.
      //
      // Just print the substring started by
      // match_base we have found.
      if (match_cnt > n/2) {
        for (int i = 0; i < len; i++)
          putchar(str[match_base+i]+'a'-1);
        putchar('\n');
      }

      // Current common prefix is not satisfying,
      // so clear the match record to find a new one.
      memset(visited, 0, sizeof(visited));
      match_cnt = 0;
    }
  }

}

int main() {

  // Use a temporary array to store each
  // inputted string.
  char tmp_str[MAX_LEN];
  int l, r;

  while (scanf("%d", &n) != 0 && n) {

    // Concatenate all inputed strings into `str`,
    // splitting by characters not in inputed strings.
    str_len = 0;
    for (int i = 0; i < n; i++) {

      // Get an input string and copy it to `str`.
      scanf("%s", tmp_str);
      for (int j = 0; tmp_str[j] != 0; j++) {

        // Set the belongingness of each character
        belong[str_len] = i; 

        // We need to shift all characters to the position
        // started by 1 as the bucket sort requires. 
        str[str_len++] = tmp_str[j]-'a'+1;
      }

      // The alphabet ends at 'z' (26 after the shift),
      // so we start the value of padding character by 27.
      str[str_len++] = 27+i;
    }
    str[str_len-1] = 0;

    // Set suffix[], rank[] and height[].
    init_suffix_array(str, str_len, 127);

    // Use a binary search based on check()
    // to find the longest common substring.
    l = 0, r = MAX_LEN;
    while (l < r) {
      int mid = (l+r+1)/2;
      if (check(mid)) l = mid;
      else r = mid-1;
    }

    // l is the max length of common substring.
    // Print all common substrings with length `l`.
    if (l == 0) printf("?\n");
    else print(l);

    putchar('\n');
  }
  
  return 0;
}