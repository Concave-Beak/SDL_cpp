# Configuring your game

## Config File
Config files are writen when you click "apply" on the config menu. All of the
config files are located on /home/user/.local/share/game/config.toml check the
index below for every field:

- [Quick considerations](###Quick-Considerations)
- [Example Config](###Example-Config)
- [Graphics](###Graphics)
- [Audio](###Audio) // NOT USED
- [Debug](###Debug)
- [Controls](###Controls) // NOT USED
- [Keybindings](###Keybindings) // NOT USED

### Quick considerations 
Config values are case sensitive. Ex: fullscreen and Fullscreen are not the same.\
If a value is not  writen in the config file, the game uses default values.

### Example Config

```toml
# Example config file

[Graphics]
window_mode = "fullscreen" # Window Mode Options: fullscreen / windowed
fullscreen_mode = "fs-windowed" # Fullscreen Mode: fs-default / fs-desktop / fs-windowed 
resolution = [
    1024, # width
    768,  # heigt
]
graphics_quality = "high" # Graphics Quality Options: low / medium / high / ultra # NOT USED
anti_aliasing = "FXAA" # Anti-aliasing Options: none / FXAA / MSAA / SMAA # NOT USED

[Audio] # NOT USED
output_device = "default" # Sound Output Device: specify device name or default
music_volume = 60         # Music Volume: specify volume level (0-100)
voice_volume = 70         # Voiceover Volume: specify volume level (0-100)

[Debug]
debug_enabled = true              # Needs to be true for the options below, this option itself does nothing, only enables or disables the below
log_file_path = ".logs/debug.log" # Log File Path: specify path for debug log file not relative to the game's path
show_fps = true
show_debug_info = true

[Controls] # NOT USED
mouse_sensitivity = 5.0      # Mouse Sensitivity (0.1 - 10)
invert_x_axis = false        # invert-x-axis 
invert_y_axis = false        # invert-y-axis 
controller_sensitivity = 7.5 # Controller Sensitivity (0.1 - 10)

[Keybindings] # NOT USED
move_up = "W"
move_down = "S"
move_left = "A"
move_right = "D"
```

---
### Graphics

The options here are related to graphical options, note that it's not complete
it needs to handle **graphics_quality** and **anti_aliasing** options. 

```toml
[Graphics]
window_mode = "fullscreen" # Window Mode Options: fullscreen / windowed
fullscreen_mode = "fs-windowed" # Fullscreen Mode: fs-default / fs-desktop / fs-windowed 
resolution = [
    1024, # width
    768,  # heigt
]
graphics_quality = "high" # Graphics Quality Options: low / medium / high / ultra # NOT USED
anti_aliasing = "FXAA" # Anti-aliasing Options: none / FXAA / MSAA / SMAA # NOT USED
```
#### Options

`window_mode` defines the mode that the window will be renderer, either
fullscreen or windowed. String. Opts: "fullscreen", "windowed".

`fullscreen_mode` will only be used if the "fullscreen" mode is enabled in
"window_mode" option. String. Opts: "fs-windowed", "fs-desktop", "fs-windowed".

`resolution` defines a resolution as a array with 2 items, width and height respectivelly. Not used when fullscreen. Int array.

---
### Debug

The optiosn here are related to debuging, it's not complete, **log_file_path** is not used.

```toml
[Debug]
debug_enabled = true              # Needs to be true for the options below, this option itself does nothing, only enables or disables the below
log_file_path = ".logs/debug.log" # Log File Path: specify path for debug log file not relative to the game's path
show_fps = true
show_debug_info = true
```

`debug_enabled` enables the debug options, if set to false non of the other debug options will be available. Boolean

`log_file_path` sets the path for log files. String. NOT USED

`show_fps` enables fps to be displayed

`show_debug_info` enables debug info to be shown
