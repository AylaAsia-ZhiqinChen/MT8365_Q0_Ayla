/* xargs.c - Run command with arguments taken from stdin.
 *
 * Copyright 2011 Rob Landley <rob@landley.net>
 *
 * See http://opengroup.org/onlinepubs/9699919799/utilities/xargs.html
 *
 * TODO: Rich's whitespace objection, env size isn't fixed anymore.
 * TODO: -x	Exit if can't fit everything in one command
 * TODO: -L	Max number of lines of input per command

USE_XARGS(NEWTOY(xargs, "^I:E:ptrn#<1s#0[!0E]", TOYFLAG_USR|TOYFLAG_BIN))

config XARGS
  bool "xargs"
  default y
  help
    usage: xargs [-0prt] [-s NUM] [-n NUM] [-E STR] COMMAND...

    Run command line one or more times, appending arguments from stdin.

    If command exits with 255, don't launch another even if arguments remain.

    -0	Each argument is NULL terminated, no whitespace or quote processing
    -E	Stop at line matching string
    -n	Max number of arguments per command
    -p	Prompt for y/n from tty before running each command
    -r	Don't run command with empty input
    -s	Size in bytes per command line
    -t	Trace, print command line to stderr

config XARGS_PEDANTIC
  bool "TODO xargs pedantic posix compatability"
  default n
  depends on XARGS
  help
    This version supports insane posix whitespace handling rendered obsolete
    by -0 mode.
*/

#define FOR_xargs
#include "toys.h"

GLOBALS(
  long s, n;
  char *E, *I;

  long entries, bytes;
  char delim;
)

// If out==NULL count TT.bytes and TT.entries, stopping at max.
// Otherwise, fill out out[]

// Returning NULL means need more data.
// Returning char * means hit data limits, start of data left over
// Returning 1 means hit data limits, but consumed all data
// Returning 2 means hit -E STR

static char *handle_entries(char *data, char **entry)
{
  if (TT.delim) {
    char *s = data;

    // Chop up whitespace delimited string into args
    while (*s) {
      char *save;

      while (isspace(*s)) {
        if (entry) *s = 0;
        s++;
      }

      if (TT.n && TT.entries >= TT.n)
        return *s ? s : (char *)1;

      if (!*s) break;
      save = s;

      TT.bytes += sizeof(char *);

      for (;;) {
        if (++TT.bytes >= TT.s && TT.s) return save;
        if (!*s || isspace(*s)) break;
        s++;
      }
      if (TT.E) {
        int len = s-save;
        if (len == strlen(TT.E) && !strncmp(save, TT.E, len))
          return (char *)2;
      }
      if (entry) entry[TT.entries] = save;
      ++TT.entries;
    }

  // -0 support
  } else {
    TT.bytes += sizeof(char *)+strlen(data)+1;
    if (TT.s && TT.bytes >= TT.s) return data;
    if (TT.n && TT.entries >= TT.n) return data;
    if (entry) entry[TT.entries] = data;
    TT.entries++;
  }

  return NULL;
}

void xargs_main(void)
{
  struct double_list *dlist = NULL, *dtemp;
  int entries, bytes, done = 0, status;
  char *data = NULL, **out;
  pid_t pid;
  long posix_max_bytes;

  // POSIX requires that we never hit the ARG_MAX limit, even if we try to
  // with -s. POSIX also says we have to reserve 2048 bytes "to guarantee
  // that the invoked utility has room to modify its environment variables
  // and command line arguments and still be able to invoke another utility",
  // though obviously that's not really something you can guarantee.
  posix_max_bytes = sysconf(_SC_ARG_MAX) - environ_bytes() - 2048;
  if (!TT.s || TT.s > posix_max_bytes) TT.s = posix_max_bytes;

  if (!FLAG(0)) TT.delim = '\n';

  // If no optargs, call echo.
  if (!toys.optc) {
    free(toys.optargs);
    *(toys.optargs = xzalloc(2*sizeof(char *)))="echo";
    toys.optc = 1;
  }

  for (entries = 0, bytes = -1; entries < toys.optc; entries++, bytes++)
    bytes += strlen(toys.optargs[entries]);

  // Loop through exec chunks.
  while (data || !done) {
    int doit = 1;

    TT.entries = 0;
    TT.bytes = bytes;

    // Loop reading input
    for (;;) {

      // Read line
      if (!data) {
        ssize_t l = 0;
        l = getdelim(&data, (size_t *)&l, TT.delim, stdin);

        if (l<0) {
          data = 0;
          done++;
          break;
        }
      }
      dlist_add(&dlist, data);

      // Count data used
      data = handle_entries(data, NULL);
      if (!data) continue;
      if (data == (char *)2) done++;
      if ((long)data <= 2) data = 0;
      else data = xstrdup(data);

      break;
    }

    if (TT.entries == 0 && FLAG(r)) continue;

    // Accumulate cally thing

    if (data && !TT.entries) error_exit("argument too long");
    out = xzalloc((entries+TT.entries+1)*sizeof(char *));

    // Fill out command line to exec
    memcpy(out, toys.optargs, entries*sizeof(char *));
    TT.entries = 0;
    TT.bytes = bytes;
    if (dlist) dlist->prev->next = 0;
    for (dtemp = dlist; dtemp; dtemp = dtemp->next)
      handle_entries(dtemp->data, out+entries);

    if (FLAG(p) || FLAG(t)) {
      int i;

      for (i = 0; out[i]; ++i) fprintf(stderr, "%s ", out[i]);
      if (FLAG(p)) {
        fprintf(stderr, "?");
        doit = yesno(0);
      } else fprintf(stderr, "\n");
    }

    if (doit) {
      if (!(pid = XVFORK())) {
        xclose(0);
        open("/dev/null", O_RDONLY);
        xexec(out);
      }
      waitpid(pid, &status, 0);
      status = WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status)+127;
    }

    // Abritrary number of execs, can't just leak memory each time...
    while (dlist) {
      struct double_list *dtemp = dlist->next;

      free(dlist->data);
      free(dlist);
      dlist = dtemp;
    }
    free(out);
  }
}
