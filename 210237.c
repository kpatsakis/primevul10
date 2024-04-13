static Jsi_TreeEntry *searchAdd(Jsi_TreeSearch *searchPtr,  Jsi_TreeEntry *hPtr)
{
    int order = (searchPtr->flags & JSI_TREE_ORDER_MASK);
    searchSpace(searchPtr, 2);
    switch (order) {
        case JSI_TREE_ORDER_LEVEL:
            if (hPtr) {
                if (hPtr->right)
                    searchPtr->Ptrs[searchPtr->top++] = hPtr->right;
                if (hPtr->left)
                    searchPtr->Ptrs[searchPtr->top++] = hPtr->left;
                return hPtr;
            }
            if (searchPtr->top<=0)
                return NULL;
            hPtr = searchPtr->Ptrs[0];
            searchPtr->top--;
            if (searchPtr->top > 0) {
                /* Not very efficient way to implement a queue, but works for now. */
                memmove(searchPtr->Ptrs, searchPtr->Ptrs+1, sizeof(Jsi_TreeEntry*)*searchPtr->top);
            }
            if (hPtr->right)
                searchPtr->Ptrs[searchPtr->top++] = hPtr->right;
            if (hPtr->left)
                searchPtr->Ptrs[searchPtr->top++] = hPtr->left;
            return hPtr;
            break;
            
        case JSI_TREE_ORDER_POST:
            if (hPtr)
                searchPtr->Ptrs[searchPtr->top++] = searchPtr->current = hPtr;
            while (searchPtr->top>0) {
                hPtr = searchPtr->Ptrs[searchPtr->top-1];
                if (hPtr->right == searchPtr->current || hPtr->left == searchPtr->current ||
                    (hPtr->left == NULL && hPtr->right == NULL)) {
                    searchPtr->top--;
                    searchPtr->current = hPtr;
                    return hPtr;
                } else {
                    searchSpace(searchPtr, 2);
                    if (hPtr->left)
                        searchPtr->Ptrs[searchPtr->top++] = hPtr->left;
                    if (hPtr->right)
                        searchPtr->Ptrs[searchPtr->top++] = hPtr->right;
                }
            }
            return NULL;
            break;
            
        case JSI_TREE_ORDER_PRE:
            if (!hPtr) {
                if (searchPtr->top<=0) return NULL;
                hPtr = searchPtr->Ptrs[--searchPtr->top];
            }
            searchPtr->Ptrs[searchPtr->top++] = hPtr;
            if (hPtr->left) searchPtr->Ptrs[searchPtr->top++] = hPtr->left;
            if (hPtr->right) searchPtr->Ptrs[searchPtr->top++] = hPtr->right;
            break;
            
        case JSI_TREE_ORDER_IN:
            while (1) {
                searchSpace(searchPtr, 2);
                if (searchPtr->current) {
                    searchPtr->Ptrs[searchPtr->top++] = searchPtr->current;
                    searchPtr->current = searchPtr->current->right;
                } else {
                    if (searchPtr->top<=0)
                        return NULL;
                    hPtr = searchPtr->Ptrs[--searchPtr->top] ;
                    searchPtr->current = hPtr->left;
                    return hPtr;
                }
            }
            break;
            
        default:
            if (hPtr) {
                Jsi_Interp *interp = hPtr->treePtr->opts.interp;
                JSI_NOTUSED(interp);
                Jsi_LogError("Invalid order: %d", order);    
            }    
    }
    return searchPtr->Ptrs[--searchPtr->top];
}