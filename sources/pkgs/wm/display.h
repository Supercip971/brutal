#pragma once

#include <bal/hw.h>
#include <brutal/gfx.h>
#include <protos/surface.h>

typedef struct
{
    SurfaceMode mode;
    BalMem mem;

    GfxSurface backbuffer;
    Gfx gfx;
} WmDisplay;

void wm_display_init(WmDisplay *self, SurfaceMode mode, BalMem mem, Alloc *alloc);

GfxBuf wm_display_frontbuffer(WmDisplay *self);

GfxBuf wm_display_backbuffer(WmDisplay *self);

void wm_display_deinit(WmDisplay *self);

Gfx *wm_display_begin(WmDisplay *self);

void wm_display_end(WmDisplay *self);
