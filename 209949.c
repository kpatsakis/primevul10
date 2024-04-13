void jsi_SortDString(Jsi_Interp *interp, Jsi_DString *dStr, const char *sep) {
    int argc, i;
    char **argv;
    Jsi_DString sStr;
    Jsi_DSInit(&sStr);
    Jsi_SplitStr(Jsi_DSValue(dStr), &argc, &argv, sep, &sStr);
    qsort(argv, argc, sizeof(char*), jsi_cmpstringp);
    Jsi_DSSetLength(dStr, 0);
    for (i=0; i<argc; i++)
        Jsi_DSAppend(dStr, (i?" ":""), argv[i], NULL);
    Jsi_DSFree(&sStr);
}