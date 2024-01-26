# Timegame

## Core gameplay
- Timegame is a puzzlegame
- Timegame is a platformer
- A level fits on a single screen (at least the first ones, later larger
    labyrinth level can be added)
- "Time is frozen", meaning items stay suspended in air, actions of the player
    dont take effect, rather get queued for later execution
- When the player exits the scene to the right, the player ends up in
    a mirrored version of the current room, but one timestep further into the
    future. Meaning, queued player actions took effect, and the physics 
    simulation executed one timestemp,
    the room the player came from gets pushed onto the stack
- When the player exits the scene to the left, the last scene gets popped 
    from the stack, and becomes the current scene

## Game elements
- Chests. A chests falls 1 unit every timestep. They can be jumped on and block
    lasers / other objects. Some chests can be pushed by the player, this could
    be colorcoded
- The target. Once the player touches the target, the stage is solved
- Laser. The laser continues until it is blocked by a wall or a chest.
- Door. A door does not the player pass, unless it is openend. It can be
    openend with a key
- Key. Can be picked up and carried by the player
- Certain elements are excluded from the time mechanic
- TODO: Pressure plates, levers, buttons, clocks, glass, ...

## TODO
### Prototype
- A few hardcoded level
- Player controller                 // Done
- Chests                            // Done
- Time mechanic                     // Will rework
- Time mechanic excluded objects
- Camera                            // Done
- Gameplay loop (includes target)

### Further?
- Animations
- Level editor + loader
- ImGui renderer
- Texture loading, rendering
- VFX (Portal glowing, particles?)
- Sound
- Lasers, Doors, Keys, Glass, ...
