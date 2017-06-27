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

#ifndef _APP_SHIFT_REGISTER_H
#define _APP_SHIFT_REGISTER_H

#include <stdbool.h>

typedef enum {
  // No tasks to be performed.
  APP_SHIFT_REGISTER_STATE_IDLE,
} AppShiftRegisterState;

typedef struct AppShiftRegisterData {
  AppShiftRegisterState state;
} AppShiftRegisterData;

// Initialize shift register related application routines.
void APP_ShiftRegister_Initialize(
    AppShiftRegisterData* app_shift_register_data);

// Perform all shift register related tasks.
void APP_ShiftRegister_Tasks(AppShiftRegisterData* app_shift_register_data);

// Check whether shift register module is busy with any tasks.
bool APP_ShiftRegister_IsBusy(AppShiftRegisterData* app_shift_register_data);

void APP_ShiftRegister_SetEnabled(
    AppShiftRegisterData* app_shift_register_data,
    bool is_enabled);

#endif  // _APP_SHIFT_REGISTER_H
