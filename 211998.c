static struct gfxinfo *php_handle_jp2(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  unsigned int box_length;
  unsigned int box_type;
  char jp2c_box_id[] = {(char)0x6a, (char)0x70, (char)0x32, (char)0x63};

  /* JP2 is a wrapper format for JPEG 2000. Data is contained within "boxes".
     Boxes themselves can be contained within "super-boxes". Super-Boxes can
     contain super-boxes which provides us with a hierarchical storage system.

     It is valid for a JP2 file to contain multiple individual codestreams.
     We'll just look for the first codestream at the root of the box structure
     and handle that.
  */

  for (;;)
  {
    box_length = php_read4(stream); /* LBox */
    /* TBox */
    String str = stream->read(sizeof(box_type));
    if (str.length() != sizeof(box_type)) {
      /* Use this as a general "out of stream" error */
      break;
    }
    memcpy(&box_type, str.c_str(), sizeof(box_type));

    if (box_length == 1) {
      /* We won't handle XLBoxes */
      return nullptr;
    }

    if (!memcmp(&box_type, jp2c_box_id, 4))
    {
      /* Skip the first 3 bytes to emulate the file type examination */
      stream->seek(3, SEEK_CUR);

      result = php_handle_jpc(stream);
      break;
    }

    /* Stop if this was the last box */
    if ((int)box_length <= 0) {
      break;
    }

    /* Skip over LBox (Which includes both TBox and LBox itself */
    if (!stream->seek(box_length - 8, SEEK_CUR)) {
      break;
    }
  }

  if (result == nullptr) {
    raise_warning("JP2 file has no codestreams at root level");
  }

  return result;
}