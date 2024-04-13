  bool parseValue(bool array_elem = false) {
    auto const ch = *p++;
    if (ch == '{') return parseMixed();
    else if (ch == '[') return parsePacked();
    else if (ch == '\"') return parseString();
    else if ((ch >= '0' && ch <= '9') ||
              ch == '-') return parseNumber(ch, array_elem);
    else if (ch == 't') return parseRue();
    else if (ch == 'f') return parseAlse();
    else if (ch == 'n') return parseUll();
    else if (isSpace(ch)) {
      skipSpace();
      return parseValue(array_elem);
    }
    else return false;
  }