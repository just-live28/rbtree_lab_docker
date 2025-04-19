#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  // 트리 구조체 동적 할당 및 0으로 초기화
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  
  // sentinel(nil) 노드 생성 및 초기화
  node_t *nil = malloc(sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->key = 0;
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  // 트리의 nil 및 root 설정
  p->nil = nil;
  p->root = nil;

  if (p == NULL || nil == NULL) {
    free(p); // p가 할당됐으면 해제
    free(nil); // nil도 해제
    return NULL;
  }
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

void color_flip(node_t *node, node_t *nil) {
  if (node == nil || node->left == nil || node->right == nil) return;

  if (node->color == RBTREE_BLACK) {
    node->color = RBTREE_RED;
    node->left->color = node->right->color = RBTREE_BLACK;
  } else {
    node->color = RBTREE_BLACK;
    node->left->color = node->right->color = RBTREE_RED;
  }
}

void color_swap(node_t * node, node_t *nil) {
  if (node == nil || node->parent == nil) return;

  int tmp = node->color;
  node->color = node->parent->color;
  node->parent->color = tmp;
}

void insert_fixup(rbtree *t, node_t *node) {
  node_t *cur = node;
  while (cur->parent->color == RBTREE_RED) {
    node_t *uncle;
    if (cur->parent == cur->parent->parent->left) {
      uncle = cur->parent->right;
    } else {
      uncle = cur->parent->left;
    }

    // Case1
    if (uncle->color == RBTREE_RED) {
      color_flip(cur->parent->parent, t->nil);
      cur = cur->parent->parent;
      continue;
    }
    // Case 2
    if ((uncle == cur->parent->right) && (cur == cur->parent->right)) {
      cur = cur->parent;
      left_rotate(t, cur->parent);
    } else if (((uncle == cur->parent->left) && (cur == cur->parent->left))) {
      cur = cur->parent;
      right_rotate(t, cur->parent);
    }
    // Case 3
    color_swap(cur->parent, t->nil);
    if (cur == cur->parent->left) {
      right_rotate(t, cur->parent->parent);
    } else {
      left_rotate(t, cur->parent->parent);
    }
  }
  t->root->color = RBTREE_BLACK;
}


node_t *rbtree_insert(rbtree *t, const key_t key) {
  


  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
