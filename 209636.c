void Jsi_ValueFree(Jsi_Interp *interp, Jsi_Value* v)
{
    interp->dbPtr->valueCnt--;
    jsi_DebugValue(v, "Free", jsi_DebugValueCallIdx(), interp);
    ValueFree(interp, v);
#ifdef JSI_MEM_DEBUG
    //if (v->VD.interp != interp)  //TODO: InterpAliasCmd leaking Values.
     //   fprintf(stderr, "cross interp delete: %p\n", v);
    if (v->VD.hPtr && !interp->cleanup) {
        if (!Jsi_HashEntryDelete(v->VD.hPtr))
            fprintf(stderr, "Value not in hash\n");
    }
    memset(v, 0, (sizeof(*v)-sizeof(v->VD)));
#endif
    Jsi_Free(v);
}