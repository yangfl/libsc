#ifndef SC_THREADNAME_H
#define SC_THREADNAME_H 1

#include <stddef.h>

#include "sc/cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file */


// size of thread name, including terminating null
#define SC_THREADNAME_SIZE 16

SC_API __nonnull() __attr_access((__write_only__, 1, 2))
/**
 * @brief Get current thread name.
 *
 * @param[out] buf Buffer to store thread name.
 * @param size Size of @p buf .
 * @return 0 on success, error otherwise.
 */
int sc_threadname_get (char *__restrict buf, size_t size) __THROW;
SC_API __nonnull() __attr_access((__read_only__, 1))
__attr_format((gnu_printf, 1, 2))
/**
 * @brief Set current thread name.
 *
 * @param format Format string.
 * @param ... Format arguments.
 * @return 0 on success, error otherwise.
 */
int sc_threadname_set (const char *__restrict format, ...) __THROW;
SC_API __nonnull() __attr_access((__read_only__, 1))
__attr_format((gnu_printf, 1, 2))
/**
 * @brief Append string to current thread name.
 *
 * @param format Format string.
 * @param ... Format arguments.
 * @return 0 on success, error otherwise.
 */
int sc_threadname_append (const char *__restrict format, ...) __THROW;


#ifdef __cplusplus
}
#endif

#endif /* SC_THREADNAME_H */
