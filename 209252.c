TarFileReader::TarFileReader(istream &stream, compression_t compression) :
  pri(new private_t), stream(SmartPointer<istream>::Phony(&stream)),
  didReadHeader(false) {

  addCompression(compression);
  pri->filter.push(*this->stream);
}