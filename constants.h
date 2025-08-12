#pragma once

// settings
extern const unsigned int SCR_WIDTH = 800;
extern const unsigned int SCR_HEIGHT = 600;
//Gizmo IDs
extern const int GIZMO_RED_ID = (255 << 16) | (0 << 8) | 0;   // 0xFF0000 = 16711680
extern const int GIZMO_GREEN_ID = (0 << 16) | (255 << 8) | 0;   // 0x00FF00 = 65280
extern const int GIZMO_BLUE_ID = (0 << 16) | (0 << 8) | 255;   // 0x0000FF = 255