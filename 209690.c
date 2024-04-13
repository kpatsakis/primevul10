Jsi_Value *Jsi_ValueDupJSON(Jsi_Interp *interp, Jsi_Value *val)
{
    Jsi_DString pStr;
    Jsi_DSInit(&pStr);
    Jsi_ValueGetDString(interp, val, &pStr, JSI_OUTPUT_JSON);
    Jsi_Value *valPtr = NULL;
    if (Jsi_JSONParse(interp, Jsi_DSValue(&pStr), &valPtr, 0) != JSI_OK)
        Jsi_LogBug("bad json parse");
    Jsi_DSFree(&pStr);
    return valPtr;
}