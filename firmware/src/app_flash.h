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

#ifndef _APP_FLASH_H
#define _APP_FLASH_H

#include "system_definitions.h"

#include <stdbool.h>

typedef enum {
  // Initial initialization: register file system media.
  APP_FLASH_STATE_REGISTER_MEDIA,
  // Mount flash fist to the filesystem mount point.
  APP_FLASH_STATE_MOUNT_DISK,
  // Ensure flash drive is properly formatted and ready for use.
  APP_FLASH_STATE_ENSURE_FORMATTED,
  // Perform drive format.
  APP_FLASH_STATE_FORMAT,
  // No tasks to be performed.
  APP_FLASH_STATE_IDLE,
  // Error occurred in flash module.
  APP_FLASH_STATE_ERROR,
} AppFlashState;

typedef struct AppFlashData {
  SYSTEM_OBJECTS* system_objects;
  AppFlashState state;
  // Initial format attempted, do not try it again if format fails.
  bool format_attempted;
} AppFlashData;

// Initialize flash related application routines.
void APP_Flash_Initialize(AppFlashData* app_flash_data,
                          SYSTEM_OBJECTS* system_objects);

// Perform all flash related tasks.
void APP_Flash_Tasks(AppFlashData* app_flash_data);

// Check whether flash module is busy with any tasks.
bool APP_Flash_IsBusy(AppFlashData* app_flash_data);

// Check whether flash module is in error state.
bool APP_Flash_IsError(AppFlashData* app_flash_data);

// Query number of total and free sectors.
//
// Return true on success.
bool APP_Flash_DriveSectorGet(uint32_t* total_sectors, uint32_t* free_sectors);

// Format the flash drive.
//
// Return true on success.
bool APP_Flash_DriveFormat(void);

#endif  // _APP_FLASH_H
