static int jsi_GetListType(Jsi_MapEntry *h) {
    Jsi_HashEntry *hPtr =(Jsi_HashEntry *)h;
    return hPtr->typ;
}