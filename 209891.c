char jsi_fromHexChar(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}