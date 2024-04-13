string Canonicalize(const string& funcname, AttrSlice attrs) {
  static const FunctionLibraryRuntime::InstantiateOptions* kEmptyOptions =
      new FunctionLibraryRuntime::InstantiateOptions;
  return Canonicalize(funcname, attrs, *kEmptyOptions);
}