static char * getword(char * begin, char **end_ptr) {
    char * end = begin;
    while ( *begin == ' ' )
        begin++;  /* Get rid of leading spaces. */
    end = begin;
    while ( *end != '\0' && *end != '\n' && *end != ' ')
      end++;  /* Keep going. */
    if ( end == begin)
        return NULL;  /* if no more words, return NULL */
    *end = '\0';  /* else put string terminator at end of this word. */
    *end_ptr = end;
/*    if (begin[0] == '$') //{  if this is a variable to be expanded 
        begin = getenv(begin+1);  begin+1, to skip past '$' 
	if (begin == NULL) {
	    perror("getenv");
	    begin = "UNDEFINED";
        } */
//    }
    return begin; /* This word is now a null-terminated string.  return it. */
