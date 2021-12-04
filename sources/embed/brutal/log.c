#include <bal/abi.h>
#include <brutal/alloc/global.h>
#include <brutal/io/buf.h>
#include <brutal/sync.h>
#include <embed/log.h>

static bool _initialized = false;
static Lock _lock;
static Buf _buf = {};
static IoWriter _log = {};

void host_log_lock(void)
{
    lock_acquire(&_lock);
}

void host_log_unlock(void)
{
    lock_release(&_lock);
}

static IoResult host_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buf_push(&_buf, data[i]);

        if (data[i] == '\n')
        {
            br_log(&(BrLogArgs){
                .message = (char const *)buf_begin(&_buf),
                .size = buf_used(&_buf),
            });

            buf_clear(&_buf);
        }
    }

    return OK(IoResult, size);
}

IoWriter *host_log_writer(void)
{
    if (!_initialized)
    {
        _log.write = host_log_write;
        buf_init(&_buf, 128, alloc_global());
        _initialized = true;
    }

    return &_log;
}

void host_log_panic(void)
{
    br_exit(&(BrExitArgs){
        .task = BR_TASK_SELF,
        .exit_value = -1,
    });

    __builtin_unreachable();
}
