#include <ctype.h>

/*
 *----------------------------------------------------------------------
 *
 * Tol_StringCaseMatch --
 *
 *	See if a particular string matches a particular pattern. Allows case
 *	insensitivity.
 *
 * Results:
 *	The return value is 1 if string matches pattern, and 0 otherwise. The
 *	matching operation permits the following special characters in the
 *	pattern: *?\[].
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tol_StringCaseMatch(
    const char *str,		/* String. */
    const char *pattern,	/* Pattern, which may contain special
				 * characters. */
    int nocase)			/* 0 for case sensitive, 1 for insensitive */
{
    int p;
    char  ch1, ch2;

    while (1) {
	p = *pattern;

	/*
	 * See if we're at the end of both the pattern and the string. If so,
	 * we succeeded. If we're at the end of the pattern but not at the end
	 * of the string, we failed.
	 */

	if (p == '\0') {
	    return (*str == '\0');
	}
	if ((*str == '\0') && (p != '*')) {
	    return 0;
	}

	/*
	 * Check for a "*" as the next pattern character. It matches any
	 * substring. We handle this by calling ourselves recursively for each
	 * postfix of string, until either we match or we reach the end of the
	 * string.
	 */

	if (p == '*') {
	    /*
	     * Skip all successive *'s in the pattern
	     */

	    while (*(++pattern) == '*') {}
	    p = *pattern;
	    if (p == '\0') {
		return 1;
	    }

	    /*
	     * This is a special case optimization for single-byte utf.
	     */

            ch2 = ( nocase ? tolower( *pattern ) : *pattern );

	    while (1) {
		/*
		 * Optimization for matching - cruise through the string
		 * quickly if the next char in the pattern isn't a special
		 * character
		 */
		if ((p != '[') && (p != '?') && (p != '\\')) {
		    if (nocase) {
			while ( *str ) {
                          ch1 = *str;
			    if ( ch2==ch1 || ch2==tolower(ch1)) {
				break;
			    }
			    ++str;
			}
		    } else {
			/*
			 * There's no point in trying to make this code
			 * shorter, as the number of bytes you want to compare
			 * each time is non-constant.
			 */

			while (*str) {
                            ch1 = *str;
			    if (ch2 == ch1) {
				break;
			    }
			    ++str;
			}
		    }
		}
		if (Tol_StringCaseMatch(str, pattern, nocase)) {
		    return 1;
		}
		if (*str == '\0') {
		    return 0;
		}
                ch1 = *str;
		++str;
	    }
	}

	/*
	 * Check for a "?" as the next pattern character. It matches any
	 * single character.
	 */

	if (p == '?') {
	    pattern++;
            ch1 = *str;
            ++str;
	    continue;
	}

	/*
	 * Check for a "[" as the next pattern character. It is followed by a
	 * list of characters that are acceptable, or by a range (two
	 * characters separated by "-").
	 */

	if (p == '[') {
	    char startChar, endChar;

	    pattern++;
            ch1 = ( nocase ? tolower( *str ) : *str );
            ++str;
	    while (1) {
		if ((*pattern == ']') || (*pattern == '\0')) {
		    return 0;
		}
                startChar = ( nocase ? tolower( *pattern ) : *pattern );
                pattern++;
		if (*pattern == '-') {
		    pattern++;
		    if (*pattern == '\0') {
			return 0;
		    }
                    endChar = ( nocase ? tolower( *pattern ) : *pattern );
                    pattern++;
		    if (((startChar <= ch1) && (ch1 <= endChar))
			    || ((endChar <= ch1) && (ch1 <= startChar))) {
			/*
			 * Matches ranges of form [a-z] or [z-a].
			 */

			break;
		    }
		} else if (startChar == ch1) {
		    break;
		}
	    }
	    while (*pattern != ']') {
		if (*pattern == '\0') {
		    --pattern;
		    break;
		}
		pattern++;
	    }
	    pattern++;
	    continue;
	}

	/*
	 * If the next pattern character is '\', just strip off the '\' so we
	 * do exact matching on the character that follows.
	 */

	if (p == '\\') {
	    pattern++;
	    if (*pattern == '\0') {
		return 0;
	    }
	}

	/*
	 * There's no special character. Just make sure that the next bytes of
	 * each string match.
	 */

        ch1 = *str;
	str++;
        ch2 = *pattern;
	pattern++;
	if (nocase) {
	    if ( tolower(ch1) != tolower(ch2)) {
		return 0;
	    }
	} else if (ch1 != ch2) {
	    return 0;
	}
    }
}

#ifdef _TEST_STRING_MATCH_
#include <stdio.h>

// g++ -D_TEST_STRING_MATCH_ StringMatch.cpp -lstdc++

void Check( const char *str, const char *pattern, int nocase )
{
  printf( "%s -- %s vs %s = %d\n", nocase?"No Case":"Case", str, pattern,
          Tol_StringCaseMatch( str, pattern, nocase ) );
}

int main()
{
  Check( "A", "a",                            0 );
  Check( "A", "a",                            1 );
  Check( "C:\\tmp", "C:\\\\*",                1 );
  Check( "Hola que tal", "Hola*",             1 );
  Check( "Hola que tal", "*Hola*tal",         1 );
  Check( "Hola que tal", "Hola*",             0 );
  Check( "Hola que tal", "*Hola",             1 );
  Check( "Hola que tal", "*Hola",             0 );
  Check( "Hola que tal", "*Hola*tal",         0 );
  Check( "a", "[a-z]",                        0 );
  Check( "A", "[a-z]",                        1 );
  Check( "A", "[a-z]",                        0 );
  Check( "A", "[A-Z]",                        0 );
  Check( "Axyz[3]*", "[A-Z]*\\[[0-9]\\]\\*",              0 );
  return 0;
}
#endif
