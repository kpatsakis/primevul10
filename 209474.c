void jsi_FreeOpcodes(Jsi_OpCodes *ops) {
    int i;
    if (!ops) return;
    for (i=0; i<ops->code_len; i++) {
        jsi_OpCode *op = ops->codes+i;
        if (op->data && op->alloc)
            Jsi_Free(op->data);
        _JSI_MEMCLEAR(op);
    }
    jsiOpCodesCnt[1]++;
    jsiOpCodesCnt[2]--;
#ifdef JSI_MEM_DEBUG
    if (ops->hPtr)
        Jsi_HashEntryDelete(ops->hPtr);
#endif
    Jsi_Free(ops->codes);
    Jsi_Free(ops);
}