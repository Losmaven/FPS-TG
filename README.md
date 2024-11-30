Once your FPS game is compiled and running in the terminal, you can use keyboard inputs to interact with the game. Here are the commands to play the game:
Movement Commands

    W: Move forward.
    S: Move backward.
    A: Rotate left (counter-clockwise).
    D: Rotate right (clockwise).

**Gameplay Details**

   ** World Map**:
        The world is a 2D grid represented in the terminal.
        # indicates walls.
        . indicates open spaces.
        P is your player's position.

   ** Field of View** (FOV):
        As you move or rotate, the rendered view in the terminal adjusts based on the player's position and direction.

    **Collision**:
        You cannot move through walls (#). If you attempt to do so, your player will stop.

**How to Exit**

    To stop the game, press Ctrl + C in the terminal. This will terminate the program.

**Steps to Run and Play**

    Open Terminal in VS Code:
        Use the shortcut Ctrl + `` (backtick) to open the terminal.

    Navigate to the Game Directory:
    cd://.......
       

**Compile the Game:**

g++ game.cpp -o game

**Run the Game:**

./game

**Play the Game:**

    Use W, A, S, D keys to navigate the 3D-rendered world in the terminal.
