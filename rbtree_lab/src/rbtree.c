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
        return cur;
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

// 삭제 후 RB트리의 규칙을 복구하는 함수
// p는 삭제된 노드 자리를 대신하게 된 fix_target 노드
void erase_fixup(rbtree *t, node_t *p) {
  node_t *cur = p;

  // 현재 노드가 루트가 아니고, BLACK일 경우 재조정
  while (cur != t->root && cur->color == RBTREE_BLACK) {
    node_t *sibling;

    // 현재 노드가 왼쪽 자식인지 오른쪽 자식인지에 따라 형제 위치 결정
    if (cur == cur->parent->left) {
      sibling = cur->parent->right;
    } else {
      sibling = cur->parent->left;
    }

    // Case 1: 형제가 RED인 경우
    // → 부모-형제 색상 교환 후 회전하여 Case 2~4로 변환
    if (sibling->color == RBTREE_RED) {
      color_swap(t, sibling);  // 형제와 부모 색상 교환
      if (cur == cur->parent->left) {
        left_rotate(t, cur->parent);  // 왼쪽 자식이면 좌회전
      } else {
        right_rotate(t, cur->parent); // 오른쪽 자식이면 우회전
      }
      continue;  // 회전 후 형제가 BLACK이 되어 Case 2~4로 넘어감
    }

    // Case 2: 형제와 형제의 두 자식이 모두 BLACK인 경우
    // → 형제를 RED로 바꾸고, 부모로 올라가 다시 검사
    if ((sibling->left->color == RBTREE_BLACK) && (sibling->right->color == RBTREE_BLACK)) {
      sibling->color = RBTREE_RED;
      if (cur->parent->color == RBTREE_RED) {
        cur->parent->color = RBTREE_BLACK;  // 부모가 RED였다면 BLACK으로 바꾸고 종료
        break;
      } else {
        cur = cur->parent;  // 부모가 BLACK이면 다시 재조정
        continue;
      }
    }

    // Case 3: 형제가 BLACK이고, 형제의 자식 RED가 할아버지까지 꺾인 상태
    // → 형제와 자식 RED 색상 교환 후 회전 → Case 4로 변환
    if ((cur == cur->parent->left) && (sibling->left->color == RBTREE_RED)) {
      color_swap(t, sibling->left);
      right_rotate(t, sibling);
      sibling = cur->parent->right;  // 회전 후 형제가 바뀔 수 있으므로 갱신
    } else if ((cur == cur->parent->right) && (sibling->right->color == RBTREE_RED)) {
      color_swap(t, sibling->right);
      left_rotate(t, sibling);
      sibling = cur->parent->left;
    }

    // Case 4: 형제가 BLACK이고, 형제의 자식 RED가 할아버지까지 일자인 상태
    // → 형제의 색을 할아버지의 색으로 덮고, 할아버지와 형제 자식 Red는 BLACK으로
    // → 할아버지를 축으로 회전하면 fixup 종료
    sibling->color = cur->parent->color;
    if ((cur == cur->parent->left) && (sibling->right->color == RBTREE_RED)) {
      cur->parent->color = sibling->right->color = RBTREE_BLACK;
      left_rotate(t, cur->parent);
    } else if ((cur == cur->parent->right) && (sibling->left->color == RBTREE_RED)) {
      cur->parent->color = sibling->left->color = RBTREE_BLACK;
      right_rotate(t, cur->parent);
    }
    break;  // fixup 완료
  }

  // 루트 노드는 항상 BLACK으로 보장
  t->root->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // 트리 또는 삭제할 노드가 NULL인 경우 -1을 return
  if (t == NULL || p == NULL) return -1; 

  node_t *cur = p;           // 현재 삭제 대상 노드
  node_t *fix_target;        // 삭제 후 색을 물려받아 fixup 대상이 되는 노드
  color_t del_color;         // 실제 삭제된 노드의 색을 저장

  // Case 1: 자식이 없는 노드를 삭제하는 경우
  if ((cur->left == t->nil) && (cur->right == t->nil)) {
    del_color = cur->color;

    // 부모와의 연결을 끊고, 삭제 노드를 nil로 대체
    if (cur == cur->parent->left) {
      cur->parent->left = t->nil;
    } else {
      cur->parent->right = t->nil;
    }

    // 삭제할 노드가 루트인 경우 nil이 새 루트가 됨
    if (cur == t->root) {
      t->root = t->nil;
    }

    fix_target = t->nil; 			// fixup 대상은 nil 노드
    t->nil->parent = cur->parent; 	// fixup을 위해 nil 노드에 임시로 부모 설정

    free(cur); // 삭제 대상 메모리 해제
  }

  // Case 2: 자식이 1개인 노드를 삭제하는 경우
  else if ((cur->left == t->nil) || (cur->right == t->nil)) {
    // 삭제 노드의 유일한 자식이 fixup 대상이 됨
    if (cur->left != t->nil) {
      fix_target = cur->left;
    } else {
      fix_target = cur->right;
    }

    del_color = cur->color;

    // 부모가 삭제 노드 대신 그 자식을 가리키도록 연결
    if (cur == cur->parent->left) {
      cur->parent->left = fix_target;
    } else {
      cur->parent->right = fix_target;
    }

    // 삭제 노드의 자식 노드 부모 포인터도 갱신
    fix_target->parent = cur->parent;

    // 루트 노드를 삭제하는 경우 자식 노드를 새 루트로 설정
    if (cur == t->root) {
      t->root = fix_target;
    }

    free(cur); // 삭제 노드 메모리 해제
  }

  // Case 3: 자식이 2개인 노드를 삭제하는 경우
  else {
    // 오른쪽 서브트리에서 최소값(후임자)을 찾아 삭제
    node_t *del_target = cur->right;
    while (del_target->left != t->nil) {
      del_target = del_target->left;
    }

    del_color = del_target->color; 	// 실제로 삭제되는 후임자의 색 저장
    cur->key = del_target->key; 	// 삭제할 노드의 key를 후임자의 key로 교체

    // Case 3-1: 후임자의 자식이 없는 경우
    if ((del_target->left == t->nil) && (del_target->right == t->nil)) {
      fix_target = t->nil;						// fixup 대상은 nil 노드
      t->nil->parent = del_target->parent;		// fixup을 위해 nil 노드에 임시로 부모 설정

      // 부모와의 방향을 확인하여 nil로 교체
      if (del_target == del_target->parent->left) {
        del_target->parent->left = t->nil;
      } else {
        del_target->parent->right = t->nil;
      }

      free(del_target);	// 후임자 메모리 해제
    }

    // Case 3-2: 후임자가 오른쪽 자식을 가진 경우
    else {
      fix_target = del_target->right;

      if (del_target == del_target->parent->left) {
        del_target->parent->left = fix_target;
      } else {
        del_target->parent->right = fix_target;
      }

      fix_target->parent = del_target->parent;

      free(del_target);	// 후임자 메모리 해제
    }
  }

  // === 색상에 따라 fixup 여부 판단 ===

  // 삭제된 노드가 Black이고, 자식이 Red였다면 Red and Black 상태가 됨
  // 색만 Black으로 바꿔주고 재조정 생략
  if ((fix_target->color == RBTREE_RED) && (del_color == RBTREE_BLACK)) {
    fix_target->color = RBTREE_BLACK;
  }

  // 삭제된 노드가 Black이고, fix_target도 Black이면 Doubly Black 상태가 됨
  // Doubly Black을 해결하기 위해 재조정 시작
  else if ((fix_target->color == RBTREE_BLACK) && (del_color == RBTREE_BLACK)) {
    erase_fixup(t, fix_target);
  }

  return 0; // 정상적으로 삭제 완료
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
