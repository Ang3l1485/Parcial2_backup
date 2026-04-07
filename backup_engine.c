#include "smart_copy.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

static void smart_copy_log(int priority, const char *fmt, ...) {
#if SMART_COPY_ENABLE_SYSLOG
    va_list args;
    va_start(args, fmt);
    openlog("smart_backup", LOG_PID, LOG_USER);
    vsyslog(priority, fmt, args);
    closelog();
    va_end(args);
#else
    (void)priority;
    (void)fmt;
#endif
}

static int close_fd_checked(int fd) {
    if (fd < 0) {
        return 0;
    }

    while (close(fd) == -1) {
        if (errno == EINTR) {
            continue;
        }
        return -1;
    }

    return 0;
}

static int write_all(int fd, const char *buffer, size_t total) {
    size_t written_total = 0;

    while (written_total < total) {
        ssize_t written_now = write(fd, buffer + written_total, total - written_total);

        if (written_now < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        if (written_now == 0) {
            errno = EIO;
            return -1;
        }

        written_total += (size_t)written_now;
    }

    return 0;
}

int sys_smart_copy(const char *src, const char *dest) {
    int fd_src = -1;
    int fd_dest = -1;
    int result = SMART_COPY_OK;
    char buffer[SMART_COPY_BUFFER_SIZE];
    struct stat src_stat;
    int saved_errno = 0;

    if (src == NULL || dest == NULL) {
        errno = EINVAL;
        return SMART_COPY_ERR_INVALID_ARG;
    }

    if (src[0] == '\0' || dest[0] == '\0') {
        errno = EINVAL;
        return SMART_COPY_ERR_INVALID_ARG;
    }

    smart_copy_log(LOG_INFO, "copy start src='%s' dest='%s'", src, dest);

    if (stat(src, &src_stat) == -1) {
        result = SMART_COPY_ERR_STAT;
        goto cleanup;
    }

    if (!S_ISREG(src_stat.st_mode)) {
        errno = EINVAL;
        result = SMART_COPY_ERR_NOT_REGULAR;
        goto cleanup;
    }

    fd_src = open(src, O_RDONLY);
    if (fd_src == -1) {
        result = SMART_COPY_ERR_OPEN_SRC;
        goto cleanup;
    }

    fd_dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (fd_dest == -1) {
        result = SMART_COPY_ERR_OPEN_DEST;
        goto cleanup;
    }

    for (;;) {
        ssize_t bytes_read = read(fd_src, buffer, sizeof(buffer));

        if (bytes_read < 0) {
            if (errno == EINTR) {
                continue;
            }
            result = SMART_COPY_ERR_READ;
            goto cleanup;
        }

        if (bytes_read == 0) {
            break;
        }

        if (write_all(fd_dest, buffer, (size_t)bytes_read) == -1) {
            result = SMART_COPY_ERR_WRITE;
            goto cleanup;
        }
    }

cleanup:
    saved_errno = errno;

    if (close_fd_checked(fd_src) == -1 && result == SMART_COPY_OK) {
        result = SMART_COPY_ERR_CLOSE;
        saved_errno = errno;
    }

    if (close_fd_checked(fd_dest) == -1 && result == SMART_COPY_OK) {
        result = SMART_COPY_ERR_CLOSE;
        saved_errno = errno;
    }

    if (result == SMART_COPY_OK) {
        smart_copy_log(LOG_INFO, "copy success src='%s' dest='%s'", src, dest);
    } else {
        smart_copy_log(LOG_ERR, "copy error code=%d src='%s' dest='%s' errno=%d (%s)",
                       result, src, dest, saved_errno, strerror(saved_errno));
    }

    errno = saved_errno;
    return result;
}
