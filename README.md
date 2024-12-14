# Red-Black-Tree-Implemented-in-C

Similar to `std::multimap` in C++, but with a few differences.

## RBTree* newRBTree(KeyComparator comparator)

  Receives a `KeyComparator` (a function pointer of type `bool(unsigned long long, unsigned long long)`) for the new tree, or `NULL` to indicate the default "less than" comparison.
  Creates an empty Red-Black Tree.

## bool RBTreeInsert(RBTree* tree, unsigned long long key, void* value)

  Receives an `unsigned long long` as the key and a `void*` as the value.
  Returns `true` if the insertion is successful (i.e., if `RBTree* tree` is not `NULL` and the insertion is performed).

## RBNode* RBTreeFind(RBTree* tree, unsigned long long key)

  Receives a key and returns a node that matches it. Returns `NULL` if no such node exists in the tree.

## bool RBTreeEraseNode(RBTree* tree, RBNode* node)

  Deletes the specified node from the tree.
  You should manage the memory yourself if you allocated it using `malloc()` or a similar function for the `void*` value.

## bool RBTreeEraseKey(RBTree* tree, unsigned long long key)

  Unlike `std::multimap::erase(key)` in C++, which can erase all elements with the specified key, this function erases only a single node with the given key.
  It is equivalent to calling `RBTreeEraseNode(tree, RBTreeFind(tree, key))`, assuming `RBTreeFind` returns a valid node pointer.
