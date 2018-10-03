#include "fabric.h"

#include <iostream>

using namespace dbhc;
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

          bool routed = true;
          // Try to route to all already placed recievers of this node
          for (auto receiverId : app.receivers(toPlace)) {
            if (!elem(receiverId, info.unplaced)) {
              auto receiver = f.findVertex(receiverId);
              bool canRoute = routeFrom(toPlace, toPlace, receiver, app, f, {});
              if (!canRoute) {
                routed = false;
                break;
              }
            }
          }

          // Try to route from all already placed sources of this node
          for (auto sourceId : app.sources(toPlace)) {
            if (!elem(sourceId, info.unplaced)) {
              auto source = f.findVertex(sourceId);              
              bool canRoute = routeFrom(source, source, toPlace, app, f, {});
              if (!canRoute) {
                routed = false;
                break;
              }
            }
          }

          if (routed) {
            info.unplaced.erase(toPlace);
            bool placedRest = pnrRecursive(app, f, info);
            if (placedRest) {
              return true;
            } else {
              // Undo modifications to search state and continue
              f.setVertexAt(r, c, NO_VERTEX);
              f.clearRouting(toPlace);
              info.unplaced.insert(toPlace);
            }
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
