/*
  opengl.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/Qore.h>

#include "qore-glut.h"

void addGlutConstants()
{
   glut_ns.addConstant("GLUT_API_VERSION",                 new QoreBigIntNode(GLUT_API_VERSION));
   glut_ns.addConstant("GLUT_XLIB_IMPLEMENTATION",         new QoreBigIntNode(GLUT_XLIB_IMPLEMENTATION));
   glut_ns.addConstant("GLUT_MACOSX_IMPLEMENTATION",       new QoreBigIntNode(GLUT_MACOSX_IMPLEMENTATION));
   glut_ns.addConstant("GLUT_RGB",                         new QoreBigIntNode(GLUT_RGB));
   glut_ns.addConstant("GLUT_RGBA",                        new QoreBigIntNode(GLUT_RGBA));
   glut_ns.addConstant("GLUT_INDEX",                       new QoreBigIntNode(GLUT_INDEX));
   glut_ns.addConstant("GLUT_SINGLE",                      new QoreBigIntNode(GLUT_SINGLE));
   glut_ns.addConstant("GLUT_DOUBLE",                      new QoreBigIntNode(GLUT_DOUBLE));
   glut_ns.addConstant("GLUT_ACCUM",                       new QoreBigIntNode(GLUT_ACCUM));
   glut_ns.addConstant("GLUT_ALPHA",                       new QoreBigIntNode(GLUT_ALPHA));
   glut_ns.addConstant("GLUT_DEPTH",                       new QoreBigIntNode(GLUT_DEPTH));
   glut_ns.addConstant("GLUT_STENCIL",                     new QoreBigIntNode(GLUT_STENCIL));
   glut_ns.addConstant("GLUT_MULTISAMPLE",                 new QoreBigIntNode(GLUT_MULTISAMPLE));
   glut_ns.addConstant("GLUT_STEREO",                      new QoreBigIntNode(GLUT_STEREO));
   glut_ns.addConstant("GLUT_LUMINANCE",                   new QoreBigIntNode(GLUT_LUMINANCE));
   glut_ns.addConstant("GLUT_NO_RECOVERY",                 new QoreBigIntNode(GLUT_NO_RECOVERY));
   glut_ns.addConstant("GLUT_LEFT_BUTTON",                 new QoreBigIntNode(GLUT_LEFT_BUTTON));
   glut_ns.addConstant("GLUT_MIDDLE_BUTTON",               new QoreBigIntNode(GLUT_MIDDLE_BUTTON));
   glut_ns.addConstant("GLUT_RIGHT_BUTTON",                new QoreBigIntNode(GLUT_RIGHT_BUTTON));
   glut_ns.addConstant("GLUT_DOWN",                        new QoreBigIntNode(GLUT_DOWN));
   glut_ns.addConstant("GLUT_UP",                          new QoreBigIntNode(GLUT_UP));
   glut_ns.addConstant("GLUT_KEY_F1",                      new QoreBigIntNode(GLUT_KEY_F1));
   glut_ns.addConstant("GLUT_KEY_F2",                      new QoreBigIntNode(GLUT_KEY_F2));
   glut_ns.addConstant("GLUT_KEY_F3",                      new QoreBigIntNode(GLUT_KEY_F3));
   glut_ns.addConstant("GLUT_KEY_F4",                      new QoreBigIntNode(GLUT_KEY_F4));
   glut_ns.addConstant("GLUT_KEY_F5",                      new QoreBigIntNode(GLUT_KEY_F5));
   glut_ns.addConstant("GLUT_KEY_F6",                      new QoreBigIntNode(GLUT_KEY_F6));
   glut_ns.addConstant("GLUT_KEY_F7",                      new QoreBigIntNode(GLUT_KEY_F7));
   glut_ns.addConstant("GLUT_KEY_F8",                      new QoreBigIntNode(GLUT_KEY_F8));
   glut_ns.addConstant("GLUT_KEY_F9",                      new QoreBigIntNode(GLUT_KEY_F9));
   glut_ns.addConstant("GLUT_KEY_F10",                     new QoreBigIntNode(GLUT_KEY_F10));
   glut_ns.addConstant("GLUT_KEY_F11",                     new QoreBigIntNode(GLUT_KEY_F11));
   glut_ns.addConstant("GLUT_KEY_F12",                     new QoreBigIntNode(GLUT_KEY_F12));
   glut_ns.addConstant("GLUT_KEY_LEFT",                    new QoreBigIntNode(GLUT_KEY_LEFT));
   glut_ns.addConstant("GLUT_KEY_UP",                      new QoreBigIntNode(GLUT_KEY_UP));
   glut_ns.addConstant("GLUT_KEY_RIGHT",                   new QoreBigIntNode(GLUT_KEY_RIGHT));
   glut_ns.addConstant("GLUT_KEY_DOWN",                    new QoreBigIntNode(GLUT_KEY_DOWN));
   glut_ns.addConstant("GLUT_KEY_PAGE_UP",                 new QoreBigIntNode(GLUT_KEY_PAGE_UP));
   glut_ns.addConstant("GLUT_KEY_PAGE_DOWN",               new QoreBigIntNode(GLUT_KEY_PAGE_DOWN));
   glut_ns.addConstant("GLUT_KEY_HOME",                    new QoreBigIntNode(GLUT_KEY_HOME));
   glut_ns.addConstant("GLUT_KEY_END",                     new QoreBigIntNode(GLUT_KEY_END));
   glut_ns.addConstant("GLUT_KEY_INSERT",                  new QoreBigIntNode(GLUT_KEY_INSERT));
   glut_ns.addConstant("GLUT_LEFT",                        new QoreBigIntNode(GLUT_LEFT));
   glut_ns.addConstant("GLUT_ENTERED",                     new QoreBigIntNode(GLUT_ENTERED));
   glut_ns.addConstant("GLUT_MENU_NOT_IN_USE",             new QoreBigIntNode(GLUT_MENU_NOT_IN_USE));
   glut_ns.addConstant("GLUT_MENU_IN_USE",                 new QoreBigIntNode(GLUT_MENU_IN_USE));
   glut_ns.addConstant("GLUT_NOT_VISIBLE",                 new QoreBigIntNode(GLUT_NOT_VISIBLE));
   glut_ns.addConstant("GLUT_VISIBLE",                     new QoreBigIntNode(GLUT_VISIBLE));
   glut_ns.addConstant("GLUT_HIDDEN",                      new QoreBigIntNode(GLUT_HIDDEN));
   glut_ns.addConstant("GLUT_FULLY_RETAINED",              new QoreBigIntNode(GLUT_FULLY_RETAINED));
   glut_ns.addConstant("GLUT_PARTIALLY_RETAINED",          new QoreBigIntNode(GLUT_PARTIALLY_RETAINED));
   glut_ns.addConstant("GLUT_FULLY_COVERED",               new QoreBigIntNode(GLUT_FULLY_COVERED));
   glut_ns.addConstant("GLUT_RED",                         new QoreBigIntNode(GLUT_RED));
   glut_ns.addConstant("GLUT_GREEN",                       new QoreBigIntNode(GLUT_GREEN));
   glut_ns.addConstant("GLUT_BLUE",                        new QoreBigIntNode(GLUT_BLUE));
   glut_ns.addConstant("GLUT_NORMAL",                      new QoreBigIntNode(GLUT_NORMAL));
   glut_ns.addConstant("GLUT_OVERLAY",                     new QoreBigIntNode(GLUT_OVERLAY));
   glut_ns.addConstant("GLUT_WINDOW_X",                    new QoreBigIntNode(GLUT_WINDOW_X));
   glut_ns.addConstant("GLUT_WINDOW_Y",                    new QoreBigIntNode(GLUT_WINDOW_Y));
   glut_ns.addConstant("GLUT_WINDOW_WIDTH",                new QoreBigIntNode(GLUT_WINDOW_WIDTH));
   glut_ns.addConstant("GLUT_WINDOW_HEIGHT",               new QoreBigIntNode(GLUT_WINDOW_HEIGHT));
   glut_ns.addConstant("GLUT_WINDOW_BUFFER_SIZE",          new QoreBigIntNode(GLUT_WINDOW_BUFFER_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_STENCIL_SIZE",         new QoreBigIntNode(GLUT_WINDOW_STENCIL_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_DEPTH_SIZE",           new QoreBigIntNode(GLUT_WINDOW_DEPTH_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_RED_SIZE",             new QoreBigIntNode(GLUT_WINDOW_RED_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_GREEN_SIZE",           new QoreBigIntNode(GLUT_WINDOW_GREEN_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_BLUE_SIZE",            new QoreBigIntNode(GLUT_WINDOW_BLUE_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_ALPHA_SIZE",           new QoreBigIntNode(GLUT_WINDOW_ALPHA_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_ACCUM_RED_SIZE",       new QoreBigIntNode(GLUT_WINDOW_ACCUM_RED_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_ACCUM_GREEN_SIZE",     new QoreBigIntNode(GLUT_WINDOW_ACCUM_GREEN_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_ACCUM_BLUE_SIZE",      new QoreBigIntNode(GLUT_WINDOW_ACCUM_BLUE_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_ACCUM_ALPHA_SIZE",     new QoreBigIntNode(GLUT_WINDOW_ACCUM_ALPHA_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_DOUBLEBUFFER",         new QoreBigIntNode(GLUT_WINDOW_DOUBLEBUFFER));
   glut_ns.addConstant("GLUT_WINDOW_RGBA",                 new QoreBigIntNode(GLUT_WINDOW_RGBA));
   glut_ns.addConstant("GLUT_WINDOW_PARENT",               new QoreBigIntNode(GLUT_WINDOW_PARENT));
   glut_ns.addConstant("GLUT_WINDOW_NUM_CHILDREN",         new QoreBigIntNode(GLUT_WINDOW_NUM_CHILDREN));
   glut_ns.addConstant("GLUT_WINDOW_COLORMAP_SIZE",        new QoreBigIntNode(GLUT_WINDOW_COLORMAP_SIZE));
   glut_ns.addConstant("GLUT_WINDOW_NUM_SAMPLES",          new QoreBigIntNode(GLUT_WINDOW_NUM_SAMPLES));
   glut_ns.addConstant("GLUT_WINDOW_STEREO",               new QoreBigIntNode(GLUT_WINDOW_STEREO));
   glut_ns.addConstant("GLUT_WINDOW_CURSOR",               new QoreBigIntNode(GLUT_WINDOW_CURSOR));
   glut_ns.addConstant("GLUT_SCREEN_WIDTH",                new QoreBigIntNode(GLUT_SCREEN_WIDTH));
   glut_ns.addConstant("GLUT_SCREEN_HEIGHT",               new QoreBigIntNode(GLUT_SCREEN_HEIGHT));
   glut_ns.addConstant("GLUT_SCREEN_WIDTH_MM",             new QoreBigIntNode(GLUT_SCREEN_WIDTH_MM));
   glut_ns.addConstant("GLUT_SCREEN_HEIGHT_MM",            new QoreBigIntNode(GLUT_SCREEN_HEIGHT_MM));
   glut_ns.addConstant("GLUT_MENU_NUM_ITEMS",              new QoreBigIntNode(GLUT_MENU_NUM_ITEMS));
   glut_ns.addConstant("GLUT_DISPLAY_MODE_POSSIBLE",       new QoreBigIntNode(GLUT_DISPLAY_MODE_POSSIBLE));
   glut_ns.addConstant("GLUT_INIT_WINDOW_X",               new QoreBigIntNode(GLUT_INIT_WINDOW_X));
   glut_ns.addConstant("GLUT_INIT_WINDOW_Y",               new QoreBigIntNode(GLUT_INIT_WINDOW_Y));
   glut_ns.addConstant("GLUT_INIT_WINDOW_WIDTH",           new QoreBigIntNode(GLUT_INIT_WINDOW_WIDTH));
   glut_ns.addConstant("GLUT_INIT_WINDOW_HEIGHT",          new QoreBigIntNode(GLUT_INIT_WINDOW_HEIGHT));
   glut_ns.addConstant("GLUT_INIT_DISPLAY_MODE",           new QoreBigIntNode(GLUT_INIT_DISPLAY_MODE));
   glut_ns.addConstant("GLUT_ELAPSED_TIME",                new QoreBigIntNode(GLUT_ELAPSED_TIME));
   glut_ns.addConstant("GLUT_WINDOW_FORMAT_ID",            new QoreBigIntNode(GLUT_WINDOW_FORMAT_ID));
   glut_ns.addConstant("GLUT_HAS_KEYBOARD",                new QoreBigIntNode(GLUT_HAS_KEYBOARD));
   glut_ns.addConstant("GLUT_HAS_MOUSE",                   new QoreBigIntNode(GLUT_HAS_MOUSE));
   glut_ns.addConstant("GLUT_HAS_SPACEBALL",               new QoreBigIntNode(GLUT_HAS_SPACEBALL));
   glut_ns.addConstant("GLUT_HAS_DIAL_AND_BUTTON_BOX",     new QoreBigIntNode(GLUT_HAS_DIAL_AND_BUTTON_BOX));
   glut_ns.addConstant("GLUT_HAS_TABLET",                  new QoreBigIntNode(GLUT_HAS_TABLET));
   glut_ns.addConstant("GLUT_NUM_MOUSE_BUTTONS",           new QoreBigIntNode(GLUT_NUM_MOUSE_BUTTONS));
   glut_ns.addConstant("GLUT_NUM_SPACEBALL_BUTTONS",       new QoreBigIntNode(GLUT_NUM_SPACEBALL_BUTTONS));
   glut_ns.addConstant("GLUT_NUM_BUTTON_BOX_BUTTONS",      new QoreBigIntNode(GLUT_NUM_BUTTON_BOX_BUTTONS));
   glut_ns.addConstant("GLUT_NUM_DIALS",                   new QoreBigIntNode(GLUT_NUM_DIALS));
   glut_ns.addConstant("GLUT_NUM_TABLET_BUTTONS",          new QoreBigIntNode(GLUT_NUM_TABLET_BUTTONS));
   glut_ns.addConstant("GLUT_DEVICE_IGNORE_KEY_REPEAT",    new QoreBigIntNode(GLUT_DEVICE_IGNORE_KEY_REPEAT));
   glut_ns.addConstant("GLUT_DEVICE_KEY_REPEAT",           new QoreBigIntNode(GLUT_DEVICE_KEY_REPEAT));
   glut_ns.addConstant("GLUT_HAS_JOYSTICK",                new QoreBigIntNode(GLUT_HAS_JOYSTICK));
   glut_ns.addConstant("GLUT_OWNS_JOYSTICK",               new QoreBigIntNode(GLUT_OWNS_JOYSTICK));
   glut_ns.addConstant("GLUT_JOYSTICK_BUTTONS",            new QoreBigIntNode(GLUT_JOYSTICK_BUTTONS));
   glut_ns.addConstant("GLUT_JOYSTICK_AXES",               new QoreBigIntNode(GLUT_JOYSTICK_AXES));
   glut_ns.addConstant("GLUT_JOYSTICK_POLL_RATE",          new QoreBigIntNode(GLUT_JOYSTICK_POLL_RATE));
   glut_ns.addConstant("GLUT_OVERLAY_POSSIBLE",            new QoreBigIntNode(GLUT_OVERLAY_POSSIBLE));
   glut_ns.addConstant("GLUT_LAYER_IN_USE",                new QoreBigIntNode(GLUT_LAYER_IN_USE));
   glut_ns.addConstant("GLUT_HAS_OVERLAY",                 new QoreBigIntNode(GLUT_HAS_OVERLAY));
   glut_ns.addConstant("GLUT_TRANSPARENT_INDEX",           new QoreBigIntNode(GLUT_TRANSPARENT_INDEX));
   glut_ns.addConstant("GLUT_NORMAL_DAMAGED",              new QoreBigIntNode(GLUT_NORMAL_DAMAGED));
   glut_ns.addConstant("GLUT_OVERLAY_DAMAGED",             new QoreBigIntNode(GLUT_OVERLAY_DAMAGED));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_POSSIBLE",       new QoreBigIntNode(GLUT_VIDEO_RESIZE_POSSIBLE));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_IN_USE",         new QoreBigIntNode(GLUT_VIDEO_RESIZE_IN_USE));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_X_DELTA",        new QoreBigIntNode(GLUT_VIDEO_RESIZE_X_DELTA));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_Y_DELTA",        new QoreBigIntNode(GLUT_VIDEO_RESIZE_Y_DELTA));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_WIDTH_DELTA",    new QoreBigIntNode(GLUT_VIDEO_RESIZE_WIDTH_DELTA));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_HEIGHT_DELTA",   new QoreBigIntNode(GLUT_VIDEO_RESIZE_HEIGHT_DELTA));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_X",              new QoreBigIntNode(GLUT_VIDEO_RESIZE_X));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_Y",              new QoreBigIntNode(GLUT_VIDEO_RESIZE_Y));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_WIDTH",          new QoreBigIntNode(GLUT_VIDEO_RESIZE_WIDTH));
   glut_ns.addConstant("GLUT_VIDEO_RESIZE_HEIGHT",         new QoreBigIntNode(GLUT_VIDEO_RESIZE_HEIGHT));
   glut_ns.addConstant("GLUT_ACTIVE_SHIFT",                new QoreBigIntNode(GLUT_ACTIVE_SHIFT));
   glut_ns.addConstant("GLUT_ACTIVE_CTRL",                 new QoreBigIntNode(GLUT_ACTIVE_CTRL));
   glut_ns.addConstant("GLUT_ACTIVE_ALT",                  new QoreBigIntNode(GLUT_ACTIVE_ALT));
   glut_ns.addConstant("GLUT_CURSOR_RIGHT_ARROW",          new QoreBigIntNode(GLUT_CURSOR_RIGHT_ARROW));
   glut_ns.addConstant("GLUT_CURSOR_LEFT_ARROW",           new QoreBigIntNode(GLUT_CURSOR_LEFT_ARROW));
   glut_ns.addConstant("GLUT_CURSOR_INFO",                 new QoreBigIntNode(GLUT_CURSOR_INFO));
   glut_ns.addConstant("GLUT_CURSOR_DESTROY",              new QoreBigIntNode(GLUT_CURSOR_DESTROY));
   glut_ns.addConstant("GLUT_CURSOR_HELP",                 new QoreBigIntNode(GLUT_CURSOR_HELP));
   glut_ns.addConstant("GLUT_CURSOR_CYCLE",                new QoreBigIntNode(GLUT_CURSOR_CYCLE));
   glut_ns.addConstant("GLUT_CURSOR_SPRAY",                new QoreBigIntNode(GLUT_CURSOR_SPRAY));
   glut_ns.addConstant("GLUT_CURSOR_WAIT",                 new QoreBigIntNode(GLUT_CURSOR_WAIT));
   glut_ns.addConstant("GLUT_CURSOR_TEXT",                 new QoreBigIntNode(GLUT_CURSOR_TEXT));
   glut_ns.addConstant("GLUT_CURSOR_CROSSHAIR",            new QoreBigIntNode(GLUT_CURSOR_CROSSHAIR));
   glut_ns.addConstant("GLUT_CURSOR_UP_DOWN",              new QoreBigIntNode(GLUT_CURSOR_UP_DOWN));
   glut_ns.addConstant("GLUT_CURSOR_LEFT_RIGHT",           new QoreBigIntNode(GLUT_CURSOR_LEFT_RIGHT));
   glut_ns.addConstant("GLUT_CURSOR_TOP_SIDE",             new QoreBigIntNode(GLUT_CURSOR_TOP_SIDE));
   glut_ns.addConstant("GLUT_CURSOR_BOTTOM_SIDE",          new QoreBigIntNode(GLUT_CURSOR_BOTTOM_SIDE));
   glut_ns.addConstant("GLUT_CURSOR_LEFT_SIDE",            new QoreBigIntNode(GLUT_CURSOR_LEFT_SIDE));
   glut_ns.addConstant("GLUT_CURSOR_RIGHT_SIDE",           new QoreBigIntNode(GLUT_CURSOR_RIGHT_SIDE));
   glut_ns.addConstant("GLUT_CURSOR_TOP_LEFT_CORNER",      new QoreBigIntNode(GLUT_CURSOR_TOP_LEFT_CORNER));
   glut_ns.addConstant("GLUT_CURSOR_TOP_RIGHT_CORNER",     new QoreBigIntNode(GLUT_CURSOR_TOP_RIGHT_CORNER));
   glut_ns.addConstant("GLUT_CURSOR_BOTTOM_RIGHT_CORNER",  new QoreBigIntNode(GLUT_CURSOR_BOTTOM_RIGHT_CORNER));
   glut_ns.addConstant("GLUT_CURSOR_BOTTOM_LEFT_CORNER",   new QoreBigIntNode(GLUT_CURSOR_BOTTOM_LEFT_CORNER));
   glut_ns.addConstant("GLUT_CURSOR_INHERIT",              new QoreBigIntNode(GLUT_CURSOR_INHERIT));
   glut_ns.addConstant("GLUT_CURSOR_NONE",                 new QoreBigIntNode(GLUT_CURSOR_NONE));
   glut_ns.addConstant("GLUT_CURSOR_FULL_CROSSHAIR",       new QoreBigIntNode(GLUT_CURSOR_FULL_CROSSHAIR));
   glut_ns.addConstant("GLUT_KEY_REPEAT_OFF",              new QoreBigIntNode(GLUT_KEY_REPEAT_OFF));
   glut_ns.addConstant("GLUT_KEY_REPEAT_ON",               new QoreBigIntNode(GLUT_KEY_REPEAT_ON));
   glut_ns.addConstant("GLUT_KEY_REPEAT_DEFAULT",          new QoreBigIntNode(GLUT_KEY_REPEAT_DEFAULT));
   glut_ns.addConstant("GLUT_JOYSTICK_BUTTON_A",           new QoreBigIntNode(GLUT_JOYSTICK_BUTTON_A));
   glut_ns.addConstant("GLUT_JOYSTICK_BUTTON_B",           new QoreBigIntNode(GLUT_JOYSTICK_BUTTON_B));
   glut_ns.addConstant("GLUT_JOYSTICK_BUTTON_C",           new QoreBigIntNode(GLUT_JOYSTICK_BUTTON_C));
   glut_ns.addConstant("GLUT_JOYSTICK_BUTTON_D",           new QoreBigIntNode(GLUT_JOYSTICK_BUTTON_D));
   glut_ns.addConstant("GLUT_GAME_MODE_ACTIVE",            new QoreBigIntNode(GLUT_GAME_MODE_ACTIVE));
   glut_ns.addConstant("GLUT_GAME_MODE_POSSIBLE",          new QoreBigIntNode(GLUT_GAME_MODE_POSSIBLE));
   glut_ns.addConstant("GLUT_GAME_MODE_WIDTH",             new QoreBigIntNode(GLUT_GAME_MODE_WIDTH));
   glut_ns.addConstant("GLUT_GAME_MODE_HEIGHT",            new QoreBigIntNode(GLUT_GAME_MODE_HEIGHT));
   glut_ns.addConstant("GLUT_GAME_MODE_PIXEL_DEPTH",       new QoreBigIntNode(GLUT_GAME_MODE_PIXEL_DEPTH));
   glut_ns.addConstant("GLUT_GAME_MODE_REFRESH_RATE",      new QoreBigIntNode(GLUT_GAME_MODE_REFRESH_RATE));
   glut_ns.addConstant("GLUT_GAME_MODE_DISPLAY_CHANGED",   new QoreBigIntNode(GLUT_GAME_MODE_DISPLAY_CHANGED));
}
