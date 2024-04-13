void HHVM_FUNCTION(parse_str,
                   const String& str,
                   Array& arr) {
  arr = Array::CreateDArray();
  HttpProtocol::DecodeParameters(arr, str.data(), str.size());
}