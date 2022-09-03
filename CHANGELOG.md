# tomb5 changelog

## [3.1.0](https://github.com/Trxyebeep/TOMB5/tree/V3.1.0) (Sep. 3 2022)

### Original bug fixes

- Fix broken distance fog formula for room vertices.
- Fix the game hanging when trying to launch multiple instances.

### tomb5 bug fixes

- Fix crane cutscene crash in The Base.
- Fix Pause Menu deadlock while using the binoculars.
- Fix HK Gun not spawning gunshells.

### tomb5 additions

- Improve the dynamic lighting formula on statics.
- Add distance fog for object and static mesh vertices.
- Make the distance fog starting point customizable.
- Restore PSX Vertex shimmer effects.
- Restore the sprite on the steel door in Escape with the Iris.
- Add flyby skipper.
- Add ammotype hotkeys.
- Make look transparency customizable.

## [3.0.0](https://github.com/Trxyebeep/TOMB5/tree/V3.0.0) (Apr. 6 2022)

### Original bug fixes

- Fix the bridge bug.
- Restore and fix flyby roll.
- Fix corrupt inventory background in 32 bit color mode.
- Fix fades.
- Fix pictures not drawing in 32 bit color mode.

### tomb5 bug fixes

- Fix flares turning white near statics.
- Fix the moon disappearing behind the clouds.
- Fix twogun invincibility.
- Fix spiders reloading in 13th Floor.
- Fix inventory exitting after save.

### tomb5 additions

- Better colsub blending mode.
- Adapt footprints and smoke to new colsub mode.
- Better footprint graphics.
- Tilt footprints with the floor.
- Add PSX style bars.
- Add TR4 style bars.
- Implement brand new inventory background.
- Make PSX sky usage optional.
- Add Discord RPC.
- Completely rework the stars.
- Add more accurate PSX sprite shadow mode.
- Make mono screen (inventory background) style customizable.
- Allow quick reloading while dead.
- Extend the time it takes for the death menu to pop up.
- Add optional loading text.

## [2.2.0](https://github.com/Trxyebeep/TOMB5/tree/V2.2.0) (Jan. 11 2022)

### Original bug fixes

- Fix Red Alert hitman disappearing in gas chamber
- Fix rain splashes on the floor
- Fix the random white flashes on Lara in multiple levels

### tomb5 bug fixes

- Fix hydra shooting
- Fix not being able to pull pushables
- Fix look_at camera

### tomb5 additions

- New system to add new things to savegames, Dash Timer now saved
- Separate holster drawing to restore tr3-like holster behavior with the revolver
- TR4 camera

## [2.1.0](https://github.com/Trxyebeep/TOMB5/tree/V2.1.0) (Dec. 18 2021)

### Original bug fixes

- Fix flipmap saving
- Fix sample playing for samples with randomness > 127

### tomb5 bug fixes

- Fix rolling with controllers

### tomb5 additions

- Move cheats to inventory
- Add gameover menu
- Add hair-floor collision
- Remember last title menu instead of always going to load menu.
- Add subtractive blending mode
- Add option to remove fog
- Add dynamic light support for static meshes

## [2.0.0](https://github.com/Trxyebeep/TOMB5/tree/V2.0.0) (Nov. 29 2021)

### tomb5 bug fixes

- Fix puzzle hole reload issue

### tomb5 additions

- New menu in the title to customize stuff
- Make cutscene skipper available and customizable
- Make cheats available and customizable
- Restore laser sight target sprite
- Make original laser sight sprite bigger
- Customizable bar positions
- Enemy health bars
- Ammo counter
- Hair respects interpolated frames (smooth hair)
- Fix floating hair in Joby levels

## [1.1.0](https://github.com/Trxyebeep/TOMB5/tree/V1.1.0) (Nov. 15 2021)

### Original bug fixes

- Fix object lighting between flipmaps

### tomb5 bug fixes

- Correct sun rotation and intensity
- Fix sequence switches collision
- Correct 13th floor lasers
- Fix sky to be more PSX-like
- More tweaks to footprints, fixes flickering of footprints at some camera angles

### tomb5 additions

- Add shadows for all objects
- Change hair drawing to use floats
- Go directly to load menu after death

## [1.0.0](https://github.com/Trxyebeep/TOMB5/tree/V1.0.0) (Nov. 10 2021)

### Original bug fixes

- Re-enable a broken cutscene in Escape with the Iris
- Fix HK torso/head rotation
- Multiple fixes to Lara gliding and hair jumps in the Rome cutscenes
- Restore the full security breach cutscene, and the stealth cutscene audio, no external patches needed
- Fix Lara's position when interacting with multiple objects (pushables, crowbar door, etc.)
- Fix the bugged hand when throwing the torch in Ireland levels
- Fix hair going through neck when crawling
- Fix stiff hair when wading
- Fix the floating hair behind Lara's back
- Fix camera when pulling out of water, and push/pulling blocks
- Fix multiple lara hand deadlocks after certain actions, such as falling from tightrope
- Fix Lara's flare hand while running, getting pickups, and airlock switches
- Fix Lara wading in shallow water
- Fix white mafia death animations
- Fix the tip of the HK not being visible in the inventory
- Fix the HK not being displayed correctly in Escape with the Iris
- Fix HK appearing in combine list
- Fix swipe card color in VCI levels
- Fix flickering of trapdoors if they open into another room
- Fix drawing of weapons on Lara's back
- Fix Lara's nose texture
- Fix problems with medipack hotkeys
- Fix Lara's mesh color in water
- Fix skies in all levels
- Fix lighting

### tomb5 additions

- Draw moon and stars in Ireland levels
- Add footprints
- Add PSX-like shadow
- Add duckroll
- Add missing keypad sounds
- Remove multiple frame/animation number restrictions from Lara's moveset to enable more fluid movement, ex: pull ups, crawling
- Add proper FOV calculations from TR2Main (Thank you Arsunt!)
- Add weapon hotkeys
