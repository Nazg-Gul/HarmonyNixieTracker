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

#ifndef _APP_NIXIE_H
#define _APP_NIXIE_H

#include <stdbool.h>

typedef enum {
  // None of nixie type related tasks is to be performed.
  APP_NIXIE_STATE_IDLE,
} AppNixieState;

typedef struct AppNixieData {
  AppNixieState state;
} AppNixieData;

// Initialize nixie types and state machine.
void APP_Nixie_Initialize(AppNixieData* app_nixie_data);

// Perform periodic tasks related on nixie types.
void APP_Nixie_Tasks(AppNixieData* app_nixie_data);

// Ceck whether nixie module is busy with any tasks.
bool APP_Nixie_IsBusy(AppNixieData* app_nixie_data);

#endif  // _APP_NIXIE_H
