# Ragnarok Swell 

**Ragnarok Swell** is a 2D endless-surfer game developed in **C++** using **SFML 2.6.1** as a Programming Fundamentals course project.

Inspired by Microsoft Edge's Surf game, Ragnarok Swell places you in the middle of an endless ocean where you must navigate through obstacles, collect useful power-ups, and escape the monster hunting you. Your objective is to survive for as long as possible, travel the greatest distance, and secure your highscore a place on the high-score/leaderboard.

The ocean never stops moving, and neither does the danger. As you travel farther, the game becomes faster, making survival increasingly challenging.

---

##  Features

### 1. Player Movement
- Control the surfer using **WASD** or the **Arrow Keys**.
- Move left and right to avoid incoming obstacles.
- Use the up key to temporarily stop the ocean's movement.
- The surfer remains centred in the playing area while the ocean and objects move around them.
- Different surfer sprites are displayed depending on the direction of movement.

### 2. Scrolling Ocean
- A continuously scrolling ocean creates the illusion of forward movement.
- Both horizontal and vertical movement are supported.
- The ocean texture repeats seamlessly, preventing visible gaps during scrolling.
- Obstacles and monsters move consistently with the ocean.

### 3. Obstacles and Collision Detection
- Seven different obstacle designs are randomly selected during gameplay.
- Obstacles spawn at random positions outside the visible playing area.
- A spacing system prevents newly spawned objects from overlapping existing obstacles, boosters, or life pickups.
- Colliding with an obstacle costs one life.
- A temporary invincibility period prevents repeated damage from continuous collisions.
- The player is briefly slowed down after losing a life.

### 4. Lives System
- The player starts with **3 lives**.
- Losing all three lives ends the game.
- Life pickups occasionally spawn at predetermined positions in the ocean.
- Collecting a life pickup restores one life, up to the maximum of three.

### 5. Speed Boosters
- Boosters occasionally appear at predetermined positions.
- Collecting a booster adds one charge to the player's inventory.
- A maximum of **3 boosters** can be stored at a time.
- Press **F** to activate a booster.
- Each booster doubles the player's scrolling speed for **2 seconds**.
- Boosters can be used strategically to move through dangerous sections of the ocean.

### 6. Monster System – Jörmungandr
- A monster known as **Jörmungandr** occasionally appears in the ocean.
- Monsters initially move with the scrolling environment.
- When a monster comes sufficiently close to the player, it begins actively chasing them.
- Its movement is calculated using the distance and direction between itself and the player.
- Monsters can be destroyed by colliding with normal obstacles.
- If a monster catches the player, the game immediately ends.

### 7. Increasing Difficulty
- The game becomes progressively faster as the player travels farther.
- Once the player reaches **1000 metres**, the ocean and monsters increase their speed by 30%.
- This makes avoiding obstacles and escaping monsters increasingly difficult.

### 8. Distance and High-Score System
- The player's distance travelled is continuously calculated and displayed.
- The current distance acts as the player's score.
- High scores are saved in `highscores.txt`.
- The game loads previously saved scores when launched.
- A Bubble Sort algorithm arranges scores in descending order.
- Only the **top 10 scores** are retained.
- The highest recorded score is displayed during gameplay.

### 9. Start Screen and Game Over Screen
- A dedicated start screen displays the game title.
- An instructions section can be opened from the start screen.
- The game remains paused until the player chooses to start.
- A game-over overlay appears when the player runs out of lives.
- Press **Enter** to restart after a game over.
- Press **Escape** to return to the start screen.

### 10. Audio and Visual Assets
- Custom image assets are used for the surfer, ocean, obstacles, boosters, life pickups, monsters, and title screen.
- Background music plays continuously and loops throughout the game.
- Text elements display the player's lives, available boosters, current distance, and highest score.

---

##  Controls

| Key | Action |
|---|---|
| `W` / `↑` | Stop ocean movement |
| `A` / `←` | Move left |
| `S` / `↓` | Resume forward movement |
| `D` / `→` | Move right |
| `F` | Activate a speed booster |
| `Enter` | Restart after game over |
| `Esc` | Return to the start screen |

---

## Built With

- **C++** – Core programming language.
- **SFML 2.6.1** – Graphics, window management, input handling, and audio.
- **Visual Studio** – Development environment and project compilation.

##  Installation and Running the Game

Ragnarok Swell is configured to run directly from the cloned repository. All essential project files and dependencies are included, so **no separate SFML installation or manual library configuration is required**.

### Requirements

- Windows operating system.
- Microsoft Visual Studio Community 2026.
- **Desktop development with C++** workload installed in Visual Studio.

### Method 1: Clone Using the Command Line

1. Open Command Prompt, PowerShell, or a terminal.
2. Clone the repository:

   ```bash
   git clone https://github.com/Abdullah-Ady/Ragnarok-Swell.git
   ```

3. Open the downloaded `Ragnarok-Swell` folder.
4. Open **`Surf project.slnx`** in Visual Studio.

### Method 2: Clone Directly Through Visual Studio (UI)

If you prefer not to use the command line, you can download the entire project directly through Visual Studio Community 2026.

1. Launch **Visual Studio Community 2026**.
2. On the start window, select **Clone a repository**.
3. In the Repository location field, enter:

   ```text
   https://github.com/Abdullah-Ady/Ragnarok-Swell.git
   ```

4. Choose the local folder where you want to save the project.
5. Click **Clone** and wait for Visual Studio to download the repository.
6. Once cloning is complete, open the downloaded project folder.
7. Locate and open **`Surf project.slnx`** in Visual Studio.

### Building and Running the Game

After opening the solution using either method:

1. Allow Visual Studio to load the project and its existing configuration.
2. Build the project using **Ctrl + Shift + B**.
3. Run the game using **F5** (with debugging) or **Ctrl + F5** (without debugging).

### Included Dependencies and Assets

The repository contains everything required to build and run the game:

- **SFML 2.6.1:** Bundled graphics, audio, and system libraries.
- **Project Configuration:** Visual Studio solution and project files with the necessary build settings.
- **Images:** All player, ocean, obstacle, monster, booster, life, and title-screen assets.
- **Fonts:** The font files required for the game's interface.
- **Music:** Background music used during gameplay.
- **High Scores:** A `highscores.txt` file for storing player records.

Simply clone the repository, open the solution, build, and run. No additional dependency downloads or manual SFML setup are necessary.
