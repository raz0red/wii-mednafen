#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gctypes.h>
#include <unistd.h>
#include <sys/stat.h>

#include <map>
#include <string>
using namespace std;

#include "gettext.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

static map<string,string> msgmap;

/* Expand some escape sequences found in the argument string.  */
static char *
expand_escape(const char *str)
{
	char *retval, *rp;
	const char *cp = str;

	retval = (char *) malloc(strlen(str) + 1);
	if (retval == NULL)
		return NULL;
	rp = retval;

	while (cp[0] != '\0' && cp[0] != '\\')
		*rp++ = *cp++;
	if (cp[0] == '\0')
		goto terminate;
	do
	{

		/* Here cp[0] == '\\'.  */
		switch (*++cp)
		{
		case '\"': /* " */
			*rp++ = '\"';
			++cp;
			break;
		case 'a': /* alert */
			*rp++ = '\a';
			++cp;
			break;
		case 'b': /* backspace */
			*rp++ = '\b';
			++cp;
			break;
		case 'f': /* form feed */
			*rp++ = '\f';
			++cp;
			break;
		case 'n': /* new line */
			*rp++ = '\n';
			++cp;
			break;
		case 'r': /* carriage return */
			*rp++ = '\r';
			++cp;
			break;
		case 't': /* horizontal tab */
			*rp++ = '\t';
			++cp;
			break;
		case 'v': /* vertical tab */
			*rp++ = '\v';
			++cp;
			break;
		case '\\':
			*rp = '\\';
			++cp;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		{
			int ch = *cp++ - '0';

			if (*cp >= '0' && *cp <= '7')
			{
				ch *= 8;
				ch += *cp++ - '0';

				if (*cp >= '0' && *cp <= '7')
				{
					ch *= 8;
					ch += *cp++ - '0';
				}
			}
			*rp = ch;
		}
			break;
		default:
			*rp = '\\';
			break;
		}

		while (cp[0] != '\0' && cp[0] != '\\')
			*rp++ = *cp++;
	} while (cp[0] != '\0');

	/* Terminate string.  */
	terminate: *rp = '\0';
	return retval;
}

static void setMSG(const char *msgid, const char *msgstr)
{
  if( msgid && msgstr )
  {
    char *msg = expand_escape(msgstr);
    msgmap[msgid] = msg;
    free( msg );
  }
}

static void gettextCleanUp(void)
{
  msgmap.clear();
}

bool LoadLanguage( const char* langfile )
{
  if( !langfile )
  {
    gettextCleanUp();
    return true;
  }

	char line[200];
	char *lastID = NULL;
	
  FILE *fp;
  fp = fopen( langfile, "rb" );
  if (fp == NULL) 
  {	
    return false;		
  }

	gettextCleanUp();

	while( fgets( line, sizeof(line), fp ) != 0 )
	{
    // lines starting with # are comments
		if (line[0] == '#')
			continue;

		if (strncmp(line, "msgid \"", 7) == 0)
		{
			char *msgid, *end;
			if (lastID)
			{
				free(lastID);
				lastID = NULL;
			}
			msgid = &line[7];
			end = strrchr(msgid, '"');
			if (end && end - msgid > 1)
			{
				*end = 0;
				lastID = strdup(msgid);
			}
		}
		else if (strncmp(line, "msgstr \"", 8) == 0)
		{
			char *msgstr, *end;

			if (lastID == NULL)
				continue;

			msgstr = &line[8];
			end = strrchr(msgstr, '"');
			if (end && end - msgstr > 1)
			{
				*end = 0;
				setMSG(lastID, msgstr);
			}
			free(lastID);
			lastID = NULL;
		}
	}

  fclose( fp );

	return true;
}

#ifdef TRACK_UNIQUE_MSGIDS
static const char* unique_msgids[4096];
static int msgids_count = 0;

static void add_msgid( const char *msgid )
{
  for( int i = 0; i < msgids_count; i++ )
  {
    if( !strcmp( msgid, unique_msgids[i] ) )
    {
      return;
    }
  }

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "adding: %d, %s\n", msgids_count, msgid );
#endif

  unique_msgids[msgids_count++] = strdup( msgid );
}

void dump_unique_msgids()
{
  for( int i = 0; i < msgids_count; i++ )
  {
#ifdef WII_NETTRACE
    net_print_string( NULL, 0, "msgid \"%s\"\n", unique_msgids[i] );
    net_print_string( NULL, 0, "msgstr \"\"\n\n" );
#endif
  }
}
#endif

const char *gettextmsg( const char *msgid )
{
#ifdef TRACK_UNIQUE_MSGIDS
  add_msgid( msgid );
#endif

  map<string,string>::iterator iter = msgmap.find(msgid);
  if( iter != msgmap.end() )
  {
    return iter->second.c_str();
  }
  return msgid;
}
