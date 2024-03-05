#pragma once


// https://github.com/Rekkonnect/GDAPI/blob/master/GDAPI/GDAPI/Information/GeometryDash/Speeds.cs

// The units the player moves per second with a speed multiplier of 1.0
#define PLAYER_BASE_SPEED 360

// The speed multiplier for the slow speed (PLAYER_SLOW_SPEED / PLAYER_BASE_SPEED)
#define PLAYER_SLOW_SPEED_MULTIPLIER (0.697 + 0.001 * 2.0 / 3.0)
// The speed multiplier for the normal speed (PLAYER_NORMAL_SPEED / PLAYER_BASE_SPEED)
#define PLAYER_NORMAL_SPEED_MULTIPLIER 0.8655
// The speed multiplier for the fast speed (PLAYER_FAST_SPEED / PLAYER_BASE_SPEED)
#define PLAYER_FAST_SPEED_MULTIPLIER (1.0761 + 0.0001 * 2.0 / 3.0)
// The speed multiplier for the faster speed (PLAYER_FASTER_SPEED / PLAYER_BASE_SPEED)
#define PLAYER_FASTER_SPEED_MULTIPLIER 1.3
// The speed multiplier for the fastest speed (PLAYER_FASTEST_SPEED / PLAYER_BASE_SPEED)
#define PLAYER_FASTEST_SPEED_MULTIPLIER 1.6

// The speed in units per second of the slow speed
#define PLAYER_SLOW_SPEED 251.16
// The speed in units per second of the normal speed
#define PLAYER_NORMAL_SPEED 311.58
// The speed in units per second of the fast speed
#define PLAYER_FAST_SPEED 387.42
// The speed in units per second of the faster speed
#define PLAYER_FASTER_SPEED 468
// The speed in units per second of the fastest speed
#define PLAYER_FASTEST_SPEED 576



// These two values were found using trail and error knowing that:
// - The lowest possible block to survive a jump under is three blocks in above the ground minus 5.5 coordinate points on the y axis
//   - This isn't true at the moment with the new player speeds
//   - TODO: make this work
// - The hardest possible triple spike you can survive has the last spike offset by +13 coordinate points on the x axis
//   - This is true at the moment with the new player speeds
// Given that they're round values, they probably are the real values, but I will have to confirm that once I get
// around to making a GD mod to show me or once hacks come to GD 2.2
// #define PLAYER_GRAVITY_FORCE 2800.0
// #define PLAYER_JUMP_FORCE 600.0

#define PLAYER_GRAVITY_FORCE 2800.0
#define PLAYER_JUMP_FORCE 610.0


// Physics
#define SOLID_COLLISION_TPS 60
