int param_compare(const void *p1, const void *p2) {
   return stricmp(((CONFIG_PARAM *) p1)->uparam, ((CONFIG_PARAM *) p2)->uparam);
}