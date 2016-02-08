#ifndef RED_BLACK_TREE_H_
#define RED_BLACK_TREE_H_

#include "../util.h"
#include <stdbool.h>
#include <assert.h>

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

/***************************************************************************************
 * The base type for nodes that belong in a red-black tree,
 * and a friendly allocator
 ***************************************************************************************/
typedef struct rb_red_blk_node {
	void *key;
	int red; /* if red=0 then the node is black */
	struct rb_red_blk_node* left;
	struct rb_red_blk_node* right;
	struct rb_red_blk_node* parent;
} rb_red_blk_node;
rb_red_blk_node* RBNodeAlloc(void);

/***************************************************************************************
 * A "polymorphic" node, for use when you want a map to something,
 * and a friendly allocator
 ***************************************************************************************/
typedef struct rb_red_blk_map_node {
	/* The "super" */
	rb_red_blk_node node;
	
	/* The extra data */
	void* info;
} rb_red_blk_map_node;

rb_red_blk_node* RBMapNodeAlloc(void);

/***************************************************************************************
 * The container object for the nodes in a red-black tree.
 ***************************************************************************************/
typedef struct rb_red_blk_tree {
	/* Compare(a,b) should return +Val if *a > *b, -Val if *a < *b, and 0 otherwise */
	int (*Compare)(const void* a, const void* b);
	/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
	void (*DestroyKey)(void* a);
	/* Taking custom allocator + destructor
	 * allows us to "subclass nodes */
	void (*DestroyNodeContents)(struct rb_red_blk_tree*, rb_red_blk_node*);
	rb_red_blk_node* (*AllocNode)(void);
	
	/***************************************************************************************
	 * A sentinel is used for root and for sentinel.  These sentinels are *
	 *  created when RBTreeCreate is caled.  root->left should always *
	 *  point to the node which is the root of the tree.  sentinel points to a *
	 *  node which should always be black but has aribtrary children and *
	 *  parent and no key or info.  The point of using these sentinels is so *
	 *  that the root and sentinel nodes do not require special cases in the code
	 ***************************************************************************************/
	rb_red_blk_node* root;
	rb_red_blk_node* sentinel;
	
	/* Tracking the left-most node allows us to easily initiate an in-order traversal */
	rb_red_blk_node* first;
	/* For efficient size queries */
	size_t size;
} rb_red_blk_tree;

/***************************************************************************************
 * The "polymorphic" tree variant that also keeps a function to destroy node contents
 * for use with rb_red_blk_map_node
 ***************************************************************************************/
typedef struct rb_red_blk_map_tree {
	/* The "super" */
	rb_red_blk_tree tree;
	
	void (*DestroyInfo)(void* a);
} rb_red_blk_map_tree;

/***************************************************************************************
 * These functions serve as in-place constructors for the tree types previously defined
 ***************************************************************************************/
void RBTreeInit(rb_red_blk_tree*,
				int  (*CompFunc)(const void*, const void*),
				void (*DestFunc)(void*),
				rb_red_blk_node* (*AllocNode)(void));

void RBTreeMapInit(rb_red_blk_map_tree* ,
				   int  (*CompFunc)(const void*, const void*),
				   void (*DestFunc)(void*),
				   rb_red_blk_node* (*AllocNode)(void),
				   void (*InfoDestFunc)(void*));

/***************************************************************************************
 * Insert a node with key "key" and return a pointer to it. This is useful
 * if you're using the tree as a standard BST, but shouldn't not be used if you're
 * using them as a backing data structure with stricter requirements
 ***************************************************************************************/
rb_red_blk_node * RBTreeInsert(rb_red_blk_tree*, void* key);

/***************************************************************************************
 * Delete the node from the tree. This is useful
 * if you're using the tree as a standard BST, but shouldn't not be used if you're
 * using them as a backing data structure with stricter requirements
 ***************************************************************************************/
void RBDelete(rb_red_blk_tree* , rb_red_blk_node* );

/***************************************************************************************
 * This destroys the tree contents. When ownTree = true, also frees the tree itself
 ***************************************************************************************/
void RBTreeDestroy(rb_red_blk_tree*, bool ownTree);

/***************************************************************************************
 * This destroys the tree contents, and reinitializes the root and sentinel.
 ***************************************************************************************/
void RBTreeClear(rb_red_blk_tree*);

/***************************************************************************************
 * These functions provide bidirectional iteration.
 * tree->sentinel is returned if no more elements are available.
 ***************************************************************************************/
rb_red_blk_node* TreePredecessor(const rb_red_blk_tree*,const rb_red_blk_node*);
rb_red_blk_node* TreeSuccessor(const rb_red_blk_tree*,const rb_red_blk_node*);

/***************************************************************************************
 * This returns the top-most node with the matching key,
 * or NULL if no such node exists.
 ***************************************************************************************/
rb_red_blk_node* RBExactQuery(const rb_red_blk_tree*, const void*);

/***************************************************************************************
 * This maps key to value.
 * If key was previously mapped, the previously mapped value is returned
 * otherwise, NULL is returned
 ***************************************************************************************/
void* RBMapPut(rb_red_blk_map_tree *tree, void* key, void* value);

/***************************************************************************************
 * This unmaps key.
 * If key was previosuly mapped, the previously mapped value is returned
 * otherwise, NULL is returned
 ***************************************************************************************/
void* RBMapRemove(rb_red_blk_map_tree *tree, const void* key);

/***************************************************************************************
 * Updates the set to contain key.
 * Returns true if the key is a new element, false otherwise
 ***************************************************************************************/
bool RBSetAdd(rb_red_blk_tree *tree, void* key);

/***************************************************************************************
 * Updates the set to not contain key.
 * Returns true if the key was present, false otherwise
 ***************************************************************************************/
bool RBSetRemove(rb_red_blk_tree *tree, const void* key);

/***************************************************************************************
 * Returns true if the key was present, false otherwise
 ***************************************************************************************/
bool RBSetContains(const rb_red_blk_tree *tree, const void* key);

#endif
