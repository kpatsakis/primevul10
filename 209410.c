static int jsi_cmpstringp(const void *p1, const void *p2)
{
   return Jsi_Strcmp(* (char * const *) p1, * (char * const *) p2);
}