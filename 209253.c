void TarFileReader::addCompression(compression_t compression) {
  switch (compression) {
  case TARFILE_NONE: break; // none
  case TARFILE_BZIP2: pri->filter.push(BZip2Decompressor()); break;
  case TARFILE_GZIP: pri->filter.push(io::zlib_decompressor()); break;
  default: THROW("Invalid compression type " << compression);
  }
}