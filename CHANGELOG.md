# tomb5 changelog

## Unreleased

### Original bug fixes

- Restore support for accented characters in the setup window.
- Fix VCI headset size/rotation.
- Fix issues with window creation.
- Fix vertical text space.
- Fix keypad scale/position and keypad text positions.
- Fix cloth and bottle inventory positions.
- Fix Lara getting stuck in front of some crawlspaces during some crawl animations.
- Fix the "Close the door quickly!" track not playing in Escape with the Iris.
- Fix line sparks (like Steel Door welding effects) disapparing.
- Fix some music tracks cutting off prematurely.
- Fix title tracks not looping.
- Fix Labyrinth fishye not covering the entire screen.
- Fix Lara's holsters and back weapon not getting affected by fog.
- Fix underwater shimmer discontinuity.
- Fix upper arm underwater ambience.

### tomb5 bug fixes

- Fix wrecking ball behavior.
- Fix floor lasers staying on after lara dies.
- Fix floor lasers sprite UVs.
- Fix multiple little issues in cutscenes.
- Fix level select text.
- Fix level select arrows.
- Fix the cutscene selector.
- Fix multiple little issues in the inventory.
- Fix VCI headset name.
- Fix "Loading" text position.
- Fix credit cutscenes.
- Fix monoscreen corruption.

### tomb5 additions

- Move input to DirectInput8.
- Improve wrecking ball shadow.
- Enable the debug cutscene selector menu.
- Improve the cutscene selector's text positions.
- Improve the PSX shadow sprite, and make it tilt with the floor.
- Add new scaling system.
- Use new scale for bar drawing.
- Use new scale for lasersight sprite and improve its size.
- Improve rain scale and restore ripples with rain hits water.
- Improve underwater dust drawing, and make it customizable.
- Improve text drawing using the new scale system.
- Use new scale for title logo drawing.
- Use new scale for pickup display drawing.
- Improve inventory item screen positioning.
- Raise internal memory limit.
- Rework moon drawing to fix all issues.
- Remove 16 bit option.
- Improve drip size, color, and duration.
- Remove redundant and confusing options.
- Heavily optimize and greatly reduce memory usage in effect drawing.
- Move all matrix operations to floats.
- Stop skipping title cutscenes.
- Localize special features text.


## [3.2.0](https://github.com/Trxyebeep/TOMB5/tree/V3.2.0) (Feb. 4 2023)

### Original bug fixes

- Fix UV rotate texture in Escape with the Iris elevator scene.
- Restore support for accented characters. (Fixes Non-English compatibility with the patch)
- Fix green flash when twogun shoots.
- Restore missing pictures from the Next Generation Concept Art special features menu.

### tomb5 bug fixes

- Fix multiple little decompilation bugs in title flyby.
- Many little style and code stability/readability/format fixes.
- Fix portal effect corruption.
- Fix sniper overlay.
- Fix Larson movement.

### tomb5 additions

- make hair follow weapon animations (Fixes hair getting pulled when using the HK)
- Improve star drawing in the Ireland levels
- Adjust InfraRed to be less strong.
- Implement Floor Lasers drawing.
- Remove fire from lara in DOZY mode.
- Implement PSX Gas Cloud drawing.

## [3.1.1](https://github.com/Trxyebeep/TOMB5/tree/V3.1.1) (Sep. 12 2022)

### Original bug fixes

- Fix wrong lighting on the mansion door in the title.
- Completely reword the credits, fixing many issues such as text overlap.

### tomb5 bug fixes

- Fix HK ammo not decreasing in lasersight burst mode.
- Fix TR4 camera deadlock.
- Fix left holster reloading in some places.

### tomb5 additions

- Implement the portal effect in Trajan's Markets.

## [3.1.0](https://github.com/Trxyebeep/TOMB5/tree/V3.1.0) (Sep. 3 2022)

### Original bug fixes

- Fix broken distance fog formula for room vertices.
- Fix the game hanging when trying to launch multiple instances.
- Fix issues with the window border.

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
