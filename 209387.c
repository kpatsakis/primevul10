static Jsi_OpCodes *codes_join(Jsi_OpCodes *a, Jsi_OpCodes *b)
{
    Jsi_OpCodes *ret = codes_new(a->code_len + b->code_len);
    memcpy(ret->codes, a->codes, a->code_len * sizeof(jsi_OpCode));
    memcpy(&ret->codes[a->code_len], b->codes, b->code_len * sizeof(jsi_OpCode));
    ret->code_size = a->code_len + b->code_len;
    ret->code_len = ret->code_size;
    ret->expr_counter = a->expr_counter + b->expr_counter;
#if 0
    a->code_len=0;
    jsi_FreeOpcodes(a);
    b->code_len=0;
    jsi_FreeOpcodes(b);
#else
    Jsi_Free(a->codes);
    Jsi_Free(b->codes);
#ifdef JSI_MEM_DEBUG
    if (a->hPtr)
        Jsi_HashEntryDelete(a->hPtr);
    if (b->hPtr)
        Jsi_HashEntryDelete(b->hPtr);
#endif
    Jsi_Free(a);
    Jsi_Free(b);
#endif
    jsiOpCodesCnt[1]++;
    jsiOpCodesCnt[2]-=2;
    return ret;
}