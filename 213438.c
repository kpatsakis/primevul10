Status EncodePyBytesArray(PyArrayObject* array, tensorflow::int64 nelems,
                          size_t* size, void** buffer) {
  // Encode all strings.
  *size = nelems * sizeof(tensorflow::tstring);
  std::unique_ptr<tensorflow::tstring[]> base_ptr(
      new tensorflow::tstring[nelems]);
  tensorflow::tstring* dst = base_ptr.get();

  TF_RETURN_IF_ERROR(
      PyBytesArrayMap(array, [&dst](const char* ptr, Py_ssize_t len) {
        dst->assign(ptr, len);
        dst++;
      }));
  *buffer = base_ptr.release();
  return Status::OK();
}