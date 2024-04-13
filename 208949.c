int is_image(char *att) {
   return (stristr(att, ".GIF") != NULL) || (stristr(att, ".JPG") != NULL) || (stristr(att, ".JPEG") != NULL)
          || (stristr(att, ".PNG") != NULL) || (stristr(att, ".SVG") != NULL);
}