
## NOTICE: UNDER CONSTRUCTION
This mod may not currently compile. And even if it does, it shouldn't do anything different from Thinker 5.5.

SMACX Builder Mod
=================

Builder aims to make it easier for people to create, distribute and play their own mods, through accessible text-file
based changes, like the vanilla game allows to a degree with `alphax.txt` and similar files. 

* Modifies the game's file reader to allow rerouting to files shadowing base game files in a mod directory. (Not started yet)
* Allows the editing of Facilities and Secret Projects (In progress)

This mod is based on [Thinker](https://github.com/induktio/thinker), for all the various benefits it offers. 

This mod is tested to work with the [GOG version](https://www.gog.com/game/sid_meiers_alpha_centauri) of Alpha Centauri.
Note that official Alien Crossfire patch version 2.0 must be installed for the launcher to work with terranx.exe.
Older game version 1.0 is NOT supported by Thinker. Installing Scient's patch v2 also works since this terranx.exe can be used by the launcher.
Thinker also includes changes to remove issues on Windows 11 that prevent playing the game expansion due to crashes after ending the turn.

[See more information](Details.md) about the features and recommended settings.
It's strongly recommended to read Details.md since many features are added not provided by the original game.

Download
--------
Builder isn't playable yet. 

Installation
------------
1. Install Thinker as explained [there](https://github.com/induktio/thinker#Installation).
2. Unzip the downloaded archive inside your game folder, overwriting `thinker.dll` when prompted.


Troubleshooting
---------------
The launcher requires original Alien Crossfire v2.0 terranx.exe in the same folder but this file is not modified on disk.
In case of startup problems, make sure the official v2.0 patch is applied on the game, and the mod is actually started
from same folder with the game. Sometimes startup issues can be fixed by starting the launcher with administrator privileges.

When starting the game with screen scaling set to something else than 100% some portion of the window may not be visible due to being
clipped out of the screen. The game does not have proper support for screen scaling so as a workaround the scaling should be set to 100%.
Sometimes opening/secret project video playback errors might be fixed by installing [DDrawCompat](https://github.com/narzoul/DDrawCompat)
but otherwise it is not necessary.

The current GOG Alpha Centauri Planetary Pack (13 November 2024) may not work on Windows XP by default. When starting the game
there can be an error message "The application failed to start because dwmapi.dll was not found." To fix this simply remove ddraw.dll
from the game folder. This file was added on the latest GOG version and it is not necessary to run the game on Windows XP.

Alt-tabbing may sometimes not work by default in Alpha Centauri's GOG version. To re-enable alt-tab feature, follow these steps.
First open a command prompt with administrator privileges in Alpha Centauri's installation folder. After entering the commands
below alt-tabbing should now work when the game is restarted.

    sdbinst -u game.sdb
    sdbinst -u game_add.sdb

After installing the game on Windows there might be a notification that Windows Features can't complete the requested changes.
This might be caused by a failure to install DirectPlay. First open a command prompt with administrator privileges.
Then after entering the command below DirectPlay should be automatically installed.

    dism /online /Enable-Feature /FeatureName:DirectPlay /All


Other mods
----------
As a rule, if it works with Thinker, it works with Builder. If it doesn't work with Thinker, it doesn't work
with Builder. 

Builder-based mods can be installed in the `mods/` subfolder of your game's directory. Edit `mods/active.txt` to contain 
the directory of the downloaded mod, and it will be active.


License
-------
This software is licensed under the MIT License. Check [License.md](License.md) for detailed conditions.

The original game assets are not covered by this license and remain property of Firaxis Games Inc and Electronic Arts Inc.

Sid Meier's Alpha Centauri and Sid Meier's Alien Crossfire is Copyright © 1997, 1998 by Firaxis Games Inc and Electronic Arts Inc.
