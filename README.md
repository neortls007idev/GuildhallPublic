*Known Issues*
---

1. Bullets can tunnel through when player shoots between 2 diagonally located stone tiles.
2. the debug timescale options don't work with the sound system.
3. Mud tiles don't slow you down.
4. The Spatial Sound is not updated every frame.
5. The NPC Turret laser goes out alignment when the turret spots the player. Happens only when the player is spotted for a while till the turret moves and reaches the ray casted laser.
6. keyboard controls may seem buggy when rotating the turret as the turret rotation logic for keyboard has not been implemented correctly and the keyboard controls were originally intended as debug tools.

=========================================================================

*Features:*-
---
The Player has 5 Lives at the start of the game and in each life the player was 3 health. Can be changed from Gamecommon.hpp file.

Enemy and Player Bullets can Destroy Each other.

Rumble Sound is played when a bullet hits a boulder.

Spatial/ 3D Sound :-  the balance and the volume on the speakers change when an NPC shoot sound is played relatively to the location of the player. The closer you are to the enemy the more the volume is and depending on the position the speaker balance changes.

=========================================================================

*How to Use/Controls*
---
- ESDF (Xbox Controller left stick) to drive forward and turn the tank toward a direction
- IJKL (Xbox Controller right stick) to turn the turret toward a direction.
- Space key to shoot bullets.
- Enter Key( Xbox Controller Start Button) to start the Game
- P or start button to toggle Pause/Un-pause while Playing.
- ESC exits game.

- F1 (cheat) toggles debug drawing of inner (cyan) and outer (magenta) radii
- F3 (cheat) toggles no-clip mode (disables physics for player tank only)
- F4 (cheat) toggles debug camera (vs. normal game camera), view entire map
- F8 (cheat) deletes and re-creates a new Game object, sets mode to Playing
- T (cheat) hold to sustain slow-motion (0.1x timescale)
- Y (cheat) hold to sustain fast-motion (4.0x timescale)

Keyboard keys ESDF and IJKL for moving tank and turret still exist but you may not get the behavior which is intended.

=========================================================================

Method 1(for new users): -
---
	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Incursion\Run
	3. Double-click Incursion_x64.exe to start the program.

Method 2(for advanced users):-
---
	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Incursion
	3. Open the Incursion.sln using Visual Studio 2019.
	4. Press F7 key to build solution or go to Build --> Build Solution option using the Menu bar.
	5. Press Ctrl + F5 key to start the program without debugging or go to Debug --> Start without Debugging option using the Menu bar.

	*__NOTE:__-* 	
		* Visual Studio 2019 must be installed on the system.
		* In step 5 mentioned above, if you want you can execute the program with the debugger too.
