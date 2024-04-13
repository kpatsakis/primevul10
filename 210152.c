Jsi_GetIndex( Jsi_Interp *interp, const char *str,
    const char **tablePtr, const char *msg, int flags,
    int *indexPtr)
{
  const char *msg2 = "unknown ";
  char **cp, *c;
  int cond, index = -1, slen, i, dup = 0;
  int exact = (flags & JSI_CMP_EXACT);
  int nocase = (flags & JSI_CMP_NOCASE);
  slen = Jsi_Strlen(str);
 /* if (slen==0) 
        return Jsi_LogError("empty option %s %s", msg, str);*/
  cp = (char**)tablePtr;
  i = -1;
  while (*cp != 0) {
    i++;
    c = *cp;
    if (c[0] != str[0]) { cp++; continue; }
    if (!nocase)
        cond = (exact ? Jsi_Strcmp(c,str) : Jsi_Strncmp(c,str,slen));
    else {
        cond = (exact ? Jsi_Strncasecmp(c,str, -1) : Jsi_Strncasecmp(c,str,slen));
    }
    if (cond == 0) {
      if (index<0) {
        index = i;
      } else {
        dup = 1;
        break;
      }
    }
    cp++;
  }
  if (index >= 0 && dup == 0) {
    *indexPtr = index;
    return JSI_OK;
  }
  if (exact && (dup || index<=0)) {
    if (interp != NULL) {
      msg2 = (index>=0? "unknown ":"duplicate ");
    }
    goto err;
  }
  cp = (char**)tablePtr;
  i = -1;
  dup = 0;
  index = -1;
  while (*cp != 0) {
    i++;
    c = *cp;
    if (c[0] == str[0] && Jsi_Strncmp(c,str, slen) == 0) {
      if (index<0) {
        index = i;
        if (slen == (int)Jsi_Strlen(c))
            break;
      } else {
        if (interp != NULL) {
          msg2 = "ambiguous ";
        }
        goto err;
      }
    }
    cp++;
  }
  if (index >= 0 && dup == 0) {
    *indexPtr = index;
    return JSI_OK;
  }
err:
  if (interp != NULL) {
    Jsi_DString dStr = {};
    Jsi_DSAppend(&dStr, msg2, msg, " \"", str, "\" not one of: ", NULL);
    cp = (char**)tablePtr;
    while (*cp != 0) {
      c = *cp;
      Jsi_DSAppend(&dStr, c, NULL);
      Jsi_DSAppend(&dStr, " ", NULL);
      cp++;
    }
    Jsi_LogError("%s", Jsi_DSValue(&dStr));
    Jsi_DSFree(&dStr);
  }
  return JSI_ERROR;
}