void Image::reset() {
  if (m_gdImage) {
    gdImageDestroy(m_gdImage);
    m_gdImage = nullptr;
  }
}