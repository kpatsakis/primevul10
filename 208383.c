static void langmap_set(void)
{
  char_u  *p;
  char_u  *p2;
  int from, to;

  ga_clear(&langmap_mapga);                 /* clear the previous map first */
  langmap_init();                           /* back to one-to-one map */

  for (p = p_langmap; p[0] != NUL; ) {
    for (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
         mb_ptr_adv(p2)) {
      if (p2[0] == '\\' && p2[1] != NUL)
        ++p2;
    }
    if (p2[0] == ';')
      ++p2;                 /* abcd;ABCD form, p2 points to A */
    else
      p2 = NULL;            /* aAbBcCdD form, p2 is NULL */
    while (p[0]) {
      if (p[0] == ',') {
        ++p;
        break;
      }
      if (p[0] == '\\' && p[1] != NUL)
        ++p;
      from = (*mb_ptr2char)(p);
      to = NUL;
      if (p2 == NULL) {
        mb_ptr_adv(p);
        if (p[0] != ',') {
          if (p[0] == '\\')
            ++p;
          to = (*mb_ptr2char)(p);
        }
      } else {
        if (p2[0] != ',') {
          if (p2[0] == '\\')
            ++p2;
          to = (*mb_ptr2char)(p2);
        }
      }
      if (to == NUL) {
        EMSG2(_("E357: 'langmap': Matching character missing for %s"),
            transchar(from));
        return;
      }

      if (from >= 256)
        langmap_set_entry(from, to);
      else {
        assert(to <= UCHAR_MAX);
        langmap_mapchar[from & 255] = (char_u)to;
      }

      /* Advance to next pair */
      mb_ptr_adv(p);
      if (p2 != NULL) {
        mb_ptr_adv(p2);
        if (*p == ';') {
          p = p2;
          if (p[0] != NUL) {
            if (p[0] != ',') {
              EMSG2(_(
                      "E358: 'langmap': Extra characters after semicolon: %s"),
                  p);
              return;
            }
            ++p;
          }
          break;
        }
      }
    }
  }
}