

#ifndef _QORE_EXECARGLIST_H

#define _QORE_EXECARGLIST_H

#include <string.h>
#include <stdlib.h>

#define ARG_BLOCK 10
class ExecArgList {
   private:
      char **arg;
      int allocated;
      int len;

      inline char *getString(char *start, int size)
      {
         char *str = (char *)malloc(sizeof(char) * (size + 1));
         strncpy(str, start, size);
         str[size] = '\0';
         //printd(5, "ExecArgList::getString() %d: %s\n", len, str);
         return str;
      }

      inline void addArg(char *str)
      {
         // resize args
         if (len == allocated)
         {
            allocated += ARG_BLOCK;
            arg = (char **)realloc(arg, sizeof(char *) * allocated);
         }
         arg[len] = str;
         len++;
      }

   public:
      inline ExecArgList(char *str)
      {
         allocated = 0;
         len = 0;
         arg = NULL;
         char *start = str, *p = str;
	 int quote = 0;

         while (*p)
         {
	    if (start == p && !quote && (*p == '\'' || *p == '\"'))
	    {
	       quote = *p;
	       start = p + 1;
	       continue;
	    }
	    p++;
	    if (quote && (*p == '\'' || *p == '\"') && *p == quote)
	    {
	       // move characters down to quote position
	       memmove(p, p + 1, strlen(p));
	       quote = 0;
	       p--;
	       continue;
	    }
	    else if (!quote && *p == ' ')
	    {
	       addArg(getString(start, p - start));
	       start = p + 1;
	    }
         }
         if (*start)
            addArg(getString(start, strlen(start)));
         // terminate list
         addArg(NULL);
      }
      inline ~ExecArgList()
      {
         if (arg)
         {
            for (int i = 0; i < len; i++)
               if (arg[i])
                  free(arg[i]);
            free(arg);
         }
      }
      inline char *getFile()
      {
         if (len)
            return arg[0];
         return NULL;
      }
      inline char **getArgs()
      {
         return arg;
      }
};

#endif
