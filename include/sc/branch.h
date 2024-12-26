#ifndef SC_BRANCH_H
#define SC_BRANCH_H 1


#define sc_likely(x) __builtin_expect(!!(x), 1)
#define sc_unlikely(x) __builtin_expect(!!(x), 0)
#define sc_promise(x) if (!(x)) __builtin_unreachable()


#endif /* SC_BRANCH_H */
