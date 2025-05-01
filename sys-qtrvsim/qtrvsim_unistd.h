// ******************************************************************
//  QtRvSim emulator https://github.com/cvut/qtrvsim support filees
//
//  qtrvsim_unistd.h  - definition of the QtRvSim syscall numbers
//
//  (C) Copyright 2019 - 2022 by Pavel Pisa
//      e-mail:   pisa@cmp.felk.cvut.cz
//      homepage: http://cmp.felk.cvut.cz/~pisa
//      license:  any combination of GPL, LGPL, MPL or BSD licenses
//
// ******************************************************************

#ifndef QTRVSIM_UNISTD_H
#define QTRVSIM_UNISTD_H

// Linux kernel compatible system calls subset

#define __NR_exit         93  // void exit(int status)
#define __NR_read         63  // ssize_t read(int fd, void *buf, size_t count)
#define __NR_write        64  // ssize_t write(int fd, const void *buf, size_t count)
#define __NR_close        57  // int close(int fd)
#define __NR_openat       56  // int openat(int fd, const char *pathname, int flags, mode_t mode)
        // use fd = -100 for normal open behaviour. Full openat not supported.
#define __NR_brk         214 // void * brk(void *addr)
#define __NR_ftruncate64  46  // int ftruncate64(int fd, off_t length)
#define __NR_readv        65  // ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
#define __NR_writev       66  // ssize_t writev(int fd, const struct iovec *iov, int iovcnt)

#endif /*QTRVSIM_UNISTD_H*/
