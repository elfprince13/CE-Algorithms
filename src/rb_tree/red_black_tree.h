#ifndef RED_BLACK_TREE_H_
#define RED_BLACK_TREE_H_

#include "../util.h"
#include <stdbool.h>

/*  CONVENTIONS:  All data structures for red-black trees have the prefix */
/*                "rb_" to prevent name conflicts. */
/*                                                                      */
/*                Function names: Each word in a function name begins with */
/*                a capital letter.  An example funcntion name is  */
/*                CreateRedTree(a,b,c). Furthermore, each function name */
/*                should begin with a capital letter to easily distinguish */
/*                them from variables. */
/*                                                                     */
/*                Variable names: Each word in a variable name begins with */
/*                a capital letter EXCEPT the first letter of the variable */
/*                name.  For example, int newLongInt.  Global variables have */
/*                names beginning with "g".  An example of a global */
/*                variable name is gNewtonsConstant. */

#include <assert.h>

typedef struct rb_red_blk_node {
  void *key;
  int red; /* if red=0 then the node is black */
  struct rb_red_blk_node* left;
  struct rb_red_blk_node* right;
  struct rb_red_blk_node* parent;
} rb_red_blk_node;

rb_red_blk_node* RBNodeAlloc(void);

typedef struct rb_red_blk_map_node {
	rb_red_blk_node node;
	void* info;
} rb_red_blk_map_node;

rb_red_blk_node* RBMapNodeAlloc(void);

/* Compare(a,b) should return +Val if *a > *b, -Val if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
typedef struct rb_red_blk_tree {
  int (*Compare)(const void* a, const void* b); 
  void (*DestroyKey)(void* a);
	void (*DestroyNodeContents)(struct rb_red_blk_tree*, rb_red_blk_node*);
	rb_red_blk_node* (*AllocNode)(void); /* This allows us to "subclass" nodes. */
  /*  A sentinel is used for root and for sentinel.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  sentinel points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and sentinel nodes do not require special cases in the code */
  rb_red_blk_node* root;             
  rb_red_blk_node* sentinel;
	rb_red_blk_node* first;
	size_t size;
} rb_red_blk_tree;

typedef struct rb_red_blk_map_tree {
	rb_red_blk_tree tree;
	void (*DestroyInfo)(void* a);
} rb_red_blk_map_tree;

void RBTreeInit(rb_red_blk_tree*,
				int  (*CompFunc)(const void*, const void*),
				void (*DestFunc)(void*),
				rb_red_blk_node* (*AllocNode)(void));

void RBTreeMapInit(rb_red_blk_map_tree* ,
				   int  (*CompFunc)(const void*, const void*),
				   void (*DestFunc)(void*),
				   rb_red_blk_node* (*AllocNode)(void),
				   void (*InfoDestFunc)(void*));

rb_red_blk_node * RBTreeInsert(rb_red_blk_tree*, void* key);
void RBDelete(rb_red_blk_tree* , rb_red_blk_node* );
void RBTreeDestroy(rb_red_blk_tree*, bool ownTree);
void RBTreeClear(rb_red_blk_tree*);
rb_red_blk_node* TreePredecessor(const rb_red_blk_tree*,const rb_red_blk_node*);
rb_red_blk_node* TreeSuccessor(const rb_red_blk_tree*,const rb_red_blk_node*);
rb_red_blk_node* RBExactQuery(const rb_red_blk_tree*, const void*);

void* RBMapPut(rb_red_blk_map_tree *tree, void* key, void* value);
void* RBMapRemove(rb_red_blk_map_tree *tree, const void* key);

bool RBSetAdd(rb_red_blk_tree *tree, void* key);
bool RBSetRemove(rb_red_blk_tree *tree, const void* key);
bool RBSetContains(const rb_red_blk_tree *tree, const void* key);

#endif
