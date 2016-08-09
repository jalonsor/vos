    #include <regex.h>


    /*
     * Match string against the extended regular expression in
     * pattern, treating errors as no match.
     *
     * Return 1 for match, 0 for no match.
     */


    int match(const char *string, char *pattern)
    {
        int    status;
        regex_t    re;


        if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
            return(0);      /* Report error. */
        }
        status = regexec(&re, string, (size_t) 0, NULL, 0);
        regfree(&re);
        if (status != 0) {
            return(0);      /* Report error. */
        }
        return(1);
    }
    
/*
    The following demonstrates how the REG_NOTBOL flag could be used with regexec() to find all substrings 
    in a line that match a pattern supplied by a user. (For simplicity of the example, very little error 
    checking is done.)
*/

    (void) regcomp (&re, pattern, 0);
    /* This call to regexec() finds the first match on the line. */
    error = regexec (&re, &buffer[0], 1, &pm, 0);
    while (error == 0) {  /* While matches found. */
        /* Substring found between pm.rm_so and pm.rm_eo. */
        /* This call to regexec() finds the next match. */
        error = regexec (&re, buffer + pm.rm_eo, 1, &pm, REG_NOTBOL);
    }


