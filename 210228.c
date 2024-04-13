static void searchSpace(Jsi_TreeSearch *searchPtr, int n)
{
    if ((searchPtr->top+n) >= searchPtr->max) {
        int i, cnt = (searchPtr->max *= 2);
        if (searchPtr->Ptrs == searchPtr->staticPtrs)
            searchPtr->Ptrs = (Jsi_TreeEntry**)Jsi_Calloc(cnt, sizeof(Jsi_TreeEntry*));
        else
            searchPtr->Ptrs = (Jsi_TreeEntry**)Jsi_Realloc(searchPtr->Ptrs, cnt* sizeof(Jsi_TreeEntry*));
        for (i=0; i<cnt; i++)
            SIGINIT((searchPtr->Ptrs[i]),TREEENTRY);

    }
}