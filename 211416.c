static Array str_offset_pair(const String& str, int offset,
                             bool hackArrOutput) {
  return hackArrOutput ? make_vec_array(str, offset)
                       : make_varray(str, offset);
}