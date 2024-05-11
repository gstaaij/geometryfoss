
# TODO

- [x] Implement stuff from [GDAPI](https://github.com/Rekkonnect/GDAPI/tree/master/GDAPI/GDAPI/Information/GeometryDash)
  - [x] [Speeds](https://github.com/Rekkonnect/GDAPI/blob/master/GDAPI/GDAPI/Information/GeometryDash/Speeds.cs) ([30e59b2](https://github.com/gstaaij/geometryfoss/commit/30e59b21d046810c813d2a1b337419a691bcf11a))
- [x] Migrate to `TraceLog` for logging (instead of `nob_log`) ([a2c7dc9](https://github.com/gstaaij/geometryfoss/commit/a2c7dc98b2d1f49c7de5e583345c6bfe2ca0c9ef))
- [x] Redo all the easing functions ([77d5a56](https://github.com/gstaaij/geometryfoss/commit/77d5a567de17e3dce496d1a17e2d79d3d2b19aa1))
- [ ] Make the easing functions like the ones in GD (ease, elastic, bounce, exponential, sine, back. Ease and elastic have an easing rate)
- [ ] Add jump pads, jump rings, and portals
- [x] If the deltaTime is too high, don't update (will cause phasing through walls if not) ([e3f9eb3](https://github.com/gstaaij/geometryfoss/commit/e3f9eb369315e5b61cb2e5bbb26de1ce17afa1ba))
- [x] Parse the texture map `.plist` files ([71ac68d](https://github.com/gstaaij/geometryfoss/commit/71ac68d1af91fb97c9523f86f8d7385e885a3fc1) and [71ab4b7](https://github.com/gstaaij/geometryfoss/commit/71ab4b7cebddbaeb690b52d4a6e0704ed883a305))
- [ ] Make the object definition texture system a lot better, we need to be able to draw multiple textures (portals), with some of them behind the player (`portal_xx_back_001.png`), or choose a random one (`pit_01_001.png`, `pit_02_001.png` and `pit_03_001.png`)
- [ ] Make update and draw loops run in seperate threads

## QoL stuff

- [ ] Add little previews for the easing functions in the UI
