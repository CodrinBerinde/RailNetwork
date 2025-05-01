/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include "qtrvsim_unistd.h"

#define UNUSED_PARAM(symbol) ((void)(symbol))


/* Open flags definition for Linux kernel copied from musl-libc */

#define KERNEL_O_CREAT        0100
#define KERNEL_O_EXCL         0200
#define KERNEL_O_NOCTTY       0400
#define KERNEL_O_TRUNC       01000
#define KERNEL_O_APPEND      02000
#define KERNEL_O_NONBLOCK    04000
#define KERNEL_O_DSYNC      010000
#define KERNEL_O_SYNC     04010000
#define KERNEL_O_RSYNC    04010000
#define KERNEL_O_DIRECTORY 0200000
#define KERNEL_O_NOFOLLOW  0400000
#define KERNEL_O_CLOEXEC  02000000

#define KERNEL_O_PATH 010000000

#define KERNEL_O_SYNC1 040000

#define KERNEL_O_ACCMODE (03 | KERNEL_O_PATH)
#define KERNEL_O_RDONLY 00
#define KERNEL_O_WRONLY 01
#define KERNEL_O_RDWR 02

#define TARGET_AT_FDCWD -100

/* Register name - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");


int _read(int file, char *ptr, int len);
int
_read (int file,
       char * ptr,
       int len)
{
	register int __a7 asm("a7") = __NR_read;
	register int __a0 asm("a0") = file;
	register char * __a1 asm("a1") = ptr;
	register int __a2 asm("a2") = len;
	__asm__ volatile (
	"ecall\n\t"
	: "=r" (__a0)
	: "r" (__a7), "r" (__a0), "r" (__a1), "r"(__a2)
	: );
	return __a0 < 0 ? -1: __a0;
}

int _write(int file, const char *ptr, int len);
int
_write (int    file,
	const char * ptr,
	int    len)
{
	register int __a7 asm("a7") = __NR_write;
	register int __a0 asm("a0") = file;
	register const char * __a1 asm("a1") = ptr;
	register int __a2 asm("a2") = len;
	__asm__ volatile (
	"ecall\n\t"
	: "=r" (__a0)
	: "r" (__a7), "r" (__a0), "r" (__a1), "r"(__a2)
	: );
	return __a0 < 0 ? -1: __a0;
}


int
_lseek (int file,
	int pos,
	int dir)
{
	UNUSED_PARAM(file);
	UNUSED_PARAM(pos);
	UNUSED_PARAM(dir);

	return -1;
}

int _openat(int fd, const char *path, int flags,	...);
int
_openat (int fd, const char * path,
       int          flags,
       ...)
{
	register int __a7 asm("a7") = __NR_openat;
	// use fd = -100 for normal open behaviour. Full openat not supported.
	register int __a0 asm("a0") = fd;
	register const char * __a1 asm("a1") = path;
	register int __a2 asm("a2") = 0;
	register int __a3 asm("a3");

#ifdef O_CREAT
	if (flags & O_CREAT)
		__a2 |= KERNEL_O_CREAT;
#endif
#ifdef O_EXCL
	if (flags & O_EXCL)
		__a2 |= KERNEL_O_EXCL;
#endif
#ifdef O_NOCTTY
	if (flags & O_NOCTTY)
		__a2 |= KERNEL_O_NOCTTY;
#endif
#ifdef O_TRUNC
	if (flags & O_TRUNC)
		__a2 |= KERNEL_O_TRUNC;
#endif
#ifdef O_APPEND
	if (flags & O_APPEND)
		__a2 |= KERNEL_O_APPEND;
#endif
#ifdef O_NONBLOCK
	if (flags & O_NONBLOCK)
		__a2 |= KERNEL_O_NONBLOCK;
#endif
#ifdef O_DSYNC
	if (flags & O_DSYNC)
		__a2 |= KERNEL_O_DSYNC;
#endif
#ifdef O_SYNC
	if (flags & O_SYNC)
		__a2 |= KERNEL_O_SYNC;
#endif
#ifdef O_RSYNC
	if (flags & O_RSYNC)
		__a2 |= KERNEL_O_RSYNC;
#endif
#ifdef O_DIRECTORY
	if (flags & O_DIRECTORY)
		__a2 |= KERNEL_O_DIRECTORY;
#endif
#ifdef O_NOFOLLOW
	if (flags & O_NOFOLLOW)
		__a2 |= KERNEL_O_NOFOLLOW;
#endif
#ifdef O_CLOEXEC
	if (flags & O_CLOEXEC)
		__a2 |= KERNEL_O_CLOEXEC;
#endif

	if (flags & O_CREAT) {
		va_list list;
		va_start(list, flags);
		__a3 = va_arg(list, int);
		va_end(list);
	}

	__asm__ volatile (
	"ecall\n\t"
	: "=r" (__a0)
	: "r" (__a7), "r" (__a0), "r" (__a1), "r"(__a2), "r"(__a3)
	: );
	return __a0 < 0 ? -1: __a0;
}

int _close(int file);
int
_close (int file)
{
	register int __a7 asm("a7") = __NR_close;
	register int __a0 asm("a0") = file;
	__asm__ volatile (
	"ecall\n\t"
	: "=r" (__a0)
	: "r" (__a7), "r" (__a0)
	: );
	return __a0 < 0 ? -1: __a0;
}

void _exit(int n);
void
_exit (int n)
{
	register int __a7 asm("a7") = __NR_exit;
	register int __a0 asm("a0") = n;

	while(1) __asm__ volatile (
	"ecall\n\t"
	:
	: "r" (__a7), "r" (__a0)
	: );
}

void abort(void);
void abort(void)
{
	while(1) __asm__ volatile (
	"ebreak\n\t"
	:
	:
	: );
}

int _kill(int n, int m);
int
_kill (int n, int m)
{
  UNUSED_PARAM(n);
  UNUSED_PARAM(m);
  return -1;
}

int _getpid(void);
int
_getpid (void)
{
  return 1;
}


void *_sbrk(ptrdiff_t incr);
void *
_sbrk (ptrdiff_t incr)
{
  extern char    _heap_stack_start;
  static char   *heap_end;
  char *        prev_heap_end;

  incr=(incr+3) & ~3;

  if (heap_end == NULL)
    heap_end = & _heap_stack_start;

  prev_heap_end = heap_end;

  if ((heap_end + incr > stack_ptr) && (&_heap_stack_start < stack_ptr))
    {
      /* Some of the libstdc++-v3 tests rely upon detecting
	 out of memory errors, so do not abort here.  */
