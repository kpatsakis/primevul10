Status PyObjectToString(PyObject* obj, const char** ptr, Py_ssize_t* len,
                        PyObject** ptr_owner) {
  *ptr_owner = nullptr;
  if (PyBytes_Check(obj)) {
    char* buf;
    if (PyBytes_AsStringAndSize(obj, &buf, len) != 0) {
      return errors::Internal("Unable to get element as bytes.");
    }
    *ptr = buf;
    return Status::OK();
  } else if (PyUnicode_Check(obj)) {
#if (PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3))
    *ptr = PyUnicode_AsUTF8AndSize(obj, len);
    if (*ptr != nullptr) return Status::OK();
#else
    PyObject* utemp = PyUnicode_AsUTF8String(obj);
    char* buf;
    if (utemp != nullptr && PyBytes_AsStringAndSize(utemp, &buf, len) != -1) {
      *ptr = buf;
      *ptr_owner = utemp;
      return Status::OK();
    }
    Py_XDECREF(utemp);
#endif
    return errors::Internal("Unable to convert element to UTF-8");
  } else {
    return errors::Internal("Unsupported object type ", obj->ob_type->tp_name);
  }
}