const char *get_errname_from_code (uint error_code, st_error *e)
{
   DBUG_ENTER("get_errname_from_code");
   DBUG_PRINT("enter", ("error_code: %d", error_code));

   if (! error_code)
   {
     DBUG_RETURN("");
   }
   for (; e->name; e++)
   {
     if (e->code == error_code)
     {
       DBUG_RETURN(e->name);
     }
   }
   /* Apparently, errors without known names may occur */
   DBUG_RETURN(unknown_error);
} 