#include "fabric.h"

#include <iostream>

using namespace std;

namespace WallPlacer {

  void placeAndRoute(const Application& app, Fabric& f) {
    for (auto node : app.nodes()) {

      bool placed = false;
      for (int r = 0; r < f.numRows(); r++) {
        for (int c = 0; c < f.numCols(); c++) {
          if (!f.tileOccupied(r, c) && f.tileSupports(r, c, app.opType(node))) {
            cout << "Placing " << node << endl;
            f.setVertexAt(r, c, node);
            placed = true;
            break;
          }
        }

        if (placed) {
          break;
        }
      }

    }
  }
}
