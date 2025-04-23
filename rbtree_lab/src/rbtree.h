#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *t);
void delete_node(node_t *node, node_t *nil);

void left_rotate(rbtree *t, node_t *axis);
void right_rotate(rbtree *t, node_t *axis);
void color_flip(rbtree *t, node_t *node);
void color_swap(rbtree *t, node_t * node);

void insert_fixup(rbtree *t, node_t *node);
node_t *rbtree_insert(rbtree *t, const key_t);

node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);

void erase_fixup(rbtree *t, node_t *p);
int rbtree_erase(rbtree *t, node_t *p);

void inorder_fill(node_t *node, node_t *nil, key_t *arr, int *idx, const size_t n);
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t);

#endif  // _RBTREE_H_
