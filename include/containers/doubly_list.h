// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// A doubly_list is an intrusive doubly-linked list with head and tail pointers.
// Nodes must embed both `next` and `prev` pointers of the same node type.
// All operations except COUNT are O(1).

// DOUBLY_LIST_EMPTY checks if the list is empty.
#define DOUBLY_LIST_EMPTY(head, tail) expression((head) == nullptr)

// DOUBLY_LIST_COUNT counts the number of nodes by traversing from head to end.
#define DOUBLY_LIST_COUNT(head, tail, count) expression({ \
  (count) = 0;                                            \
  auto _node = (head);                                    \
  while (_node != nullptr) {                              \
    (count)++;                                            \
    _node = _node->next;                                  \
  }                                                       \
})

// DOUBLY_LIST_HEAD returns the head node of the list.
#define DOUBLY_LIST_HEAD(head, tail) expression((head))

// DOUBLY_LIST_TAIL returns the tail node of the list.
#define DOUBLY_LIST_TAIL(head, tail) expression((tail))

// DOUBLY_LIST_PUSH_FRONT adds a node to the front of the list.
#define DOUBLY_LIST_PUSH_FRONT(head, tail, node) expression({ \
  (node)->prev = nullptr;                                     \
  (node)->next = (head);                                      \
  if ((head) != nullptr) (head)->prev = (node);               \
  else                                                        \
    (tail) = (node);                                          \
  (head) = (node);                                            \
})

// DOUBLY_LIST_PUSH_BACK adds a node to the back of the list.
#define DOUBLY_LIST_PUSH_BACK(head, tail, node) expression({ \
  (node)->next = nullptr;                                    \
  (node)->prev = (tail);                                     \
  if ((tail) != nullptr) (tail)->next = (node);              \
  else                                                       \
    (head) = (node);                                         \
  (tail) = (node);                                           \
})

// DOUBLY_LIST_POP_FRONT removes the head node from the list and returns it.
#define DOUBLY_LIST_POP_FRONT(head, tail, node) expression({ \
  (node) = (head);                                           \
  if ((head) != nullptr) {                                   \
    (head) = (head)->next;                                   \
    if ((head) != nullptr) (head)->prev = nullptr;           \
    else                                                     \
      (tail) = nullptr;                                      \
    (node)->next = nullptr;                                  \
  }                                                          \
})

// DOUBLY_LIST_POP_BACK removes the tail node from the list and returns it.
#define DOUBLY_LIST_POP_BACK(head, tail, node) expression({ \
  (node) = (tail);                                          \
  if ((tail) != nullptr) {                                  \
    (tail) = (tail)->prev;                                  \
    if ((tail) != nullptr) (tail)->next = nullptr;          \
    else                                                    \
      (head) = nullptr;                                     \
    (node)->prev = nullptr;                                 \
  }                                                         \
})

// DOUBLY_LIST_REMOVE removes an arbitrary node from the list.
#define DOUBLY_LIST_REMOVE(head, tail, node) expression({         \
  if ((node)->prev != nullptr) (node)->prev->next = (node)->next; \
  else                                                            \
    (head) = (node)->next;                                        \
  if ((node)->next != nullptr) (node)->next->prev = (node)->prev; \
  else                                                            \
    (tail) = (node)->prev;                                        \
  (node)->next = nullptr;                                         \
  (node)->prev = nullptr;                                         \
})

// DOUBLY_LIST_INSERT_AFTER inserts a node immediately after a given node.
#define DOUBLY_LIST_INSERT_AFTER(head, tail, after, node) expression({ \
  (node)->prev = (after);                                              \
  (node)->next = (after)->next;                                        \
  if ((after)->next != nullptr) (after)->next->prev = (node);          \
  else                                                                 \
    (tail) = (node);                                                   \
  (after)->next = (node);                                              \
})

// DOUBLY_LIST_INSERT_BEFORE inserts a node immediately before a given node.
#define DOUBLY_LIST_INSERT_BEFORE(head, tail, before, node) expression({ \
  (node)->next = (before);                                               \
  (node)->prev = (before)->prev;                                         \
  if ((before)->prev != nullptr) (before)->prev->next = (node);          \
  else                                                                   \
    (head) = (node);                                                     \
  (before)->prev = (node);                                               \
})

// DOUBLY_LIST_FOREACH iterates forward from head to tail.
// 'it' is declared as the loop variable; its type is deduced from 'head'.
#define DOUBLY_LIST_FOREACH(head, tail, it) \
  for (auto it = (head); (it) != nullptr; (it) = (it)->next)

// DOUBLY_LIST_FOREACH_REVERSE iterates backward from tail to head.
// 'it' is declared as the loop variable; its type is deduced from 'tail'.
#define DOUBLY_LIST_FOREACH_REVERSE(head, tail, it) \
  for (auto it = (tail); (it) != nullptr; (it) = (it)->prev)
