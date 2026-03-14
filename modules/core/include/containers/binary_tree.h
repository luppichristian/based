// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/safe.h"
#include "basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

/*
BINARY_TREE_* manages an intrusive binary tree.
Each node must provide `left`, `right`, and `parent` members.

Example:

  typedef struct bst_node {
    struct bst_node* left;
    struct bst_node* right;
    struct bst_node* parent;
    i32 key;
  } bst_node;
*/

// Structural predicates.
#define BINARY_TREE_IS_ROOT(node) ((node)->parent == NULL)
#define BINARY_TREE_IS_LEAF(node) ((node)->left == NULL && (node)->right == NULL)

// Traversal helpers.
#define BINARY_TREE_FIRST_INORDER(root, out) stmt(      \
    (out) = (root);                                     \
    safe_while ((out) != NULL && (out)->left != NULL) { \
      (out) = (out)->left;                              \
    })

#define BINARY_TREE_NEXT_INORDER(root, node, out) stmt(                                    \
    typeof((root)) _binary_tree_root = (root);                                             \
    typeof((node)) _binary_tree_cursor = (node);                                           \
    (out) = NULL;                                                                          \
    if (_binary_tree_root != NULL && _binary_tree_cursor != NULL) {                        \
      if (_binary_tree_cursor->right != NULL) {                                            \
        _binary_tree_cursor = _binary_tree_cursor->right;                                  \
        safe_while (_binary_tree_cursor->left != NULL) {                                   \
          _binary_tree_cursor = _binary_tree_cursor->left;                                 \
        }                                                                                  \
        (out) = _binary_tree_cursor;                                                       \
      } else {                                                                             \
        safe_while (_binary_tree_cursor != _binary_tree_root) {                            \
          typeof((_binary_tree_cursor)) _binary_tree_parent = _binary_tree_cursor->parent; \
          if (_binary_tree_cursor == _binary_tree_parent->left) {                          \
            (out) = _binary_tree_parent;                                                   \
            break;                                                                         \
          }                                                                                \
          _binary_tree_cursor = _binary_tree_parent;                                       \
        }                                                                                  \
      }                                                                                    \
    })

#define BINARY_TREE_FIRST_POSTORDER(root, out) stmt(                              \
    (out) = (root);                                                               \
    safe_while ((out) != NULL && ((out)->left != NULL || (out)->right != NULL)) { \
      (out) = (out)->left != NULL ? (out)->left : (out)->right;                   \
    })

#define BINARY_TREE_NEXT_POSTORDER(root, node, out) stmt(                                                                   \
    typeof((root)) _binary_tree_root = (root);                                                                              \
    typeof((node)) _binary_tree_cursor = (node);                                                                            \
    (out) = NULL;                                                                                                           \
    if (_binary_tree_root != NULL && _binary_tree_cursor != NULL && _binary_tree_cursor != _binary_tree_root) {             \
      typeof((_binary_tree_cursor)) _binary_tree_parent = _binary_tree_cursor->parent;                                      \
      if (_binary_tree_cursor == _binary_tree_parent->right || _binary_tree_parent->right == NULL) {                        \
        (out) = _binary_tree_parent;                                                                                        \
      } else {                                                                                                              \
        _binary_tree_cursor = _binary_tree_parent->right;                                                                   \
        safe_while (_binary_tree_cursor->left != NULL || _binary_tree_cursor->right != NULL) {                              \
          _binary_tree_cursor = _binary_tree_cursor->left != NULL ? _binary_tree_cursor->left : _binary_tree_cursor->right; \
        }                                                                                                                   \
        (out) = _binary_tree_cursor;                                                                                        \
      }                                                                                                                     \
    })

