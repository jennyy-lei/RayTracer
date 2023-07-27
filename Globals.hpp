#pragma once

#define NUM_THREADS 14

#define EPSILON 0.01
#define PROGRESS_STEP 5
#define RENDER_BOUNDING_VOLUMES false

#define TRANS true
  #define RECURSION_DEPTH 5

#define SUPERSAMPLING false

#define SHADOW_RAYS true
  #define SOFT_SHADOW_RAY_NUM 30

#define ADAPTIVE_AA true
  #define ADAPTIVE_AA_DEBUG false
  #define ADAPTIVE_AA_THRESHOLD 0.25

#define PHONG_SHADING false

#define CAUSTIC_MAP false
  #define CAUSTIC_MAP_DEPTH 10
  #define CAUSTIC_MAP_PHOTONS false
  #define CAUSTIC_MAP_RAYS_PER_OBJECT 15000
  #define CAUSTIC_MAP_OBJFILE "icosa.obj"
  #define CAUSTIC_MAP_RAYS_PER_FACE 1000
  #define CAUSTIC_MAP_MAX_DIST 10000
  #define CAUSTIC_MAP_CONSTANT 105
  #define CAUSTIC_MAP_K 5

// #define RENDER_X 110
// #define RENDER_Y 30
// #define RANGE 0
// #define RENDER_X 64
// #define RENDER_Y 64
// #define RANGE 0
