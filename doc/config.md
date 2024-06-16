# Configuring your game

## Config File
The config file is located on /home/user/.config/SoulBound/config.toml check the
the index below for every configurable field:

- [Quick considerations](#Quick-Considerations)
- [Example Config](#Example-Config)
- [Graphics](#Graphics)
- [Audio](#Audio) **# NOT USED**
- [Debug](#Debug)
- [Controls](#Controls) **# NOT USED**
- [Keybindings](#Keybindings) 

### Quick considerations 
Config values are case-sensitive. Ex: fullscreen and Fullscreen are not the same.\
If a value is not written in the config file, the game uses default values.

Also, as this is a work in progress, not all configurable items are complete.

### Example Config

```toml
# Example config file

[Graphics]
window_mode = "fullscreen" # Window Mode Options: fullscreen / windowed
fullscreen_mode = "fs-windowed" # Fullscreen Mode: fs-default / fs-desktop / fs-windowed 
resolution = [
    1920, # width
    1080,  # heigt
]
graphics_quality = "high" # Graphics Quality Options: low / medium / high / ultra # NOT USED
anti_aliasing = "FXAA" # Anti-aliasing Options: none / FXAA / MSAA / SMAA # NOT USED

[Audio] # NOT USED
output_device = "default" # Sound Output Device: specify device name or default
music_volume = 60         # Music Volume: specify volume level (0-100)
voice_volume = 70         # Voiceover Volume: specify volume level (0-100)

[Debug]
debug_enabled = true              # Needs to be true for the options below, this option itself does nothing, only enables or disables the below
log_file_path = ".logs/debug.log" # Log File Path: specify the path for the debug log file not relative to the game's path
show_fps = true
show_debug_info = true
no_video = true # Only for backend debugging purposes
draw_vision_cones = false # NOT USED

[Controls] # NOT USED
mouse_sensitivity = 5.0      # Mouse Sensitivity (0.1 - 10)
invert_x_axis = false        # invert x-axis 
invert_y_axis = false        # invert y-axis 
controller_sensitivity = 7.5 # Controller Sensitivity (0.1 - 10)
dodge_with_mouse = false

[Keybindings] 
move_up = "W"
move_down = "S"
move_left = "A"
move_right = "D"

attack1 = "MOUSE1"
attack2 = "MOUSE2"

switch_weapons = "Q"
open_inventory = "E" # NOT USED

quit_game = "ESC"
```

---
### Graphics

The options here are related to graphical options, note that it's not complete
it needs to handle **graphics_quality** and **anti_aliasing** options. 

```toml
[Graphics]
window_mode = "fullscreen" # Window Mode Options: fullscreen/windowed
fullscreen_mode = "fs-windowed" # Fullscreen Mode: fs-default/ fs-desktop/fs-windowed 
resolution = [
    1024, # width
    768,  # heigt
]
graphics_quality = "high" # Graphics Quality Options: low/medium/high/ultra # NOT USED
anti_aliasing = "FXAA" # Anti-aliasing Options: none / FXAA / MSAA / SMAA # NOT USED
```
#### Options

`window_mode` defines the mode that the window will be renderer, either
fullscreen or windowed. String. Opts: "fullscreen", "windowed".

`fullscreen_mode` will only be used if the "fullscreen" mode is enabled in
the "window_mode" option. String. Opts: "fs-windowed", "fs-desktop", "fs-windowed".

`resolution` defines a resolution as an array with 2 items, width and height respectively. Not used when fullscreen. Int array.

---
### Debug

The options here are related to debugging, it's not complete, and **log_file_path** is not used.

```toml
[Debug]
debug_enabled = true              # Needs to be true for the options below, this option itself does nothing, only enables or disables the below
log_file_path = ".logs/debug.log" # Log File Path: specify the path for the debug log file not relative to the game's path
show_fps = true
show_debug_info = true
```

`debug_enabled` enables the debug options, if set to false none of the other debug options will be available. Boolean

`log_file_path` sets the path for log files. String. **NOT USED**

`show_fps` enable fps to be displayed

`show_debug_info` enables debug info to be shown

### Keybindings

#### Actions
`move_up` - Move up\
`move_down` - Move down\
`move_left` - Move left\
`move_right` - move right

`attack1` - Primary attack\
`attack2` - Secondary attack **# NOT USED**

`switch_weapons` - Switch weapons in weapon slot **# NOT USED**\
`open_inventory` - Open's inventory **# NOT USED**

`quit_game` - Quits the game

### Keys

Keys are also case-sensitive and written in all caps.

Valid Keys:
- `A-Z`
- `ESC` - Escape key
- `MOUSE1` - Left mouse button
- `MOUSE2` - Right mouse button
