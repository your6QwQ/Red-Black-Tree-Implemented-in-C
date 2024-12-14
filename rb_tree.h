//
// Created by your on 2024/11/29.
//

#ifndef WINDOWSPROJECT_RB_TREE_H
#define WINDOWSPROJECT_RB_TREE_H

#ifndef __cplusplus
#if defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L
/* bool, true and false are keywords.  */
#else
#define bool	unsigned char
#define true	1
#define false	0
#endif
#else /* __cplusplus */
/* Supporting _Bool in C++ is a GCC extension.  */
#define _Bool	bool
#endif /* __cplusplus */

#include <stdbool.h>
#include <malloc.h>


typedef struct RBNode {
    unsigned long long data_key;
    void* data_value;
    struct RBNode* left;
    struct RBNode* right;
    struct RBNode* parent;
    bool color;
} RBNode;

const RBNode RbTreeNIL = {0};
#define PNIL ((RBNode*)&RbTreeNIL)

typedef bool (*KeyComparator)(unsigned long long, unsigned long long);
typedef struct RBTree {
    RBNode* root;
    size_t size;
    KeyComparator comparator;
} RBTree;

bool RBTreeLess(unsigned long long a, unsigned long long b) {
    return a < b;
}

bool RBTreeGreater(unsigned long long a, unsigned long long b) {
    return b < a;
}

RBTree* newRBTree(KeyComparator comparator) {
    RBTree *tree = (RBTree*) malloc(sizeof(RBTree));

    tree->root = PNIL;
    tree->size = 0;
    tree->comparator = comparator ? comparator : RBTreeLess;

    return tree;
}

RBNode* newRBNode(unsigned long long data_key, void* data_value) {
    RBNode *node = (RBNode*) malloc(sizeof(RBNode));

    node->left = node->right = PNIL;
    node->parent = NULL;
    node->data_key = data_key;
    node->data_value = data_value;
    node->color = true;

    return node;
}

RBNode* RBTreeFind(RBTree* rbTree, unsigned long long data_key) {
    if (!rbTree) return NULL;

    RBNode* temp = rbTree->root;
    while (temp && temp != PNIL) {
        if (!rbTree->comparator(data_key, temp->data_key) && !rbTree->comparator(temp->data_key, data_key)) return temp;
        if (rbTree->comparator(data_key, temp->data_key)) temp = temp->left;
        else temp = temp->right;
    }
    return NULL;
}

bool RBNodeIsLeft(RBNode* node) {
    return node->parent && node->parent->left == node;
}

bool RBNodeIsRight(RBNode* node) {
    return node->parent && node->parent->right == node;
}

bool RBNodeRotateLeft(RBTree* tree, RBNode* rbNode) {
    RBNode *l, *r;
    l = rbNode;
    if (l == PNIL || !l) return false;
    r = rbNode->right;
    if (r == PNIL || !r) return false;

    if (r->left != PNIL) r->left->parent = l;
    l->right = r->left;
    if (l->parent) {
        if (RBNodeIsLeft(l)) l->parent->left = r;
        else l->parent->right = r;
    }
    else {
        tree->root = r;
    }
    r->parent = l->parent;
    l->parent = r;
    r->left = l;
    return true;
}

bool RBNodeRotateRight(RBTree* tree, RBNode* rbNode) {
    RBNode *l, *r;
    r = rbNode;
    if (r == PNIL || !r) return false;
    l = rbNode->left;
    if (l == PNIL || !l) return false;

    if (l->right != PNIL) l->right->parent = r;
    r->left = l->right;
    if (r->parent) {
        if (RBNodeIsLeft(r)) r->parent->left = l;
        else r->parent->right = l;
    }
    else {
        tree->root = l;
    }
    l->parent = r->parent;
    r->parent = l;
    l->right = r;
    return true;
}

RBNode* RBNodeSeekSibling(RBNode* node) {
    if (!node || !node->parent) {
        return NULL;
    }
    return node == node->parent->left ? node->parent->right : node->parent->left;
}

RBNode* RBNodeSeekUncle(RBNode* node) {
    if (!node || !node->parent) {
        return NULL;
    }
    return RBNodeSeekSibling(node->parent);
}

RBNode* RBTreeSeekParent(RBTree* rbTree, unsigned long long key) {
    RBNode *ptr = rbTree->root, *temp = NULL;
    if (ptr == PNIL) return NULL;

    while (ptr != PNIL) {
        temp = ptr;
        if (rbTree->comparator(key, ptr->data_key)) {
            ptr = ptr->left;
        }
        else {
            ptr = ptr->right;
        }
    }

    return temp;
}

void RBTreeInsertBalanceManifest(RBTree* tree, RBNode* node) {
    if (node == PNIL || !node) return;

    if (node == tree->root || !node->parent) return;                 // Case 0

    if (!node->parent->color) return;                                // Case 1

    if (node->parent == tree->root) {                                // Case 2
        node->parent->color = false;
        return;
    }

    if (!RBNodeSeekUncle(node)) exit(-1);
    if (RBNodeSeekUncle(node)->color) {                              // Case 3
        node->parent->color = false;
        node->parent->parent->color = true;
        RBNodeSeekUncle(node)->color = false;
        RBTreeInsertBalanceManifest(tree, node->parent->parent);
        return;
    }

    if (RBNodeIsLeft(node) ^ RBNodeIsLeft(node->parent)) {           // Case 4
        RBNodeIsLeft(node) ? RBNodeRotateRight(tree, node->parent) : RBNodeRotateLeft(tree, node->parent);
        RBTreeInsertBalanceManifest(tree, RBNodeIsLeft(node) ? node->left : node->right);
        return;
    }

    else {                                                           // Case 5
        RBNodeIsLeft(node) ? RBNodeRotateRight(tree, node->parent->parent) : RBNodeRotateLeft(tree, node->parent->parent);
        node->parent->color = false;
        RBNodeSeekSibling(node)->color = true;
        return;
    }
}

