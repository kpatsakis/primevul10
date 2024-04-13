int ndpi_check_dga_name(struct ndpi_detection_module_struct *ndpi_str,
			struct ndpi_flow_struct *flow,
			char *name) {
  int len, rc = 0;

  len = strlen(name);
  
  if(len >= 5) {
    int i, j, num_found = 0, num_impossible = 0, num_bigram_checks = 0, num_digits = 0, num_vowels = 0, num_words = 0;
    char tmp[128], *word, *tok_tmp;

    len = snprintf(tmp, sizeof(tmp)-1, "%s", name);
    if(len < 0) return(0);

    for(i=0, j=0; (i<len) && (j<(sizeof(tmp)-1)); i++) {
	tmp[j++] = tolower(name[i]);
    }

    tmp[j] = '\0';
    len = j;

    for(word = strtok_r(tmp, ".", &tok_tmp); ; word = strtok_r(NULL, ".", &tok_tmp)) {
      if(!word) break;

      num_words++;
      
      if(strlen(word) < 3) continue;

#ifdef DGA_DEBUG
      printf("-> %s [%s][len: %u]\n", word, name, (unsigned int)strlen(word));
#endif
      
      for(i = 0; word[i+1] != '\0'; i++) {
	if(isdigit(word[i])) {
	  num_digits++;
	  
	  // if(!isdigit(word[i+1])) num_impossible++;
	  
	  continue;	  
	}
	
	switch(word[i]) {
	case '_':
	case '-':
	case ':':
	  continue;
	  break;
	
	case '.':
	  continue;
	  break;
	}

	switch(word[i]) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
	  num_vowels++;
	  break;
	}
	
	if(isdigit(word[i+1])) {
	  num_digits++;
	  // num_impossible++;
	  continue;
	}
	
	num_bigram_checks++;

	if(ndpi_match_bigram(ndpi_str, &ndpi_str->bigrams_automa, &word[i])) {
	  num_found++;
	} else {
	  if(ndpi_match_bigram(ndpi_str,
			       &ndpi_str->impossible_bigrams_automa,
			       &word[i])) {
#ifdef DGA_DEBUG
	    printf("IMPOSSIBLE %s\n", &word[i]);
#endif
	    num_impossible++;
	  }
	}
      } /* for */
    } /* for */

#ifdef DGA_DEBUG
    printf("[num_found: %u][num_impossible: %u][num_digits: %u][num_bigram_checks: %u][num_vowels: %u/%u]\n",
	   num_found, num_impossible, num_digits, num_bigram_checks, num_vowels, j-num_vowels);
#endif
	    
    if(num_bigram_checks
       && ((num_found == 0) || ((num_digits > 5) && (num_words <= 3)) || enough(num_found, num_impossible)))
      rc = 1;

    if(rc && flow)
      NDPI_SET_BIT(flow->risk, NDPI_SUSPICIOUS_DGA_DOMAIN);

#ifdef DGA_DEBUG
    if(rc)
      printf("DGA %s [num_found: %u][num_impossible: %u]\n",
	     name, num_found, num_impossible);
#endif
  }

  return(rc);
}