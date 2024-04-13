Status PyArray_TYPE_to_TF_DataType(PyArrayObject* array,
                                   TF_DataType* out_tf_datatype) {
  int pyarray_type = PyArray_TYPE(array);
  PyArray_Descr* descr = PyArray_DESCR(array);
  switch (pyarray_type) {
    case NPY_FLOAT16:
      *out_tf_datatype = TF_HALF;
      break;
    case NPY_FLOAT32:
      *out_tf_datatype = TF_FLOAT;
      break;
    case NPY_FLOAT64:
      *out_tf_datatype = TF_DOUBLE;
      break;
    case NPY_INT32:
      *out_tf_datatype = TF_INT32;
      break;
    case NPY_UINT8:
      *out_tf_datatype = TF_UINT8;
      break;
    case NPY_UINT16:
      *out_tf_datatype = TF_UINT16;
      break;
    case NPY_UINT32:
      *out_tf_datatype = TF_UINT32;
      break;
    case NPY_UINT64:
      *out_tf_datatype = TF_UINT64;
      break;
    case NPY_INT8:
      *out_tf_datatype = TF_INT8;
      break;
    case NPY_INT16:
      *out_tf_datatype = TF_INT16;
      break;
    case NPY_INT64:
      *out_tf_datatype = TF_INT64;
      break;
    case NPY_BOOL:
      *out_tf_datatype = TF_BOOL;
      break;
    case NPY_COMPLEX64:
      *out_tf_datatype = TF_COMPLEX64;
      break;
    case NPY_COMPLEX128:
      *out_tf_datatype = TF_COMPLEX128;
      break;
    case NPY_OBJECT:
    case NPY_STRING:
    case NPY_UNICODE:
      *out_tf_datatype = TF_STRING;
      break;
    case NPY_VOID:
      // Quantized types are currently represented as custom struct types.
      // PyArray_TYPE returns NPY_VOID for structs, and we should look into
      // descr to derive the actual type.
      // Direct feeds of certain types of ResourceHandles are represented as a
      // custom struct type.
      return PyArrayDescr_to_TF_DataType(descr, out_tf_datatype);
    default:
      if (pyarray_type == Bfloat16NumpyType()) {
        *out_tf_datatype = TF_BFLOAT16;
        break;
      } else if (pyarray_type == NPY_ULONGLONG) {
        // NPY_ULONGLONG is equivalent to NPY_UINT64, while their enum values
        // might be different on certain platforms.
        *out_tf_datatype = TF_UINT64;
        break;
      } else if (pyarray_type == NPY_LONGLONG) {
        // NPY_LONGLONG is equivalent to NPY_INT64, while their enum values
        // might be different on certain platforms.
        *out_tf_datatype = TF_INT64;
        break;
      } else if (pyarray_type == NPY_INT) {
        // NPY_INT is equivalent to NPY_INT32, while their enum values might be
        // different on certain platforms.
        *out_tf_datatype = TF_INT32;
        break;
      } else if (pyarray_type == NPY_UINT) {
        // NPY_UINT is equivalent to NPY_UINT32, while their enum values might
        // be different on certain platforms.
        *out_tf_datatype = TF_UINT32;
        break;
      }
      return errors::Internal("Unsupported numpy type: ",
                              numpy_type_name(pyarray_type));
  }
  return Status::OK();
}