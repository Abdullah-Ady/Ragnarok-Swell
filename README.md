
# Ragnarok Swell



Ragnarok Swell is a 2D endless-surfer built in **C++** with **SFML**. You steer a surfer across an endless, scrolling ocean and try to travel as far as you can without crashing. The sea is littered with obstacles to dodge, and as the game grows, monsters will hunt down anyone who strays too close to their hitbox.

---

# Project Vision

The game started as a Programming Fundamentals course project. Its original proposal set out the following vision, which the repository is building towards step by step:

- Steer left and right with the arrow keys to weave through the ocean.
- Collect speed boosters and spend them to escape monsters.
- The longer you survive, the harder it gets: the surfer speeds up (to a cap), more obstacles appear, and monsters track you faster.
- Hitting obstacles three times, or being caught by a monster, ends the run and offers a restart.
- The farthest distance travelled is saved as the high score.
- Extra mechanics: whirlpools that change your direction, camera shake on impact, near-miss combos, and bespoke pixel art.

To see the original game that inspired this project, open `edge://surf` in Microsoft Edge.

---

# Features Added So Far



## 1. Project Setup and Game Window

- Visual Studio project with SFML bundled in `libs/SFML`, so it builds without a separate install step.
- A **1280 x 720** window titled **Surf** (title bar and close button only) with vertical sync on to match the monitor's refresh rate.

## 2. Player Placeholder

- The surfer is currently a **green circle (radius 56)** placed in the upper third of the screen, centred horizontally. Like in Edge's Surf, the player stays put and the world moves around them.
- Shapes only for now; sprites and pixel art come later.

## 3. Scrolling Ocean

- The sea is a checkerboard grid of teal and royal-blue tiles drawn over a deep-blue background.
- The tiles scroll vertically and wrap around seamlessly, which gives the feeling of constant forward motion.

## 4. Frame-Time Based Movement

- Movement is scaled by the time between frames, so the game runs at the same speed on fast and slow machines.
- Frame time is capped at **0.1 s** so a lag spike can never teleport the world.




## 6. Obstacles

- Obstacles (**64 x 64** placeholder blocks) are spawned at random positions in the region around the screen, never inside the visible area, so they never pop into view.
- A cap of **20 obstacles** keeps the density under control.
- Obstacles scroll with the water, in sync with both vertical and horizontal movement.
- Obstacles that fall far behind the surfer are removed automatically so the list never grows forever.

