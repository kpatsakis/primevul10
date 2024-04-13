int TfLiteFloatArrayGetSizeInBytes(int size) {
  static TfLiteFloatArray dummy;
  return sizeof(dummy) + sizeof(dummy.data[0]) * size;
}