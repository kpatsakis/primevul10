void ricon(char *name, char *comment, char *onclick) {
   rsprintf
           ("<img align=\"middle\" name=\"%s\" src=\"icons/elc_%s.png\" alt=\"%s\" title=\"%s\" border=\"0\"\n",
            name, name, comment, comment);
   rsprintf(" onclick=\"%s\"", onclick);
   rsprintf(" onmousedown=\"document.images.%s.src='icons/eld_%s.png'\"\n", name, name);
   rsprintf(" onmouseup=\"document.images.%s.src='icons/elc_%s.png'\"\n", name, name);
   rsprintf(" onmouseover=\"this.style.cursor='pointer';\" />\n");
}