#include "red_black_tree.h"
#include <stdlib.h>
#include <stddef.h>

/***************************************************************************************
 *
 * These are forward declarations of helper functions, and not part of the public API
 *
 ***************************************************************************************/

/* Subroutines used to delete a node and make sure it's cleaned up properly */
static void RBTreeNodeDestroy(rb_red_blk_tree* tree, rb_red_blk_node* target);
static void RBMapTreeNodeDestroy(rb_red_blk_map_tree* tree, rb_red_blk_map_node* target);
/* Subroutine used by both init and clear */
static void RBPrimeRoot(rb_red_blk_tree * tree);
/***********************************************************************/
/*  FUNCTION:  LeftRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and sentinel pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input: tree, x */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/
static void LeftRotate(rb_red_blk_tree* tree, rb_red_blk_node* x);
/***********************************************************************/
/*  FUNCTION:  RightRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and sentinel pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input?: tree, y */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/
static void RightRotate(rb_red_blk_tree* tree, rb_red_blk_node* y);
/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
/**/
/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
/**/
/*  OUTPUT:  none */
/**/
/*  Modifies Input:  tree, z */
/**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the RBTreeInsert function and not by the user */
/***********************************************************************/
static void TreeInsertHelp(rb_red_blk_tree* tree, rb_red_blk_node* z);
/***********************************************************************/
/*  FUNCTION:  TreeDestHelper */
/**/
/*    INPUTS:  tree is the tree to destroy and x is the current node */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively destroys the nodes of the tree */
/*              postorder using the DestroyKey and DestroyInfo functions. */
/**/
/*    Modifies Input: tree, x */
/**/
/*    Note:    This function should only be called by RBTreeDestroy */
/***********************************************************************/
static void TreeDestHelper(rb_red_blk_tree* tree, rb_red_blk_node* x);
/***********************************************************************/
/*  FUNCTION:  RBDeleteFixUp */
/**/
/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
/*             out node in RBTreeDelete. */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
/**/
/*    Modifies Input: tree, x */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/
static void RBDeleteFixUp(rb_red_blk_tree* tree, rb_red_blk_node* x);

/***************************************************************************************
 *
 * The functions that follow are part of the public API
 *
 ***************************************************************************************/

rb_red_blk_node* RBNodeAlloc() { return (rb_red_blk_node*)SafeMalloc(sizeof(rb_red_blk_node)); }
rb_red_blk_node* RBMapNodeAlloc() {	return (rb_red_blk_node*)SafeMalloc(sizeof(rb_red_blk_map_node)); }

void RBTreeInit(rb_red_blk_tree* newTree,
				int  (*CompFunc)(const void*, const void*),
				void (*DestFunc)(void*),
				rb_red_blk_node* (*AllocNode)(void)) {
	newTree->Compare=  CompFunc;
	newTree->DestroyKey= DestFunc;
	newTree->AllocNode = AllocNode;
	newTree->DestroyNodeContents = &RBTreeNodeDestroy;
	
	newTree->sentinel= AllocNode();
	newTree->root= AllocNode();
	RBPrimeRoot(newTree);
}

void RBTreeMapInit(rb_red_blk_map_tree* newTree,
				   int  (*CompFunc)(const void*, const void*),
				   void (*DestFunc)(void*),
				   rb_red_blk_node* (*AllocNode)(void),
				   void (*InfoDestFunc)(void*)){
	RBTreeInit((rb_red_blk_tree*)newTree, CompFunc, DestFunc, AllocNode);
	newTree->DestroyInfo = InfoDestFunc;
	newTree->tree.DestroyNodeContents = (void (*)(rb_red_blk_tree*, rb_red_blk_node*))(&RBMapTreeNodeDestroy);
	
}

