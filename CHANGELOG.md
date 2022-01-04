# tomb5 changelog

## [2.1.0](https://github.com/Trxyebeep/TOMB5/tree/V2.1.0) (18/12/2021)

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

## [2.0.0](https://github.com/Trxyebeep/TOMB5/tree/V2.0.0) (29/11/2021)

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

## [1.1.0](https://github.com/Trxyebeep/TOMB5/tree/V1.1.0) (15/11/2021)

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

## [1.0.0](https://github.com/Trxyebeep/TOMB5/tree/V1.0.0) (10/11/2021)

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
