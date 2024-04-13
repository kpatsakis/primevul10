Status GetPyArrayDescrForTensor(const TF_Tensor* tensor,
                                PyArray_Descr** descr) {
  if (TF_TensorType(tensor) == TF_RESOURCE) {
    PyObject* field = PyTuple_New(3);
#if PY_MAJOR_VERSION < 3
    PyTuple_SetItem(field, 0, PyBytes_FromString("resource"));
#else
    PyTuple_SetItem(field, 0, PyUnicode_FromString("resource"));
#endif
    PyTuple_SetItem(field, 1, PyArray_TypeObjectFromType(NPY_UBYTE));
    PyTuple_SetItem(field, 2, PyLong_FromLong(1));
    PyObject* fields = PyList_New(1);
    PyList_SetItem(fields, 0, field);
    int convert_result = PyArray_DescrConverter(fields, descr);
    Py_CLEAR(field);
    Py_CLEAR(fields);
    if (convert_result != 1) {
      return errors::Internal("Failed to create numpy array description for ",
                              "TF_RESOURCE-type tensor");
    }
  } else {
    int type_num = -1;
    TF_RETURN_IF_ERROR(
        TF_DataType_to_PyArray_TYPE(TF_TensorType(tensor), &type_num));
    *descr = PyArray_DescrFromType(type_num);
  }

  return Status::OK();
}