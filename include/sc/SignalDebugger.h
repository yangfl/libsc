#ifndef SC_SIGNALDEBUGGER_H
#define SC_SIGNALDEBUGGER_H 1

// #include <signal.h>
struct sigaction;
#include <stdbool.h>

#include "cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * Signal debugger, usually @c SIGSEGV .
 */


SC_API
/// Define this as strong symbol and set to @c true to disable signal hooking.
extern bool sc_no_signal_debugger;

/// Signal capturer.
struct ScSignalDebugger {
  /// Whether to wait debugger attachment when signal is received.
  bool attach;
  /// Exit status when signal is received.
  int exit_status;
  /// File descriptor for logging.
  int fd;
};

SC_API
/// The default signal debugger.
extern struct ScSignalDebugger sc_signal_debugger;

SC_API __attr_access((__write_only__, 2))
/**
 * @brief Capture, print backtrace and (optionally) wait for sc_debugger when
 *   signal is received.
 *
 * @param sig Signal number. If 0, @c SIGSEGV is used.
 * @param[out] oldact Previously associated action. Can be @c NULL.
 * @return 0 on success, -1 if @c sigaction() error.
 */
int sc_debug_signal (int sig, struct sigaction *oldact) __THROW;


#ifdef __cplusplus
}
#endif

#endif /* SC_SIGNALDEBUGGER_H */
