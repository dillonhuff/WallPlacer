#include "fabric.h"

#include <iostream>

using namespace std;

namespace WallPlacer {

  void placeAndRoute(const Application& app, Fabric& f) {
    int i = 0;
    for (auto node : app.nodes()) {
      cout << "i = " << i << endl;
      cout << "node = " << node << endl;
      assert(f.tileSupports(1, i, app.opType(node)));
      f.setVertexAt(1, i, node);
      i++;
    }
  }
}
