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

#include "utildefines.h"

#define LOG_PREFIX "APP FLASH: "

// Regular print / message.
#define FLASH_PRINT(format, ...) APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define FLASH_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Error print / message.
#define FLASH_ERROR_PRINT(format, ...) \
  APP_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define FLASH_ERROR_MESSAGE(message) APP_MESSAGE(LOG_PREFIX, message)
// Debug print / message.
#define FLASH_DEBUG_PRINT(format, ...) \
  APP_DEBUG_PRINT(LOG_PREFIX, format, ##__VA_ARGS__)
#define FLASH_DEBUG_MESSAGE(message) APP_DEBUG_MESSAGE(LOG_PREFIX, message)

#define FLASH_DEVICE_NAME  "/dev/mtda1"
#define FLASH_MOUNT_POINT  "/mnt/sst25_drive"

// Get driver handle from the application data.
//
// TODO(sergey): This of the future, how to nicely support multiple external
// flash drivers?
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
  app_flash_data->format_attempted = false;
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
          FLASH_MESSAGE("Registered SST25 flash in file system "
                        "media manager.\r\n");
          app_flash_data->state = APP_FLASH_STATE_MOUNT_DISK;
        } else {
          FLASH_MESSAGE("Failure registering SST25 flash in "
                        "file system media manager!\r\n");
          app_flash_data->state = APP_FLASH_STATE_ERROR;
        }
      }
      break;
    }

    case APP_FLASH_STATE_MOUNT_DISK: {
      if (SYS_FS_Mount(FLASH_DEVICE_NAME, FLASH_MOUNT_POINT, 
                       FAT, 0, NULL) != SYS_FS_RES_SUCCESS) {
        FLASH_DEBUG_MESSAGE("Mount failed, will try again later.\r\n");
        // The disk could not be mounted. Try mounting again until
        // the operation succeeds.
        app_flash_data->state = APP_FLASH_STATE_MOUNT_DISK;
      } else {
        // Mount was successful. Format the disk. */
        FLASH_PRINT("Disk %s mounted on %s.\r\n",
                    FLASH_DEVICE_NAME, FLASH_MOUNT_POINT);
        app_flash_data->state = APP_FLASH_STATE_ENSURE_FORMATTED;
      }
      break;
    }

    case APP_FLASH_STATE_ENSURE_FORMATTED: {
      // SYS_MODULE_OBJ flash = flash_driver_get(app_flash_data);
      uint32_t total_sectors, free_sectors;
      if (SYS_FS_DriveSectorGet(FLASH_MOUNT_POINT,
                                &total_sectors,
                                &free_sectors) != SYS_FS_RES_SUCCESS) {
        FLASH_ERROR_MESSAGE("Failed to query drive sector information.\r\n");
        app_flash_data->state = APP_FLASH_STATE_ERROR;
      } else {
        FLASH_PRINT("Drive has %d total sectors, %d free sectors.\r\n",
                    total_sectors, free_sectors);
        if (total_sectors == 0) {
          FLASH_MESSAGE("Drive does not seems to be formatted.\r\n");
          if (app_flash_data->format_attempted) {
            FLASH_ERROR_MESSAGE("Drive formate was already attempted but "
                                "failed, will not try again.\r\n");
            app_flash_data->state = APP_FLASH_STATE_ERROR;
          } else {
            FLASH_MESSAGE("Will perform drive format.\r\n");
            app_flash_data->state = APP_FLASH_STATE_FORMAT;
          }
        } else {
          FLASH_MESSAGE("Drive appears to be properly formatted.\r\n");
          app_flash_data->state = APP_FLASH_STATE_IDLE;
        }
      }
      break;
    }

    case APP_FLASH_STATE_FORMAT: {
      FLASH_MESSAGE("Formatting the drive.\r\n");
      app_flash_data->format_attempted = 1;
      if (SYS_FS_DriveFormat(FLASH_MOUNT_POINT,
                             SYS_FS_FORMAT_SFD,
                             0) != SYS_FS_RES_SUCCESS) {
        /* Format of the disk failed. */
        FLASH_ERROR_MESSAGE("Error formatting the drive.\r\n");
        app_flash_data->state = APP_FLASH_STATE_ERROR;
      } else {
        app_flash_data->state = APP_FLASH_STATE_ENSURE_FORMATTED;
        FLASH_MESSAGE("Drive is formatted, verifying...\r\n");
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

bool APP_Flash_IsBusy(AppFlashData* app_flash_data) {
  return (app_flash_data->state != APP_FLASH_STATE_IDLE);
}

bool APP_Flash_IsError(AppFlashData* app_flash_data) {
  return (app_flash_data->state == APP_FLASH_STATE_ERROR);
}

bool APP_Flash_DriveSectorGet(uint32_t* total_sectors, uint32_t* free_sectors) {
  return (SYS_FS_DriveSectorGet(FLASH_MOUNT_POINT,
                                total_sectors,
                                free_sectors) == SYS_FS_RES_SUCCESS);
}

bool APP_Flash_DriveFormat(void) {
  return (SYS_FS_DriveFormat(FLASH_MOUNT_POINT,
                             SYS_FS_FORMAT_SFD,
                             0) == SYS_FS_RES_SUCCESS);
}
