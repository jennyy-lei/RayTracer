#include "ImageMap.hpp"

#include <iostream>

#include "Globals.hpp"

using namespace std;
using namespace glm;

ImageMap::ImageMap(string name) {
  uint err = lodepng::decode(img, w, h, name, LCT_RGB);
}

vec3 ImageMap::get_col(float x, float y, uint scale) {
  x = (uint)(x * scale) % w;
  y = (uint)(y * scale) % h;
  // cout << x << " , " << y << endl;
  vec3 col = vec3(
    img[3 * (w * y + x) + 0],
    img[3 * (w * y + x) + 1],
    img[3 * (w * y + x) + 2]
  );
  return col;
}

vec3 BumpMap::get_val(float x, float y, uint scale) {
  // cout << "get: " << x << ", " << y << endl;
  // cout << "col: " << get_col(x,y).r << ", " << get_col(x,y).g << ", " << get_col(x,y).b << endl;
  vec3 col = get_col(x, y, scale) * 2.0f / 255.0f - 1.0f;
  return col;
}

vec3 TextureMap::get_val(float x, float y, uint scale) {
  // cout << col.r << ", " << col.g << ", " << col.b << endl;
  return get_col(x, y, scale) / 255.0f;
}
