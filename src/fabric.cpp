#include "fabric.h"

#include <iostream>

using namespace std;

namespace WallPlacer {

  void placeAndRoute(const Application& app, Fabric& f) {
    int i = 0;
    for (auto node : app.nodes()) {
      f.setVertexAt(0, i, node);
      i++;
    }
  }
}
