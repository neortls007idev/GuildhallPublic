

Known Issues

1. UI is underneath the Game so Game objects are able to overlay the RenderUI( Not sure why this is    happening cause I am still Rendering the UI at last and thus drawing it on top of everything else.

2. If you use Space key to enter the game state form the start state the game starts with the ship instantly shooting a bullet.

3. Nothing renders for 3 seconds once you complete all 5 waves and return to the attract screen.( the controller vibraiton there is on purpose as feedback of victory).

4. The screenshake might need to be toned down but yeah its on purpose right now and yes all the minimal screen shake happening with bullet spawn is intentional.

5. Controller vibration has not been managed well and player may need to manually reset the joystick to turn it off.

How to Use

For controls use left analog stck on the xbox controller for movement and the 'A' button on the controller for shooting.

THE KEYBOARD CONTROLS:-

UP arrow to thrust.
LEFT and RIGHT arrow key for rotating left or right.
SPACE key to shoot bullet.( toggle key)
P for Pause.( toggle key)
F8 for new game.(only works in game state and game restarts in game state).

Debugger Options
F1( toggle key)
T for slow-Mo.( needs to be pressed down)
N for respawn when you loose all lives you have only 3secs to do that or the game goes back to attract screen.

Method 1(for new users): -
	
	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Starship\Run 
	3. Double-click Starship_x64.exe to start the program.

Method 2(for advanced users):-

	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SD\Starship
	3. Open the Starship.sln using Visual Studio 2019.
	4. Press F7 key to build solution or go to Build --> Build Solution option using the Menu bar.
	5. Press Ctrl + F5 key to start the program without debugging or go to Debug --> Start without Debugging option using the Menu bar.

	NOTE:- 	
		* Visual Studio 2019 must be installed on the system.
		* In step 5 mentioned above, if you want you can execute the program with the debugger too.

Deep Learning
	
The typing style is really important and enhances the readability of the code.So sticking with a typing style is really important as inconsistent styles hinder with the readability of the code, although I am not sure how much I consistency I was able to maintain. Although I did things in a really backward C sort of Bottom-Up approach It is really important to understand the OOPS concepts and how they integrate with the Top-Down coding approach and enhance code reusablity rather than writing pieces of redundant code which caused me a lot of grief during this project. Due to using the Bottom-Up coding approach a lot of naked variables were used which at the moment seemed like a good idea to do all in attempt to make things work in retrospect now seems down right stupid and the wrong way of approaching the problem. I have come to this realization that I need to change my coding practices really quickly and avoid the Bottom-Up approach unless absolutely necessary.