rb_red_blk_node * RBTreeInsert(rb_red_blk_tree* tree, void* key) {
	rb_red_blk_node * y;
	rb_red_blk_node * x;
	rb_red_blk_node * newNode;
	
	++(tree->size);
	x=tree->AllocNode();
	x->key=key;
	
	TreeInsertHelp(tree,x);
	newNode=x;
	x->red=1;
	while(x->parent->red) { /* use sentinel instead of checking for root */
		if (x->parent == x->parent->parent->left) {
			y=x->parent->parent->right;
			if (y->red) {
				x->parent->red=0;
				y->red=0;
				x->parent->parent->red=1;
				x=x->parent->parent;
			} else {
				if (x == x->parent->right) {
					x=x->parent;
					LeftRotate(tree,x);
				}
				x->parent->red=0;
				x->parent->parent->red=1;
				RightRotate(tree,x->parent->parent);
			}
		} else { /* case for x->parent == x->parent->parent->right */
			y=x->parent->parent->left;
			if (y->red) {
				x->parent->red=0;
				y->red=0;
				x->parent->parent->red=1;
				x=x->parent->parent;
			} else {
				if (x == x->parent->left) {
					x=x->parent;
					RightRotate(tree,x);
				}
				x->parent->red=0;
				x->parent->parent->red=1;
				LeftRotate(tree,x->parent->parent);
			}
		}
	}
	tree->root->left->red=0;
	if(tree->first != tree->sentinel){
		rb_red_blk_node *const pred = TreePredecessor(tree, tree->first);
		if(pred != tree->sentinel){
			tree->first = pred;
		}
	} else {
		tree->first = newNode;
	}
	assert(!tree->sentinel->red && "sentinel not red in RBTreeInsert");
	assert(!tree->root->red && "root not red in RBTreeInsert");
	return(newNode);
}

rb_red_blk_node* TreeSuccessor(const rb_red_blk_tree* tree,const rb_red_blk_node* x) {
	rb_red_blk_node* y;
	rb_red_blk_node* sentinel=tree->sentinel;
	rb_red_blk_node* root=tree->root;
	
	if (sentinel != (y = x->right)) { /* assignment to y is intentional */
		while(y->left != sentinel) { /* returns the minium of the right subtree of x */
			y=y->left;
		}
		return(y);
	} else {
		y=x->parent;
		while(x == y->right) { /* sentinel used instead of checking for sentinel */
			x=y;
			y=y->parent;
		}
		if (y == root) return(sentinel);
		return(y);
	}
}

rb_red_blk_node* TreePredecessor(const rb_red_blk_tree* tree, const rb_red_blk_node* x) {
	rb_red_blk_node* y;
	rb_red_blk_node* sentinel=tree->sentinel;
	rb_red_blk_node* root=tree->root;
	
	if (sentinel != (y = x->left)) { /* assignment to y is intentional */
		while(y->right != sentinel) { /* returns the maximum of the left subtree of x */
			y=y->right;
		}
		return(y);
	} else {
		y=x->parent;
		while(x == y->left) {
			if (y == root) return(sentinel);
			x=y;
			y=y->parent;
		}
		return(y);
	}
}

void RBTreeDestroy(rb_red_blk_tree* tree, bool ownTree) {
	TreeDestHelper(tree,tree->root->left);
	free(tree->root);
	free(tree->sentinel);
	if(ownTree) free(tree);
}


void RBTreeClear(rb_red_blk_tree *tree){
	TreeDestHelper(tree, tree->root->left);
	RBPrimeRoot(tree);
}

rb_red_blk_node* RBExactQuery(const rb_red_blk_tree* tree, const void* q) {
	rb_red_blk_node* x=tree->root->left;
	const rb_red_blk_node* sentinel=tree->sentinel;
	if (x != sentinel) {
		int compVal = tree->Compare(x->key, q);
		while(0 != compVal) {/*assignment*/
			x= (compVal > 0) ? x->left : x->right;
			if ( x == sentinel) {
				break;
			}
			compVal=tree->Compare(x->key, q);
		}
	}
	return (x == sentinel) ? NULL : x;
}

