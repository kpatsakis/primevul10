static String preg_do_repl_func(const Variant& function, const String& subject,
                                int* offsets, const char* const* subpat_names,
                                int count) {
  Array subpats = Array::CreateDArray();
  for (int i = 0; i < count; i++) {
    auto off1 = offsets[i<<1];
    auto off2 = offsets[(i<<1)+1];
    auto sub = subject.substr(off1, off2 - off1);

    if (subpat_names[i]) {
      subpats.set(String(subpat_names[i]), sub);
    }
    subpats.set(i, sub);
  }

  return vm_call_user_func(function, make_varray(subpats)).toString();
}