#if 1
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);

      abort ();
#else
      errno = ENOMEM;
      return (caddr_t) -1;
#endif
    }

  heap_end += incr;

  return /*(caddr_t)*/ prev_heap_end;
}

int _fstat(int file, struct stat *st);
int
_fstat (int file, struct stat * st)
{
  UNUSED_PARAM(file);
  UNUSED_PARAM(st);
  return -1;
}

int _stat (const char *fname, struct stat *st);
int _stat (const char *fname, struct stat *st)
{
  UNUSED_PARAM(fname);
  UNUSED_PARAM(st);
  return -1;
}

int _link(const char *path1, const char *path2);
int
_link (const char *path1,
       const char *path2)
{
  UNUSED_PARAM(path1);
  UNUSED_PARAM(path2);
  return -1;
}

int _unlink(const char *path);
int
_unlink (const char *path)
{
  UNUSED_PARAM(path);
  return -1;
}

void _raise(void);
void
_raise (void)
{
  return;
}

int _gettimeofday(struct timeval *tp, struct timezone *tzp);
int
_gettimeofday (struct timeval *tp,
	       struct timezone *tzp)
{

  if(tp)
    {
      tp->tv_sec = 0;
      tp->tv_usec = 0;
    }
  /* Return fixed data for the timezone.  */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}

/* Return a clock that ticks at 100Hz.  */
clock_t _times(struct tms *tp);
clock_t
_times (struct tms * tp)
{
  clock_t timeval = 0;

  if (tp)
    {
      tp->tms_utime  = timeval;	/* user time */
      tp->tms_stime  = 0;	/* system time */
      tp->tms_cutime = 0;	/* user time, children */
      tp->tms_cstime = 0;	/* system time, children */
    }

  return timeval;
};


int isatty(int fd);
int
isatty (int fd)
{
  UNUSED_PARAM(fd);
  return 1;
}

int _system(const char *s);
int
_system (const char *s)
{
  UNUSED_PARAM(s);
  return -1;
}

int _rename(const char *oldpath, const char *newpath);
int
_rename (const char * oldpath, const char * newpath)
{
  UNUSED_PARAM(oldpath);
  UNUSED_PARAM(newpath);
  return -1;
}

/* extern (.*) ([^ ]*) _PARAMS \(\(struct _reent \*(,*)(.*)\)\); */
/* \1 \2 (struct _reent *\3\4) { return \2(\4);} */

struct _reent;