void RBDelete(rb_red_blk_tree* tree, rb_red_blk_node* z){
	rb_red_blk_node* y;
	rb_red_blk_node* x;
	rb_red_blk_node* sentinel=tree->sentinel;
	rb_red_blk_node* root=tree->root;
	
	if(z == tree->first){
		tree->first = TreeSuccessor(tree, z);
	}
	
	y= ((z->left == sentinel) || (z->right == sentinel)) ? z : TreeSuccessor(tree,z);
	x= (y->left == sentinel) ? y->right : y->left;
	if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
		root->left=x;
	} else {
		if (y == y->parent->left) {
			y->parent->left=x;
		} else {
			y->parent->right=x;
		}
	}
	if (y != z) { /* y should not be sentinel in this case */
		
		assert( (y!=tree->sentinel) && "y is sentinel in RBDelete\n");
		/* y is the node to splice out and x is its child */
		
		if (!(y->red)) RBDeleteFixUp(tree,x);
  
		tree->DestroyNodeContents(tree, z);
		y->left=z->left;
		y->right=z->right;
		y->parent=z->parent;
		y->red=z->red;
		z->left->parent=z->right->parent=y;
		if (z == z->parent->left) {
			z->parent->left=y;
		} else {
			z->parent->right=y;
		}
		free(z);
	} else {
		tree->DestroyNodeContents(tree, y);
		if (!(y->red)) RBDeleteFixUp(tree,x);
		free(y);
	}
	--(tree->size);
	
	assert(!tree->sentinel->red && "sentinel not black in RBDelete");
}

void* RBMapPut(rb_red_blk_map_tree *tree, void* key, void* value){
	void * ret;
	rb_red_blk_map_node* node = (rb_red_blk_map_node*)RBExactQuery((rb_red_blk_tree*)tree, key);
	if(node) {
		ret = node->info;
	} else {
		ret = NULL;
		node = (rb_red_blk_map_node*)RBTreeInsert((rb_red_blk_tree*)tree, key);
	}
	node->info = value;
	return ret;
}

void* RBMapRemove(rb_red_blk_map_tree *tree, const void* key){
	void * ret;
	rb_red_blk_map_node* node = (rb_red_blk_map_node*)RBExactQuery((rb_red_blk_tree*)tree, key);
	if(node) {
		ret = node->info;
		RBDelete((rb_red_blk_tree*)tree, (rb_red_blk_node*)node);
	} else {
		ret = NULL;
	}
	return ret;
}

bool RBSetAdd(rb_red_blk_tree *tree, void* key){
	bool ret;
	rb_red_blk_node* node = RBExactQuery(tree, key);
	if ((ret = !node)) {
		RBTreeInsert(tree, key);
	}
	return ret;
}

bool RBSetRemove(rb_red_blk_tree *tree, const void* key){
	bool ret;
	rb_red_blk_node* node = RBExactQuery(tree, key);
	if ((ret = (bool)node)) {
		RBDelete(tree, node);
	}
	return ret;
}

bool RBSetContains(const rb_red_blk_tree *tree, const void* key){
	return (bool)RBExactQuery(tree, key);
}

/***************************************************************************************
 *
 * The functions that follow are helper functions, and not part of the public API
 *
 ***************************************************************************************/

void RBTreeNodeDestroy(rb_red_blk_tree* tree, rb_red_blk_node* target){
	if(tree->DestroyKey) tree->DestroyKey(target);
}

void RBMapTreeNodeDestroy(rb_red_blk_map_tree* tree, rb_red_blk_map_node* target){
	if(tree->DestroyInfo) tree->DestroyInfo(target->info);
	RBTreeNodeDestroy((rb_red_blk_tree* )tree, (rb_red_blk_node*)target);
}

void RBPrimeRoot(rb_red_blk_tree * tree){
	rb_red_blk_node* temp;
	temp=tree->sentinel;
	temp->parent=temp->left=temp->right=temp;
	temp->red=0;
	temp->key=0;
	temp=tree->root;
	temp->parent=temp->left=temp->right=tree->sentinel;
	temp->key=0;
	temp->red=0;
	tree->size = 0;
	tree->first = tree->sentinel;
}


