isIpAddress(char *src_addr)
{
  char *ptr;
  int address;
  int i;
  char *s = (char *) malloc (sizeof (char) * LOW_CHAR);

  xstrncpy(s, src_addr, LOW_CHAR);
  
  /* make sure we have numbers and dots only! */
  if(strspn(s, "0123456789.") != strlen(s)) {
    xfree(s);
    return 1;
  }

  /* split up each number from string */
  ptr = strtok(s, ".");
  if(ptr == NULL) {
    xfree(s);
    return 1;
  }
  address = atoi(ptr);
  if(address < 0 || address > 255) {
    xfree(s);
    xfree(ptr);
    return 1;
  }
  
  for(i = 2; i < 4; i++) {
    ptr = strtok(NULL, ".");
    if (ptr == NULL) {
       xfree(s);
       return 1;
    }
    address = atoi(ptr);
    if (address < 0 || address > 255) {
       xfree(ptr);
       xfree(s);
       return 1;
    }
  }
  xfree(s);
  
  return 0;
}