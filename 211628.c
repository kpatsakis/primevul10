Variant php_split(const String& spliton, const String& str, int count,
                  bool icase) {
  const char* strp = str.data();
  const char* endp = strp + str.size();

  regex_t re;
  int copts = icase ? REG_ICASE : 0;
  int err = regcomp(&re, spliton.data(), REG_EXTENDED | copts);
  if (err) {
    php_reg_eprint(err, &re);
    return false;
  }

  Array return_value = Array::CreateVArray();
  regmatch_t subs[1];

  /* churn through str, generating array entries as we go */
  while ((count == -1 || count > 1) &&
         !(err = regexec(&re, strp, 1, subs, 0))) {
    if (subs[0].rm_so == 0 && subs[0].rm_eo) {
      /* match is at start of string, return empty string */
      return_value.append("");
      /* skip ahead the length of the regex match */
      strp += subs[0].rm_eo;
    } else if (subs[0].rm_so == 0 && subs[0].rm_eo == 0) {
      /* No more matches */
      regfree(&re);
      raise_warning("Invalid Regular Expression to split()");
      return false;
    } else {
      /* On a real match */

      /* make a copy of the substring */
      int size = subs[0].rm_so;

      /* add it to the array */
      return_value.append(String(strp, size, CopyString));

      /* point at our new starting point */
      strp = strp + subs[0].rm_eo;
    }

    /* if we're only looking for a certain number of points,
       stop looking once we hit it */
    if (count != -1) {
      count--;
    }
  }

  /* see if we encountered an error */
  if (err && err != REG_NOMATCH) {
    php_reg_eprint(err, &re);
    regfree(&re);
    return false;
  }

  /* otherwise we just have one last element to add to the array */
  int size = endp - strp;
  return_value.append(String(strp, size, CopyString));

  regfree(&re);
  return return_value;
}