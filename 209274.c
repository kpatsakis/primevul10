void CharCodeToUnicode::parseCMap1(int (*getCharFunc)(void *), void *data,
				   int nBits) {
  PSTokenizer *pst;
  char tok1[256], tok2[256], tok3[256];
  int nDigits, n1, n2, n3;
  CharCode i;
  CharCode code1, code2;
  GString *name;
  FILE *f;

  nDigits = nBits / 4;
  pst = new PSTokenizer(getCharFunc, data);
  pst->getToken(tok1, sizeof(tok1), &n1);
  while (pst->getToken(tok2, sizeof(tok2), &n2)) {
    if (!strcmp(tok2, "usecmap")) {
      if (tok1[0] == '/') {
	name = new GString(tok1 + 1);
	if ((f = globalParams->findToUnicodeFile(name))) {
	  parseCMap1(&getCharFromFile, f, nBits);
	  fclose(f);
	} else {
	  error(-1, "Couldn't find ToUnicode CMap file for '%s'",
		name->getCString());
	}
	delete name;
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } else if (!strcmp(tok2, "beginbfchar")) {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) {
	if (!strcmp(tok1, "endbfchar")) {
	  break;
	}
	if (!pst->getToken(tok2, sizeof(tok2), &n2) ||
	    !strcmp(tok2, "endbfchar")) {
	  error(-1, "Illegal entry in bfchar block in ToUnicode CMap");
	  break;
	}
	if (!(n1 == 2 + nDigits && tok1[0] == '<' && tok1[n1 - 1] == '>' &&
	      tok2[0] == '<' && tok2[n2 - 1] == '>')) {
	  error(-1, "Illegal entry in bfchar block in ToUnicode CMap");
	  continue;
	}
	tok1[n1 - 1] = tok2[n2 - 1] = '\0';
	if (sscanf(tok1 + 1, "%x", &code1) != 1) {
	  error(-1, "Illegal entry in bfchar block in ToUnicode CMap");
	  continue;
	}
	addMapping(code1, tok2 + 1, n2 - 2, 0);
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } else if (!strcmp(tok2, "beginbfrange")) {
      while (pst->getToken(tok1, sizeof(tok1), &n1)) {
	if (!strcmp(tok1, "endbfrange")) {
	  break;
	}
	if (!pst->getToken(tok2, sizeof(tok2), &n2) ||
	    !strcmp(tok2, "endbfrange") ||
	    !pst->getToken(tok3, sizeof(tok3), &n3) ||
	    !strcmp(tok3, "endbfrange")) {
	  error(-1, "Illegal entry in bfrange block in ToUnicode CMap");
	  break;
	}
	if (!(n1 == 2 + nDigits && tok1[0] == '<' && tok1[n1 - 1] == '>' &&
	      n2 == 2 + nDigits && tok2[0] == '<' && tok2[n2 - 1] == '>')) {
	  error(-1, "Illegal entry in bfrange block in ToUnicode CMap");
	  continue;
	}
	tok1[n1 - 1] = tok2[n2 - 1] = '\0';
	if (sscanf(tok1 + 1, "%x", &code1) != 1 ||
	    sscanf(tok2 + 1, "%x", &code2) != 1) {
	  error(-1, "Illegal entry in bfrange block in ToUnicode CMap");
	  continue;
	}
	if (!strcmp(tok3, "[")) {
	  i = 0;
	  while (pst->getToken(tok1, sizeof(tok1), &n1) &&
		 code1 + i <= code2) {
	    if (!strcmp(tok1, "]")) {
	      break;
	    }
	    if (tok1[0] == '<' && tok1[n1 - 1] == '>') {
	      tok1[n1 - 1] = '\0';
	      addMapping(code1 + i, tok1 + 1, n1 - 2, 0);
	    } else {
	      error(-1, "Illegal entry in bfrange block in ToUnicode CMap");
	    }
	    ++i;
	  }
	} else if (tok3[0] == '<' && tok3[n3 - 1] == '>') {
	  tok3[n3 - 1] = '\0';
	  for (i = 0; code1 <= code2; ++code1, ++i) {
	    addMapping(code1, tok3 + 1, n3 - 2, i);
	  }

	} else {
	  error(-1, "Illegal entry in bfrange block in ToUnicode CMap");
	}
      }
      pst->getToken(tok1, sizeof(tok1), &n1);
    } else {
      strcpy(tok1, tok2);
    }
  }
  delete pst;
}