void LeftRotate(rb_red_blk_tree* tree, rb_red_blk_node* x) {
	rb_red_blk_node* y;
	rb_red_blk_node* sentinel=tree->sentinel;
	
	/*  I originally wrote this function to use the sentinel for */
	/*  sentinel to avoid checking for sentinel.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of sentinel.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the sentinel sentinel */
	/*  and expects the sentinel sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of sentinel to be */
	/*  unchanged. */
	
	y=x->right;
	x->right=y->left;
	
	if (y->left != sentinel) y->left->parent=x; /* used to use sentinel here */
	/* and do an unconditional assignment instead of testing for sentinel */
	
	y->parent=x->parent;
	
	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	if( x == x->parent->left) {
		x->parent->left=y;
	} else {
		x->parent->right=y;
	}
	y->left=x;
	x->parent=y;
	
	assert(!tree->sentinel->red && "sentinel not red in LeftRotate");
}

void RightRotate(rb_red_blk_tree* tree, rb_red_blk_node* y) {
	rb_red_blk_node* x;
	rb_red_blk_node* sentinel=tree->sentinel;
	
	/*  I originally wrote this function to use the sentinel for */
	/*  sentinel to avoid checking for sentinel.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of sentinel.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the sentinel sentinel */
	/*  and expects the sentinel sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of sentinel to be */
	/*  unchanged. */
	
	x=y->left;
	y->left=x->right;
	
	if (sentinel != x->right)  x->right->parent=y; /*used to use sentinel here */
	/* and do an unconditional assignment instead of testing for sentinel */
	
	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	x->parent=y->parent;
	if( y == y->parent->left) {
		y->parent->left=x;
	} else {
		y->parent->right=x;
	}
	x->right=y;
	y->parent=x;
	
	assert(!tree->sentinel->red && "sentinel not red in RightRotate");
}

void TreeInsertHelp(rb_red_blk_tree* tree, rb_red_blk_node* z) {
	/*  This function should only be called by InsertRBTree (see above) */
	rb_red_blk_node* x;
	rb_red_blk_node* y;
	rb_red_blk_node* sentinel=tree->sentinel;
	
	z->left=z->right=sentinel;
	y=tree->root;
	x=tree->root->left;
	while( x != sentinel) {
		y=x;
		x = (tree->Compare(x->key,z->key) > 0) ? x->left : x->right;
	}
	z->parent=y;
	*(rb_red_blk_node**)((uint8_t*)y + (((y == tree->root) ||
										 (tree->Compare(y->key,z->key) > 0)) ? offsetof(rb_red_blk_node,left) : offsetof(rb_red_blk_node,right))) = z;
	assert(!tree->sentinel->red && "sentinel not red in TreeInsertHelp");
}

void TreeDestHelper(rb_red_blk_tree* tree, rb_red_blk_node* x) {
	rb_red_blk_node* sentinel=tree->sentinel;
	if (x != sentinel) {
		TreeDestHelper(tree,x->left);
		TreeDestHelper(tree,x->right);
		tree->DestroyNodeContents(tree, x);
		free(x);
	}
}

void RBDeleteFixUp(rb_red_blk_tree* tree, rb_red_blk_node* x) {
	rb_red_blk_node* root=tree->root->left;
	rb_red_blk_node* w;
	
	while( (!x->red) && (root != x)) {
		if (x == x->parent->left) {
			w=x->parent->right;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				LeftRotate(tree,x->parent);
				w=x->parent->right;
			}
			if ( (!w->right->red) && (!w->left->red) ) {
				w->red=1;
				x=x->parent;
			} else {
				if (!w->right->red) {
					w->left->red=0;
					w->red=1;
					RightRotate(tree,w);
					w=x->parent->right;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->right->red=0;
				LeftRotate(tree,x->parent);
				x=root; /* this is to exit while loop */
			}
		} else { /* the code below is has left and right switched from above */
			w=x->parent->left;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				RightRotate(tree,x->parent);
				w=x->parent->left;
			}
			if ( (!w->right->red) && (!w->left->red) ) {
				w->red=1;
				x=x->parent;
			} else {
				if (!w->left->red) {
					w->right->red=0;
					w->red=1;
					LeftRotate(tree,w);
					w=x->parent->left;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->left->red=0;
				RightRotate(tree,x->parent);
				x=root; /* this is to exit while loop */
			}
		}
	}
	x->red=0;
	
	assert(!tree->sentinel->red && "sentinel not black in RBDeleteFixUp");
}