int _close_r(struct _reent *reent, int file)
{ UNUSED_PARAM(reent); return _close(file);}
/*
  int _fcntl_r(struct _reent *, int, int, int)
  { return _fcntl( int, int, int);}
*/
int _fstat_r(struct _reent *reent, int file, struct stat *st);
int _fstat_r(struct _reent *reent, int file, struct stat *st)
{ UNUSED_PARAM(reent); return _fstat(file, st);}

int _getpid_r(struct _reent *reent);
int _getpid_r(struct _reent *reent)
{ UNUSED_PARAM(reent); return _getpid();}

int _kill_r(struct _reent *reent, int n, int m);
int _kill_r(struct _reent *reent, int n, int m)
{ UNUSED_PARAM(reent); return _kill(n, m);}

int _link_r(struct _reent *reent, const char *path1, const char *path2);
int _link_r(struct _reent *reent, const char *path1, const char *path2)
{ UNUSED_PARAM(reent); return _link(path1, path2);}

_off_t _lseek_r(struct _reent *reent, int file, _off_t pos, int dir);
_off_t _lseek_r(struct _reent *reent, int file, _off_t pos, int dir)
{ UNUSED_PARAM(reent); return _lseek(file, pos, dir);}

int _open_r(struct _reent *reent, const char *path, int flags, int opts);
int _open_r(struct _reent *reent, const char *path, int flags, int opts)
{ UNUSED_PARAM(reent); return _openat(TARGET_AT_FDCWD, path, flags, opts);}

int _openat_r(struct _reent *reent, int fd, const char *path, int flags, int opts);
int _openat_r(struct _reent *reent, int fd, const char *path, int flags, int opts)
{ UNUSED_PARAM(reent); return _openat(fd, path, flags, opts);}

_ssize_t _read_r(struct _reent *reent, int file, void *ptr, size_t len);
_ssize_t _read_r(struct _reent *reent, int file, void *ptr, size_t len)
{ UNUSED_PARAM(reent); return _read(file, ptr, len);}

void *_sbrk_r(struct _reent *reent, ptrdiff_t incr);
void *_sbrk_r(struct _reent *reent, ptrdiff_t incr)
{ UNUSED_PARAM(reent); return _sbrk(incr);}

int _stat_r(struct _reent *reent, const char *path, struct stat *st);
int _stat_r(struct _reent *reent, const char *path, struct stat *st)
{ UNUSED_PARAM(reent); return _stat(path, st);}

int _unlink_r(struct _reent *reent, const char *path);
int _unlink_r(struct _reent *reent, const char *path)
{ UNUSED_PARAM(reent); return _unlink(path);}

_ssize_t _write_r(struct _reent *reent, int file, const void *ptr, size_t len);
_ssize_t _write_r(struct _reent *reent, int file, const void *ptr, size_t len)
{ UNUSED_PARAM(reent); return _write(file, ptr, len);}

/* FIXME: TDK added dummy functions. Need to implement. */
int _isatty(int fd);
int _isatty(int fd)
{
  UNUSED_PARAM(fd);
  return -1;
}

int _swistat(int fd, struct stat *st);
int _swistat(int fd, struct stat *st)
{
  UNUSED_PARAM(fd);
  UNUSED_PARAM(st);
  return -1;
}

/* Return a clock that ticks at 100Hz.  */
clock_t _clock(void);
clock_t _clock(void)
{
  clock_t timeval=0;
  return timeval;
}

int _swiclose(int fh);
int _swiclose(int fh)
{
  UNUSED_PARAM(fh);
  return -1;
}

int _swiopen(const char *path, int flags);
int _swiopen(const char *path, int flags)
{
  UNUSED_PARAM(path);
  UNUSED_PARAM(flags);
  return -1;
}

int _swiwrite(int fh, char *ptr, int len);
int _swiwrite(int fh, char *ptr, int len)
{
  UNUSED_PARAM(fh);
  UNUSED_PARAM(ptr);
  UNUSED_PARAM(len);
  return -1;
}

int _swilseek(int fd, int ptr, int dir);
int _swilseek(int fd, int ptr, int dir)
{
  UNUSED_PARAM(fd);
  UNUSED_PARAM(ptr);
  UNUSED_PARAM(dir);
  return -1;
}

int _swiread(int fh, char *ptr,	int len);
int _swiread(int fh, char *ptr,	int len)
{
  UNUSED_PARAM(fh);
  UNUSED_PARAM(ptr);
  UNUSED_PARAM(len);
  return -1;
}

void initialise_monitor_handles(void);
void initialise_monitor_handles(void)
{
}
