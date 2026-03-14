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
TREE_* manages an intrusive general tree with sibling links.
Each node must provide `parent`, `first_child`, `last_child`,
`next_sibling`, and `prev_sibling` members.

Example:

  typedef struct scene_node {
    struct scene_node* parent;
    struct scene_node* first_child;
    struct scene_node* last_child;
    struct scene_node* next_sibling;
    struct scene_node* prev_sibling;
    u64 id;
  } scene_node;
*/

// Structural predicates.
#define TREE_IS_ROOT(node) ((node)->parent == NULL)
#define TREE_IS_LEAF(node) ((node)->first_child == NULL)

// Traversal helpers.
#define TREE_NEXT_PREORDER(root, node, out) stmt(                                       \
    type_of((root)) _tree_root = (root);                                                \
    type_of((node)) _tree_cursor = (node);                                              \
    (out) = NULL;                                                                       \
    if (_tree_root != NULL && _tree_cursor != NULL) {                                   \
      if (_tree_cursor->first_child != NULL) {                                          \
        (out) = _tree_cursor->first_child;                                              \
      } else {                                                                          \
        safe_while (_tree_cursor != _tree_root && _tree_cursor->next_sibling == NULL) { \
          _tree_cursor = _tree_cursor->parent;                                          \
        }                                                                               \
        if (_tree_cursor != _tree_root) {                                               \
          (out) = _tree_cursor->next_sibling;                                           \
        }                                                                               \
      }                                                                                 \
    })

// Mutation helpers.
#define TREE_INSERT_CHILD_FRONT(parent, node) stmt( \
    (node)->parent = (parent);                      \
    (node)->prev_sibling = NULL;                    \
    (node)->next_sibling = (parent)->first_child;   \
    if ((parent)->first_child != NULL) {            \
      (parent)->first_child->prev_sibling = (node); \
    } else {                                        \
      (parent)->last_child = (node);                \
    }(parent)                                       \
        ->first_child = (node);)

#define TREE_INSERT_CHILD_BACK(parent, node) stmt( \
    (node)->parent = (parent);                     \
    (node)->next_sibling = NULL;                   \
    (node)->prev_sibling = (parent)->last_child;   \
    if ((parent)->last_child != NULL) {            \
      (parent)->last_child->next_sibling = (node); \
    } else {                                       \
      (parent)->first_child = (node);              \
    }(parent)                                      \
        ->last_child = (node);)

#define TREE_INSERT_BEFORE(before, node) stmt(       \
    (node)->parent = (before)->parent;               \
    (node)->next_sibling = (before);                 \
    (node)->prev_sibling = (before)->prev_sibling;   \
    if ((before)->prev_sibling != NULL) {            \
      (before)->prev_sibling->next_sibling = (node); \
    } else if ((before)->parent != NULL) {           \
      (before)->parent->first_child = (node);        \
    }(before)                                        \
        ->prev_sibling = (node);)

#define TREE_INSERT_AFTER(after, node) stmt(        \
    (node)->parent = (after)->parent;               \
    (node)->prev_sibling = (after);                 \
    (node)->next_sibling = (after)->next_sibling;   \
    if ((after)->next_sibling != NULL) {            \
      (after)->next_sibling->prev_sibling = (node); \
    } else if ((after)->parent != NULL) {           \
      (after)->parent->last_child = (node);         \
    }(after)                                        \
        ->next_sibling = (node);)

#define TREE_REMOVE(node) stmt(                                  \
    if ((node)->prev_sibling != NULL) {                          \
      (node)->prev_sibling->next_sibling = (node)->next_sibling; \
    } else if ((node)->parent != NULL) {                         \
      (node)->parent->first_child = (node)->next_sibling;        \
    } if ((node)->next_sibling != NULL) {                        \
      (node)->next_sibling->prev_sibling = (node)->prev_sibling; \
    } else if ((node)->parent != NULL) {                         \
      (node)->parent->last_child = (node)->prev_sibling;         \
    }(node)                                                      \
        ->parent = NULL;                                         \
    (node)->prev_sibling = NULL;                                 \
    (node)->next_sibling = NULL;)

// Typed traversal macros.
#define TREE_FOREACH_CHILDREN(parent, it) \
  safe_for (type_of(((parent)->first_child)) it = (parent)->first_child; (it) != NULL; (it) = (it)->next_sibling)

#define TREE_FOREACH_CHILDREN_REVERSE(parent, it) \
  safe_for (type_of(((parent)->last_child)) it = (parent)->last_child; (it) != NULL; (it) = (it)->prev_sibling)

#define TREE_FOREACH_PREORDER(root, it)                  \
  safe_for (type_of(((root))) it = (root); (it) != NULL; \
            (it) = ({ type_of((root)) _tree_next = NULL; TREE_NEXT_PREORDER((root), (it), _tree_next); _tree_next; }))

// =========================================================================
c_end;
// =========================================================================
