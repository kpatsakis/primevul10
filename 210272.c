void Jsi_RegExpFree(Jsi_Regex* re) {
    regfree(&re->reg);
    _JSI_MEMCLEAR(re);
    Jsi_Free(re);
}