bool RBTreeInsert(RBTree* rbTree, unsigned long long data_key, void* data_value) {
    if (!rbTree) return false;

    if (!rbTree->size) {                                             // Case 0
        rbTree->root = newRBNode(data_key, data_value);
    }
    else {
        RBNode* parent = RBTreeSeekParent(rbTree, data_key), * new_node = newRBNode(data_key, data_value);

        if (rbTree->comparator(data_key, parent->data_key)) {
            parent->left = new_node;
            parent->left->parent = parent;
        }
        else {
            parent->right = new_node;
            parent->right->parent = parent;
        }

        RBTreeInsertBalanceManifest(rbTree, new_node);
    }

    rbTree->size++;
    return true;
}

void RBTreeEraseBalanceManifest(RBTree* rbTree, RBNode* node) {
    if (!rbTree || !node || !RBNodeSeekSibling(node)) return;

    if (RBNodeSeekSibling(node)->color) {           // Case 0
        if (RBNodeIsLeft(node)) RBNodeRotateLeft(rbTree, node->parent);
        else RBNodeRotateRight(rbTree, node->parent);
        node->parent->color = true;
        node->parent->parent->color = false;
        RBTreeEraseBalanceManifest(rbTree, node);
        return;
    }

    if (node->parent->color && !RBNodeSeekSibling(node)->left->color && !RBNodeSeekSibling(node)->right->color) {  // Case 1
        node->parent->color = false;
        RBNodeSeekSibling(node)->color = true;
        return;
    }

    if (!node->parent->color && !RBNodeSeekSibling(node)->color && !RBNodeSeekSibling(node)->left->color && !RBNodeSeekSibling(node)->right->color) {  // Case 2
        RBNodeSeekSibling(node)->color = true;
        RBTreeEraseBalanceManifest(rbTree, node->parent);
        return;
    }

    if (RBNodeIsLeft(node) && RBNodeSeekSibling(node)->left->color && !RBNodeSeekSibling(node)->right->color) {  // Case 3
        RBNodeRotateRight(rbTree, RBNodeSeekSibling(node));
        RBNodeSeekSibling(node)->color = false;
        RBNodeSeekSibling(node)->right->color = true;
        RBTreeEraseBalanceManifest(rbTree, node);
        return;
    }
    else if (!RBNodeIsLeft(node) && !RBNodeSeekSibling(node)->left->color && RBNodeSeekSibling(node)->right->color) {
        RBNodeRotateLeft(rbTree, RBNodeSeekSibling(node));
        RBNodeSeekSibling(node)->color = false;
        RBNodeSeekSibling(node)->left->color = true;
        RBTreeEraseBalanceManifest(rbTree, node);
        return;
    }

    if (RBNodeIsLeft(node) && !RBNodeSeekSibling(node)->color && RBNodeSeekSibling(node)->right->color) {  // Case 4
        RBNodeRotateLeft(rbTree, node->parent);
        node->parent->parent->color = node->parent->color;
        node->parent->color = false;
        node->parent->parent->right->color = false;
        return;
    }
    else if (!RBNodeIsLeft(node) && !RBNodeSeekSibling(node)->color && RBNodeSeekSibling(node)->left->color) {
        RBNodeRotateRight(rbTree, node->parent);
        node->parent->parent->color = node->parent->color;
        node->parent->color = false;
        node->parent->parent->left->color = false;
        return;
    }

}

bool RBTreeEraseNode(RBTree* rbTree, RBNode* node) {
    if (!rbTree || !node) return false;

    if (rbTree->size == 1) {                                                     // Case 0
        free(node);
        rbTree->root = PNIL;
        rbTree->size--;
        return true;
    }

    if (node->left != PNIL && node->right != PNIL) {                             // Case 1
        RBNode *temp = node->right;
        while (temp->left != PNIL) {
            temp = temp->left;
        }
        temp->data_key = temp->data_key ^ node->data_key;
        node->data_key = temp->data_key ^ node->data_key;
        temp->data_key = temp->data_key ^ node->data_key;
        void* temp_ptr = node->data_value;
        node->data_value = temp->data_value;
        temp->data_value = temp_ptr;
        return RBTreeEraseNode(rbTree, temp);
    }

    if (node->left == PNIL && node->right == PNIL) {                             // Case 2
        if (node->color) {
            if (RBNodeIsLeft(node)) node->parent->left = PNIL;
            else node->parent->right = PNIL;
            free(node);
        }
        else {
            RBTreeEraseBalanceManifest(rbTree, node);
            if (RBNodeIsLeft(node)) node->parent->left = PNIL;
            else node->parent->right = PNIL;
            free(node);
        }
        rbTree->size--;
        return true;
    }

    else {                                                                       // Case 3
        if (node->left != PNIL) {
            node->left->color = false;
            node->left->parent = node->parent;
            if (!node->parent) rbTree->root = node->left;
            else if (RBNodeIsLeft(node)) node->parent->left = node->left;
            else node->parent->right = node->left;
            free(node);
        }
        else {
            node->right->color = false;
            node->right->parent = node->parent;
            if (!node->parent) rbTree->root = node->right;
            else if (RBNodeIsLeft(node)) node->parent->left = node->right;
            else node->parent->right = node->right;
            free(node);
        }
        rbTree->size--;
        return true;
    }
}

bool RBTreeEraseKey(RBTree* rbTree, unsigned long long data_key) {
    return RBTreeEraseNode(rbTree, RBTreeFind(rbTree, data_key));
}

#endif //WINDOWSPROJECT_RB_TREE_H
