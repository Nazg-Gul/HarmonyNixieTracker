// Copyright (c) 2017, Sergey Sharybin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Sergey Sharybin (sergey.vfx@gmail.com)

#include "app_flash.h"

static SYS_MODULE_OBJ flash_driver_get(AppFlashData* app_flash_data) {
  return app_flash_data->system_objects->drvSst25Obj0;
}

static const SYS_FS_MEDIA_FUNCTIONS sst25_media_functions = {
    .mediaStatusGet     = DRV_SST25_MediaIsAttached,
    .mediaGeometryGet   = DRV_SST25_GeometryGet,
    .sectorRead         = DRV_SST25_BlockRead,
    .sectorWrite        = DRV_SST25_BlockEraseWrite,
    .eventHandlerset    = DRV_SST25_BlockEventHandlerSet,
    .commandStatusGet   = (void *)DRV_SST25_CommandStatus,
    .Read               = DRV_SST25_BlockRead,
    .erase              = DRV_SST25_BlockErase,
    .addressGet         = DRV_SST25_AddressGet,
    .open               = DRV_SST25_Open,
    .close              = DRV_SST25_Close,
    .tasks              = DRV_SST25_Tasks,
};


void APP_Flash_Initialize(AppFlashData* app_flash_data,
                          SYSTEM_OBJECTS* system_objects) {
  // TODO(sergey): Think about passing explicit flash handle.
  app_flash_data->system_objects = system_objects;
  app_flash_data->state = APP_FLASH_STATE_REGISTER_MEDIA;
}

void APP_Flash_Tasks(AppFlashData* app_flash_data) {
  switch (app_flash_data->state) {
    case APP_FLASH_STATE_REGISTER_MEDIA: {
      SYS_MODULE_OBJ flash = flash_driver_get(app_flash_data);
      if (DRV_SST25_Status(flash) == SYS_STATUS_READY) {
           if (SYS_FS_MEDIA_MANAGER_Register(
               (SYS_MODULE_OBJ)DRV_SST25_INDEX_0,
               (SYS_MODULE_INDEX)DRV_SST25_INDEX_0,
               &sst25_media_functions,
               SYS_FS_MEDIA_TYPE_SPIFLASH) != SYS_FS_MEDIA_HANDLE_INVALID) {
             SYS_CONSOLE_MESSAGE("Registered SST25 flash in file system media manager.\r\n");
             app_flash_data->state = APP_FLASH_STATE_IDLE;
           } else {
             SYS_CONSOLE_MESSAGE("Failure registering SST25 flash in file system media manager!\r\n");
             app_flash_data->state = APP_FLASH_STATE_ERROR;
           }
      }
      break;
    }
    case APP_FLASH_STATE_IDLE:
      // Nothing to do.
      break;
    case APP_FLASH_STATE_ERROR:
      //TODO(sergey): Report error in some way?
      //TODO(sergey): Go back to the idle state so we can recover somehow?
      break;
  }
}
