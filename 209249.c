TarFileReader::TarFileReader(const string &path, compression_t compression) :
  pri(new private_t), stream(SystemUtilities::iopen(path)),
  didReadHeader(false) {

  addCompression(compression == TARFILE_AUTO ? infer(path) : compression);
  pri->filter.push(*this->stream);
}