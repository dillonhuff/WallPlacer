#include "fabric.h"

#include <iostream>

using namespace std;

namespace WallPlacer {

  class PlaceInfo {
  public:
    std::set<VertexId> unplaced;
  };

  bool pnrRecursive(const Application& app, Fabric& f, PlaceInfo& info) {
    cout << "Number unplaced = " << info.unplaced.size() << endl;
    if (info.unplaced.size() == 0) {
      return true;
    }

    VertexId toPlace = *std::begin(info.unplaced);

    for (int r = 0; r < f.numRows(); r++) {
      for (int c = 0; c < f.numCols(); c++) {
        if (!f.tileOccupied(r, c) && f.tileSupports(r, c, app.opType(toPlace))) {
          
          f.setVertexAt(r, c, toPlace);

          info.unplaced.erase(toPlace);
          bool placedRest = pnrRecursive(app, f, info);
          if (placedRest) {
            return true;
          } else {
            info.unplaced.insert(toPlace);
          }
        }
      }
    }
    
    return false;
  }

  // TODO: Add restrictions on where you can route, cant
  // route through the IOs
  bool placeAndRoute(const Application& app, Fabric& f) {
    PlaceInfo info;
    info.unplaced = {begin(app.nodes()), end(app.nodes())};
    bool res = pnrRecursive(app, f, info);

    return res;
    
    for (auto node : app.nodes()) {

      bool placed = false;
      for (int r = 0; r < f.numRows(); r++) {
        for (int c = 0; c < f.numCols(); c++) {
          if (!f.tileOccupied(r, c) && f.tileSupports(r, c, app.opType(node))) {
            //cout << "Placing " << node << endl;
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
