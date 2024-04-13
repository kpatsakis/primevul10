void rsicon(char *name, char *comment, char *elcode) {
   rsprintf("<img align=\"middle\" name=\"%s\" src=\"icons/elc_%s.png\" alt=\"%s\" title=\"%s\" border=\"0\"",
            name, name, comment, comment);
   rsprintf(" onclick=\"elcode(document.form1.Text, '','%s')\"", elcode);
   rsprintf(" onmousedown=\"document.images.%s.src='icons/eld_%s.png'\"", name, name);
   rsprintf(" onmouseup=\"document.images.%s.src='icons/elc_%s.png'\"", name, name);
   rsprintf(" onmouseover=\"this.style.cursor='pointer';\" />");
}