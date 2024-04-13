static bool preg_get_backref(const char** str, int* backref) {
  char in_brace = 0;
  const char* walk = *str;

  if (walk[1] == 0) {
    return false;
  }

  if (*walk == '$' && walk[1] == '{') {
    in_brace = 1;
    walk++;
  }
  walk++;

  if (*walk >= '0' && *walk <= '9') {
    *backref = *walk - '0';
    walk++;
  } else {
    return false;
  }

  if (*walk && *walk >= '0' && *walk <= '9') {
    *backref = *backref * 10 + *walk - '0';
    walk++;
  }

  if (in_brace) {
    if (*walk == 0 || *walk != '}') {
      return false;
    }
    walk++;
  }

  *str = walk;
  return true;
}