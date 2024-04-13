static Jsi_OpCodes *codes_new(int size)
{
    Jsi_OpCodes *ops = (Jsi_OpCodes *)Jsi_Calloc(1, sizeof(*ops));
    jsiOpCodesCnt[0]++;
    jsiOpCodesCnt[2]++;
    ops->codes = (jsi_OpCode *)Jsi_Calloc(size, sizeof(jsi_OpCode));
    ops->code_size = size;
#ifdef JSI_MEM_DEBUG
    static int idNum = 0;
    ops->hPtr = Jsi_HashSet(jsiIntData.mainInterp->codesTbl, ops, ops);
    ops->id = idNum++;
#endif
    return ops;
}