#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <protos/boot.h>
#include <protos/event.h>
#include <protos/hw.h>
#include <protos/pci.h>
#include <protos/system.h>
#include <protos/window.h>
#include "system/bus.h"

/* --- System Server Protocol ----------------------------------------------- */

static SystemError system_server_expose_handler(void *self, IpcCap const *req, void *, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    bus_expose(bus, *req);
    return IPC_SUCCESS;
}

static SystemError system_server_consume_handler(void *self, IpcProto const *req, IpcCap *resp, MAYBE_UNUSED Alloc *alloc)
{
    Bus *bus = self;
    *resp = bus_consume(bus, *req);

    if (resp->addr.id == BR_ID_NIL)
    {
        return SYSTEM_NOT_FOUND;
    }

    return IPC_SUCCESS;
}

static SystemServerVTable _system_server_vtable = {
    system_server_expose_handler,
    system_server_consume_handler,
};

/* --- Boot Info Protocol --------------------------------------------------- */

static BootError boot_info_rsdp_handler(void *self, void *, uintptr_t *resp, Alloc *)
{
    Handover *handover = self;
    *resp = handover->rsdp;
    return IPC_SUCCESS;
}

static BootInfoVTable _bus_info_vtable = {
    boot_info_rsdp_handler,
};

/* --- Hw Display Protocol -------------------------------------------------- */

HwError hw_display_surface_handler(void *self, void *, BalFb *resp, Alloc *)
{
    Handover *handover = self;
    HandoverFramebuffer const *framebuffer = &handover->framebuffer;

    size_t fb_size = align_up$(framebuffer->height * framebuffer->pitch, MEM_PAGE_SIZE);

    bal_fb_init_pmm(
        resp,
        framebuffer->addr,
        fb_size,
        framebuffer->width,
        framebuffer->height,
        framebuffer->pitch,
        GFX_FMT_BGRA8888);

    return IPC_SUCCESS;
}

static HwDisplayVTable _hw_display_vtable = {
    nullptr,
    nullptr,
    hw_display_surface_handler,
};

/* --- Entry Point ---------------------------------------------------------- */

int br_main(Handover *handover)
{
    Bus bus = {};
    bus_init(&bus, handover, alloc_global());

    IpcComponent self = {};
    ipc_component_init(&self, alloc_global());

    IpcCap system_server_cap = system_server_provide(&self, &_system_server_vtable, &bus);
    bus_expose(&bus, system_server_cap);

    IpcCap boot_info_cap = boot_info_provide(&self, &_bus_info_vtable, handover);
    bus_expose(&bus, boot_info_cap);

    IpcCap hw_display_cap = hw_display_provide(&self, &_hw_display_vtable, handover);
    bus_expose(&bus, hw_display_cap);

    for (size_t i = 0; i < handover->modules.size; i++)
    {
        HandoverModule *mod = &handover->modules.module[i];

        Str name = str$(&mod->module_name);

        if (str_eq(str$("system"), name))
        {
            continue;
        }

        log$("Mounting component '{}'...", name);
        Unit unit = {};
        unit_init_from_module(&unit, *mod, alloc_global());
        bus_activate(&bus, &unit);
    }

    return ipc_component_run(&self);
}
