#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = calloc(1, sizeof(*p));
  if (!p) return NULL;

  node_t *nil = malloc(sizeof(*nil));
  if (!nil) {
    free(p); return NULL;
  }

  nil->color = RBTREE_BLACK;
  nil->key   = 0;
  nil->left = nil->right = nil->parent = nil;

  p->nil  = nil;
  p->root = nil;

  return p;
}


void delete_rbtree(rbtree *t) {
  if (t == NULL) return;

  delete_node(t->root, t->nil);
  free(t->nil);
  free(t);
}

void delete_node(node_t *node, node_t *nil) {
  if (node == nil) return;

  delete_node(node->left, nil);
  delete_node(node->right, nil);
  free(node);
}

void left_rotate(rbtree *t, node_t *axis){
  node_t *new_parent = axis->right;
  node_t *remain_child = axis->right->left;

  // 축 부모 노드의 자식 포인터 변경
  if (axis->parent == t->nil) {
    t->root = new_parent;
  } else if (axis->parent->left == axis) {
    axis->parent->left = new_parent;
  } else {
    axis->parent->right = new_parent;
  }

  // 자식 재배치
  new_parent->left = axis;
  new_parent->parent = axis->parent;
  axis->parent = new_parent;

  // 축의 오른쪽 자식 포인터 재설정
  axis->right = remain_child;
  if (remain_child != t->nil) {
    remain_child->parent = axis;
  }
}

void right_rotate(rbtree *t, node_t *axis){
  node_t *new_parent = axis->left;
  node_t *remain_child = axis->left->right;

  // 축 부모 노드의 자식 포인터 변경
  if (axis->parent == t->nil) {
    t->root = new_parent;
  } else if (axis->parent->left == axis) {
    axis->parent->left = new_parent;
  } else {
    axis->parent->right = new_parent;
  }

  // 자식 재배치
  new_parent->right = axis;
  new_parent->parent = axis->parent;
  axis->parent = new_parent;

  // 축의 왼쪽 자식 포인터 재설정
  axis->left = remain_child;
  if (remain_child != t->nil) {
    remain_child->parent = axis;
  }
}

void color_flip(rbtree *t, node_t *node) {
  if (node == t->nil || node->left == t->nil || node->right == t->nil) return;

  if (node->color == RBTREE_BLACK) {
    node->color = RBTREE_RED;
    node->left->color = node->right->color = RBTREE_BLACK;
  } else {
    node->color = RBTREE_BLACK;
    node->left->color = node->right->color = RBTREE_RED;
  }
}

void color_swap(rbtree *t, node_t *node) {
  if (node == t->nil || node->parent == t->nil) return;

  int tmp = node->color;
  node->color = node->parent->color;
  node->parent->color = tmp;
}

