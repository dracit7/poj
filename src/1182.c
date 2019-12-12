#include <stdio.h>

#define MAXARRAY 65536

// Possible relations
enum relation {
  SAME, // same species
  PREDATE, // a is the predator of b
  EATENBY, // b is the predator of a
};

// Possible operations
enum operations {
  IN_SAME_SPECIES = 1,
  IS_PREDATOR = 2,
};

/*
 * We put all objects with certain relationship in an uf-set,
 * so there will be an uf-set for each group.
 */

// parent[i]: the parent node of `i` in the uf-set
int parent[MAXARRAY]; 
// relation_with_root[i]: `i`'s relation with its root
int relation_with_root[MAXARRAY];

/*
 * uf-set APIs
 */

// find the root of `x` and update all
// relations_with_root in this subtree.
int find_update(int x) {
  if (x == parent[x])
    return x;

  // After a merge operation, we suppose the root of x
  // has changed, so x's relation_with_root need to be updated.
  //
  // We use a recursive function to update all states on a
  // chain from x to root - so the first element to be updated
  // should be the old root: `x` is the old root and `x`'s parent
  // is the new root.
  //
  // Since relation_with_root[new_root] should be SAME(0), this step
  // simply keep the value of relation_with_root[x] unchanged.
  //
  // For the next step, `x` is the child of the old root, so
  // relation_with_root[x] is the relation with the *old* root now,
  // and since the operations of relation are arithmatic, the relation
  // with the new root should be the composition of relation[x->old_root]
  // and relation[old_root->new_root].
  //
  // So, relation[x] = relation[x->old_root] + relation[x->new_root] = relation[x->new_root].
  //
  // Next, to ensure that `x`'s parent is still the child of new_root 
  // in the next update operation, we need to do a path compression.
  //
  // It's important to do the recursive call before the change of relation,
  // I have WAed twice because of the wrong order.
  int px = parent[x];
  parent[x] = find_update(parent[x]);
  relation_with_root[x] = (relation_with_root[x]+relation_with_root[px]) % 3;

  return parent[x];
}

// merge two uf-sets `a` and `b`
void merge(int a, int b) {
  parent[b] = a;
}

// a is the predator of b
int is_predator(int a, int b) {
  return (relation_with_root[a] + 1) % 3 == relation_with_root[b];
}

int main() {

  int N, K; // N animals, K statements
  scanf("%d%d", &N, &K);

  // Init all uf-sets and relations
  for (int i = 0; i < N; i++) {
    parent[i] = i;
    relation_with_root[i] = SAME;
  }

  int lie_times = 0;
  int ope, x, y;

  // Read all statements and discern the occurence of lies.
  for (int i = 0; i < K; i++) {
    scanf("%d%d%d", &ope, &x, &y);

    // Out of range
    if (x > N || y > N) {
      lie_times++;
      continue;
    }

    // Self-predator
    if (ope == IS_PREDATOR && x == y) {
      lie_times++;
      continue;
    }

    int root_x = find_update(x);
    int root_y = find_update(y);

    // If x & y's relationship is certain, we could
    // discern the facticity of this statement rightaway.
    if (root_x == root_y) {
      if (ope == IN_SAME_SPECIES && relation_with_root[x] != relation_with_root[y])
        lie_times++;
      if (ope == IS_PREDATOR && !is_predator(x, y))
        lie_times++;

    // Elsewise, assume that the statement is true.
    } else {
      merge(root_x, root_y);
      
      // Since we've changed the root of y, we need to
      // modify the relation_with_root of y.
      //
      // But how? Firstly, root_x and root_y's relation can be
      // represented by relation(x, y) & relation(x, root_x) & relation(y, root_y).
      //
      // relation(x, y) = ope - 1, since the statement is true.
      // relation(x, root_x) = relation_with_root[x]
      // relation(y, root_y) = relation_with_root[y]
      // SO we can come up with a formula to calculate relation_with_root_x[root_y].
      //
      // Another WA here: do not write ope-1 straightaway, use ope+2 instead.
      //
      // We modify the relation of y's children in
      // the next find_update() operation.
      relation_with_root[root_y] = (relation_with_root[x] - relation_with_root[y] + ope + 2) % 3;

    }
  }

  printf("%d\n", lie_times);
  return 0;
}