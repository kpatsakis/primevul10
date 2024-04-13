Variant WuManberReplacement::translate(String source) const {
  size_t  pos      = 0,
          nextwpos = 0,
          lastpos  = source.size() - m;

  if (!valid) {
    return false;
  }

  // all patterns are longer than the source
  if (m > source.size()) {
    return source;
  }

  StringBuffer  result(source.size());
  while (pos <= lastpos) {
    uint16_t h = strtr_hash(source.data() + pos + m - B, B) & SHIFT_TAB_MASK;
    size_t shift_pos = shift[h];

    if (shift_pos > 0) {
      pos += shift_pos;
    } else {
      uint16_t  h2        = h & HASH_TAB_MASK,
                prefix_h  = strtr_hash(source.data() + pos, Bp);
      int offset_start  = hash[h2],
          offset_end    = hash[h2 + 1], // exclusive
          i             = 0;

      for (i = offset_start; i < offset_end; i++) {
        if (prefix[i] != prefix_h) {
          continue;
        }

        const PatAndRepl *pnr = &patterns[i];
        if (pnr->getPat().size() > source.size() - pos ||
            memcmp(pnr->getPat().data(), source.data() + pos,
                   pnr->getPat().size()) != 0) {
          continue;
        }

        result.append(source.data() + nextwpos, pos - nextwpos);
        result.append(pnr->getRepl());
        pos += pnr->getPat().size();
        nextwpos = pos;
        goto end_outer_loop;
      }

      pos++;
end_outer_loop: ;
    }
  }

  result.append(source.data() + nextwpos, source.size() - nextwpos );

  return result.detach();
}