void insert_fixup(rbtree *t, node_t *node) {
  node_t *cur = node;
  
  while (cur->parent != t->nil && cur->parent->color == RBTREE_RED) {
    if (cur->parent->parent == t->nil) break;
    node_t *grandparent = cur->parent->parent;
    
    // 부모가 할아버지의 왼쪽 자식인 경우
    if (cur->parent == grandparent->left) {
        node_t *uncle = grandparent->right;
        
        // Case 1: 삼촌이 RED
        if (uncle->color == RBTREE_RED) {
            cur->parent->color    = RBTREE_BLACK;
            uncle->color          = RBTREE_BLACK;
            grandparent->color    = RBTREE_RED;
            cur = grandparent;
            continue;
        }
        
        // Case 2: 삼촌이 BLACK이고 현재 노드가 부모의 오른쪽 자식
        if (cur == cur->parent->right) {
            cur = cur->parent;
            left_rotate(t, cur);
        }
        
        // Case 3: 삼촌이 BLACK이고 현재 노드가 부모의 왼쪽 자식
        cur->parent->color    = RBTREE_BLACK;
        grandparent->color    = RBTREE_RED;
        right_rotate(t, grandparent);
      }
      // 부모가 할아버지의 오른쪽 자식인 경우 (대칭)
      else {
          node_t *uncle = grandparent->left;
          
          // Case 1: 삼촌이 RED
          if (uncle->color == RBTREE_RED) {
              cur->parent->color    = RBTREE_BLACK;
              uncle->color          = RBTREE_BLACK;
              grandparent->color    = RBTREE_RED;
              cur = grandparent;
              continue;
          }
          
          // Case 2: 삼촌이 BLACK이고 현재 노드가 부모의 왼쪽 자식
          if (cur == cur->parent->left) {
              cur = cur->parent;
              right_rotate(t, cur);
          }
          
          // Case 3: 삼촌이 BLACK이고 현재 노드가 부모의 오른쪽 자식
          cur->parent->color    = RBTREE_BLACK;
          grandparent->color    = RBTREE_RED;
          left_rotate(t, grandparent);
      }
  }
  
  // 언제나 루트는 BLACK
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  if (t == NULL) return NULL;
  
  node_t *new_node = malloc(sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->parent = t->nil;
  new_node->left = t->nil;
  new_node->right = t->nil;

  node_t *parent = t->nil;
  node_t *cur = t->root;
  while (cur != t->nil) {
    parent = cur;
    if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  new_node->parent = parent;
  if (parent == t->nil) {
    t->root = new_node;
  } else if (new_node->key < parent->key) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }

  insert_fixup(t, new_node);
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;
  while (cur != t->nil) {
    if (key == cur->key) {
      return cur;
    } else if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return t->nil;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *cur = t->root;
  while (cur->left != t->nil) {
    cur = cur->left;
  }
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *cur = t->root;
  while (cur->right != t->nil) {
    cur = cur->right;
  }
  return cur;
}

// 부모-자식 포인터를 v로 대체
void transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left  = v;
  else
    u->parent->right = v;
  
  if (v != t->nil) {
    v->parent = u->parent;
  }
}

// 삭제 후 RB트리의 규칙을 복구하는 함수
// p는 삭제된 노드 자리를 대신하게 된 fix_target 노드
void erase_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      node_t *w = x->parent->right;
      // Case 1: 형제가 RED
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // Case 2: 형제의 양쪽 자식이 모두 BLACK
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // Case 3: 형제의 오른쪽 자식이 BLACK
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // Case 4: 형제의 오른쪽 자식이 RED
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    } else {
      // x가 오른쪽 자식인 경우: 왼쪽 형제에 대해 대칭 처리
      node_t *w = x->parent->left;
      // Case 1: 형제가 RED
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // Case 2: 형제의 양쪽 자식이 모두 BLACK
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // Case 3: 형제의 왼쪽 자식이 BLACK
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // Case 4: 형제의 왼쪽 자식이 RED
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  // 마지막으로 x를 BLACK으로
  x->color = RBTREE_BLACK;
}


int rbtree_erase(rbtree *t, node_t *p) {
  if (!t || !p || p == t->nil) return -1;

  node_t *y = p;  // 트리에서 제거될 노드
  node_t *x;      // y의 자리를 대체할 노드
  color_t y_original_color = y->color;

  if (p->left == t->nil) {
    x = p->right;
    transplant(t, p, p->right);
  } else if (p->right == t->nil) {
    x = p->left;
    transplant(t, p, p->left);
  } else {
    y = p->right;
    while (y->left != t->nil) {
      y = y->left;
    }

    y_original_color = y->color;
    x = y->right;

    if (y->parent == p) {
      x->parent = y;
    } else {
      transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }

    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if (y_original_color == RBTREE_BLACK) {
      erase_fixup(t, x);
  }

  free(p);
  return 0;
}

void inorder_fill(node_t *node, node_t *nil, key_t *arr, int *idx, const size_t n) {
  if (node == nil || *idx >= n) return;

  inorder_fill(node->left, nil, arr, idx, n);
  arr[(*idx)++] = node->key;
  inorder_fill(node->right, nil, arr, idx, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t == NULL) return -1;

  int idx = 0;
  inorder_fill(t->root, t->nil, arr, &idx, n);

  return idx;
}