#define BINARY_TREE_NEXT_PREORDER(root, node, out) stmt(                                                \
    typeof((root)) _binary_tree_root = (root);                                                          \
    typeof((node)) _binary_tree_cursor = (node);                                                        \
    (out) = NULL;                                                                                       \
    if (_binary_tree_root != NULL && _binary_tree_cursor != NULL) {                                     \
      if (_binary_tree_cursor->left != NULL) {                                                          \
        (out) = _binary_tree_cursor->left;                                                              \
      } else if (_binary_tree_cursor->right != NULL) {                                                  \
        (out) = _binary_tree_cursor->right;                                                             \
      } else {                                                                                          \
        safe_while (_binary_tree_cursor != _binary_tree_root) {                                         \
          typeof((_binary_tree_cursor)) _binary_tree_parent = _binary_tree_cursor->parent;              \
          if (_binary_tree_cursor == _binary_tree_parent->left && _binary_tree_parent->right != NULL) { \
            (out) = _binary_tree_parent->right;                                                         \
            break;                                                                                      \
          }                                                                                             \
          _binary_tree_cursor = _binary_tree_parent;                                                    \
        }                                                                                               \
      }                                                                                                 \
    })

// Mutation helpers.
#define BINARY_TREE_INSERT_LEFT(parent, node) stmt( \
    (node)->parent = (parent);                      \
    (parent)->left = (node);)

#define BINARY_TREE_INSERT_RIGHT(parent, node) stmt( \
    (node)->parent = (parent);                       \
    (parent)->right = (node);)

#define BINARY_TREE_REMOVE(root_ptr, node) stmt( \
    if ((node)->parent == NULL) {                \
      *(root_ptr) = NULL;                        \
    } else if ((node) == (node)->parent->left) { \
      (node)->parent->left = NULL;               \
    } else {                                     \
      (node)->parent->right = NULL;              \
    }(node)                                      \
        ->parent = NULL;)

#define BINARY_TREE_ROTATE_LEFT(root_ptr, node) stmt( \
    typeof((node)->right) _right = (node)->right;     \
    (node)->right = _right->left;                     \
    if (_right->left != NULL) {                       \
      _right->left->parent = (node);                  \
    } _right->parent = (node)->parent;                \
    if ((node)->parent == NULL) {                     \
      *(root_ptr) = _right;                           \
    } else if ((node) == (node)->parent->left) {      \
      (node)->parent->left = _right;                  \
    } else {                                          \
      (node)->parent->right = _right;                 \
    } _right->left = (node);                          \
    (node)->parent = _right;)

#define BINARY_TREE_ROTATE_RIGHT(root_ptr, node) stmt( \
    typeof((node)->left) _left = (node)->left;         \
    (node)->left = _left->right;                       \
    if (_left->right != NULL) {                        \
      _left->right->parent = (node);                   \
    } _left->parent = (node)->parent;                  \
    if ((node)->parent == NULL) {                      \
      *(root_ptr) = _left;                             \
    } else if ((node) == (node)->parent->left) {       \
      (node)->parent->left = _left;                    \
    } else {                                           \
      (node)->parent->right = _left;                   \
    } _left->right = (node);                           \
    (node)->parent = _left;)

// Typed traversal macros.
#define BINARY_TREE_FOREACH_PREORDER(root, it)          \
  safe_for (typeof(((root))) it = (root); (it) != NULL; \
            (it) = ({ typeof((root)) _binary_tree_next = NULL; BINARY_TREE_NEXT_PREORDER((root), (it), _binary_tree_next); _binary_tree_next; }))

#define BINARY_TREE_FOREACH_INORDER(root, it) \
  safe_for (typeof(((root))) it = ({ typeof((root)) _binary_tree_first = NULL; BINARY_TREE_FIRST_INORDER((root), _binary_tree_first); _binary_tree_first; });      \
            (it) != NULL;                     \
            (it) = ({ typeof((root)) _binary_tree_next = NULL; BINARY_TREE_NEXT_INORDER((root), (it), _binary_tree_next); _binary_tree_next; }))

#define BINARY_TREE_FOREACH_POSTORDER(root, it) \
  safe_for (typeof(((root))) it = ({ typeof((root)) _binary_tree_first = NULL; BINARY_TREE_FIRST_POSTORDER((root), _binary_tree_first); _binary_tree_first; });        \
            (it) != NULL;                       \
            (it) = ({ typeof((root)) _binary_tree_next = NULL; BINARY_TREE_NEXT_POSTORDER((root), (it), _binary_tree_next); _binary_tree_next; }))

// =========================================================================
c_end;
// =========================================================================
