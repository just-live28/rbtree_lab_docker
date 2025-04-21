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
    free(p);
    free(nil);
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
  while (cur->parent->color == RBTREE_RED) {
    node_t *uncle;
    if (cur->parent == cur->parent->parent->left) {
      uncle = cur->parent->right;
    } else {
      uncle = cur->parent->left;
    }

    // Case1
    if (uncle->color == RBTREE_RED) {
      color_flip(t, cur->parent->parent);
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
    color_swap(t, cur->parent);
    if (cur == cur->parent->left) {
      right_rotate(t, cur->parent->parent);
    } else {
      left_rotate(t, cur->parent->parent);
    }
  }
  cur->color = RBTREE_BLACK;
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

  if (t->root == t->nil) {
    new_node->color = RBTREE_BLACK;
    t->root = new_node;
  } else {
    node_t *parent, *cur;
    cur = t->root;
    while (cur != t->nil) {
      if (key == cur->key) {
        free(new_node);
        return t->root;
      } else if (key < cur->key) {
        parent = cur;
        cur = cur->left;
      } else {
        parent = cur;
        cur = cur->right;
      }
    }
    if (new_node->key < parent->key) {
      parent->left = new_node;
    } else {
      parent->right = new_node;
    }
    new_node->parent = parent;
  }
  insert_fixup(t, new_node);

  return t->root;
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

void erase_fixup(rbtree *t, node_t *p) {
  node_t *cur = p;
  while (cur != t->root && cur->color == RBTREE_BLACK) {
    node_t *sibling;
    if (cur == cur->parent->left) {
      sibling = cur->parent->right;
    } else {
      sibling = cur->parent->left;
    }
    // Case 1
    if (sibling->color == RBTREE_RED) {
      color_swap(t, sibling);
      if (cur == cur->parent->left) {
        left_rotate(t, cur->parent);
      } else {
        right_rotate(t, cur->parent);
      }
      continue;
    }
    // Case 2
    if ((sibling->left->color == RBTREE_BLACK) && (sibling->right->color == RBTREE_BLACK)) {
      sibling->color = RBTREE_RED;
      if (cur->parent->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;
        break;
      } else {
        cur = cur->parent;
        continue;
      }
    }
    // Case 3
    if ((cur == cur->parent->left) && (sibling->left->color == RBTREE_RED)) {
      color_swap(t, sibling->left);
      right_rotate(t, sibling);
      sibling = cur->parent->right;
    } else if (((cur == cur->parent->right) && (sibling->right->color == RBTREE_RED))) {
      color_swap(t, sibling->right);
      left_rotate(t, sibling);
      sibling = cur->parent->left;
    }
    // Case 4
    sibling->color = cur->parent->color;
    if ((cur == cur->parent->left) && (sibling->right->color == RBTREE_RED)) {
      cur->parent->color = sibling->right->color = RBTREE_BLACK;
      left_rotate(t, cur->parent);
    } else if ((cur == cur->parent->right) && (sibling->left->color == RBTREE_RED)) {
      cur->parent->color = sibling->left->color = RBTREE_BLACK;
      right_rotate(t, cur->parent);
    }
    break;
  }
  t->root->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  if (t == NULL || p == NULL) return -1;

  node_t *cur = p, *fix_target;
  color_t del_color;
  if ((cur->left == t->nil) && (cur->right == t->nil)) {
    // 자식이 없을 때
    del_color = cur->color;
    if (cur == cur->parent->left) {
      cur->parent->left = t->nil;
    } else {
      cur->parent->right = t->nil;
    }
    if (cur == t->root) {
      t->root = t->nil;
    }
    fix_target = cur->parent;
    free(cur);
  } else if (((cur->left == t->nil) || (cur->right == t->nil))) {
    // 자식이 1개일 때
    if (cur->left != t->nil) {
      fix_target = cur->left;
    } else {
      fix_target = cur->right;
    }

    del_color = cur->color;

    if (cur == cur->parent->left) {
      cur->parent->left = fix_target;
    } else {
      cur->parent->right = fix_target;
    }

    fix_target->parent = cur->parent;

    if (cur == t->root) {
      t->root = fix_target;
    }

    free(cur);
  } else {
    // 자식이 2개일 때
    node_t *del_target = cur->right;
    while (del_target->left != t->nil) {
      del_target = del_target->left;
    }
    del_color = del_target->color;
    cur->key = del_target->key;
    if ((del_target->left == t->nil) && (del_target->right == t->nil)) {
      fix_target = del_target->parent;
      if (del_target == fix_target->left) {
        fix_target->left = t->nil;
      } else {
        fix_target->right = t->nil;
      }
      free(del_target);
    } else {
      fix_target = del_target->right;
      if (del_target == del_target->parent->left) {
        del_target->parent->left = fix_target;
      } else {
        del_target->parent->right = fix_target;
      }
      fix_target->parent = del_target->parent;
      free(del_target);
    }
  }

  if (del_color == RBTREE_BLACK) {
    erase_fixup(t, fix_target);
  }

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
