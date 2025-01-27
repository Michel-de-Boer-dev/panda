#ifndef QEMU_LOG_H
#define QEMU_LOG_H


/* Private global variables, don't use */
extern FILE *qemu_logfile;
extern long qemu_loglevel;

/* 
 * The new API:
 *
 */

/* Log settings checking macros: */

/* Returns true if qemu_log() will really write somewhere
 */
static inline bool qemu_log_enabled(void)
{
    return qemu_logfile != NULL;
}

/* Returns true if qemu_log() will write somewhere else than stderr
 */
static inline bool qemu_log_separate(void)
{
    return qemu_logfile != NULL && qemu_logfile != stderr;
}

#define CPU_LOG_TB_OUT_ASM (1UL << 0)
#define CPU_LOG_TB_IN_ASM  (1UL << 1)
#define CPU_LOG_TB_OP      (1UL << 2)
#define CPU_LOG_TB_OP_OPT  (1UL << 3)
#define CPU_LOG_INT        (1UL << 4)
#define CPU_LOG_EXEC       (1UL << 5)
#define CPU_LOG_PCALL      (1UL << 6)
#define CPU_LOG_TB_CPU     (1UL << 8)
#define CPU_LOG_RESET      (1UL << 9)
#define LOG_UNIMP          (1UL << 10)
#define LOG_GUEST_ERROR    (1UL << 11)
#define CPU_LOG_MMU        (1UL << 12)
#define CPU_LOG_TB_NOCHAIN (1UL << 13)
#define CPU_LOG_PAGE       (1UL << 14)
#define LOG_TRACE          (1UL << 15)
#define CPU_LOG_TB_OP_IND  (1UL << 16)
#define CPU_LOG_TAINT_OPS  (1UL << 28)
#define CPU_LOG_RR         (1UL << 29)
#define CPU_LOG_LLVM_IR    (1UL << 30)
#define CPU_LOG_LLVM_ASM   (1UL << 31)
#define LOG_AVATAR         (1UL << 32)

/* Returns true if a bit is set in the current loglevel mask
 */
static inline bool qemu_loglevel_mask(long mask)
{
    return (qemu_loglevel & mask) != 0;
}

/* Lock output for a series of related logs.  Since this is not needed
 * for a single qemu_log / qemu_log_mask / qemu_log_mask_and_addr, we
 * assume that qemu_loglevel_mask has already been tested, and that
 * qemu_loglevel is never set when qemu_logfile is unset.
 */

static inline void qemu_log_lock(void)
{
    qemu_flockfile(qemu_logfile);
}

static inline void qemu_log_unlock(void)
{
    qemu_funlockfile(qemu_logfile);
}

/* Logging functions: */

/* main logging function
 */
int GCC_FMT_ATTR(1, 2) qemu_log(const char *fmt, ...);

/* vfprintf-like logging function
 */
static inline void GCC_FMT_ATTR(1, 0)
qemu_log_vprintf(const char *fmt, va_list va)
{
    if (qemu_logfile) {
        vfprintf(qemu_logfile, fmt, va);
    }
}

/* log only if a bit is set on the current loglevel mask:
 * @mask: bit to check in the mask
 * @fmt: printf-style format string
 * @args: optional arguments for format string
 */
#define qemu_log_mask(MASK, FMT, ...)                   \
    do {                                                \
        {       \
            qemu_log(FMT, ## __VA_ARGS__);              \
        }                                               \
    } while (0)

/* log only if a bit is set on the current loglevel mask
 * and we are in the address range we care about:
 * @mask: bit to check in the mask
 * @addr: address to check in dfilter
 * @fmt: printf-style format string
 * @args: optional arguments for format string
 */
#define qemu_log_mask_and_addr(MASK, ADDR, FMT, ...)    \
    do {                                                \
        if (unlikely(qemu_loglevel_mask(MASK)) &&       \
                     qemu_log_in_addr_range(ADDR)) {    \
            qemu_log(FMT, ## __VA_ARGS__);              \
        }                                               \
    } while (0)

/* Maintenance: */

/* define log items */
typedef struct QEMULogItem {
    long mask;
    const char *name;
    const char *help;
} QEMULogItem;

extern const QEMULogItem qemu_log_items[];

void qemu_set_log(long log_flags);
void qemu_log_needs_buffers(void);
void qemu_set_log_filename(const char *filename, Error **errp);
void qemu_set_dfilter_ranges(const char *ranges, Error **errp);
bool qemu_log_in_addr_range(uint64_t addr);
long qemu_str_to_log_mask(const char *str);

/* Print a usage message listing all the valid logging categories
 * to the specified FILE*.
 */
void qemu_print_log_usage(FILE *f);

/* fflush() the log file */
void qemu_log_flush(void);
/* Close the log file */
void qemu_log_close(void);

#endif
