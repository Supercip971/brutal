#pragma once

#include <brutal/task/types.h>
#include "kernel/task.h"

typedef struct
{
    CpuId id;
    bool present;

    bool retain_enable;
    int retain_depth;

    Task *idle;
    Task *current;
    Task *next;
} Cpu;

Cpu *cpu(CpuId id);

int cpu_count(void);

Cpu *cpu_self(void);

CpuId cpu_self_id(void);

void cpu_resched_other(CpuId cpu);

void cpu_begin_interrupt(void);

void cpu_end_interrupt(void);

void cpu_retain_disable(void);

void cpu_retain_enable(void);

void cpu_enable_interrupts(void);

void cpu_disable_interrupts(void);

void cpu_retain_interrupts(void);

void cpu_release_interrupts(void);
