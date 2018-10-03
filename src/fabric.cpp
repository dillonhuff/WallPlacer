#include "fabric.h"

#include <iostream>

using namespace dbhc;
using namespace std;

namespace WallPlacer {

  class PlaceInfo {
  public:
    std::set<VertexId> unplaced;
  };

  std::vector<GridPosition>
  routeFrom(const VertexId sourceVertex,
            const GridPosition currentPos,
            const GridPosition endPos,
            Fabric& f,
            std::set<GridPosition>& alreadyChecked) {
    cout << "Routing from " << currentPos << " to " << endPos << endl;
    f.print(cout);
    alreadyChecked.insert(currentPos);

    cout << "Following from " << currentPos << " to " << endPos << endl;

    if (currentPos == endPos) {
      return {endPos};
    }

    vector<GridPos> route{currentPos};
    for (auto nextTile : f.compassNeighbors(currentPos)) {
      cout << "Next tile is " << nextTile << endl;
      if (nextTile == endPos) {
        return {nextTile, endPos};
      }

      if (!elem(nextTile, alreadyChecked) && f.canRouteThrough(nextTile)) {
        f.routeVertexAt(nextTile.first, nextTile.second, sourceVertex);

        auto possibleRoute =
          routeFrom(sourceVertex, nextTile, endPos, f, alreadyChecked);
        if (possibleRoute.size() > 0) {
          return possibleRoute;
        }

        // Undo and continue searching
        f.routeVertexAt(nextTile.first, nextTile.second, NO_VERTEX);
        
      }
    }

    // No path from current position to destination
    f.routeVertexAt(currentPos.first, currentPos.second, NO_VERTEX);
    
    return {};
  }
  
  bool pnrRecursive(const Application& app, Fabric& f, PlaceInfo& info) {
    cout << "Number unplaced = " << info.unplaced.size() << endl;
    if (info.unplaced.size() == 0) {
      return true;
    }

    VertexId toPlace = *std::begin(info.unplaced);

    cout << "Placing " << toPlace << endl;
    for (int r = 0; r < f.numRows(); r++) {
      for (int c = 0; c < f.numCols(); c++) {
        if (!f.tileOccupied(r, c) && f.tileSupports(r, c, app.opType(toPlace))) {
          
          f.setVertexAt(r, c, toPlace);

          bool routed = true;
          // Try to route to all already placed recievers of this node
          for (auto receiverId : app.receivers(toPlace)) {
            if (!elem(receiverId, info.unplaced)) {
              auto receiver = f.findVertex(receiverId);
              std::set<GridPosition> searched;
              auto route = routeFrom(toPlace, f.findVertex(toPlace), receiver, f, searched);
              if (route.size() == 0) {
                cout << "Couldnt route to " << receiverId << endl;
                routed = false;
                break;
              }
            }
          }

          // Try to route from all already placed sources of this node
          for (auto sourceId : app.sources(toPlace)) {
            if (!elem(sourceId, info.unplaced)) {
              auto source = f.findVertex(sourceId);
              std::set<GridPosition> searched;
              auto route = routeFrom(sourceId, source, f.findVertex(toPlace), f, searched);
              if (route.size() == 0) {
                cout << "Couldnt route to " << sourceId << endl;
                routed = false;
                break;
              }
            }
          }

          cout << "After pnr" << endl;
          f.print(cout);

          if (routed) {
            info.unplaced.erase(toPlace);

            bool placedRest = pnrRecursive(app, f, info);
            if (placedRest) {

              cout << "Placed " << toPlace << endl;
              return true;
            } else {
              // Undo modifications to search state and continue
              info.unplaced.insert(toPlace);
              f.setVertexAt(r, c, NO_VERTEX);
              f.clearRouting(toPlace);
              cout << "After clearing " << toPlace << endl;
              f.print(cout);
            }
          } else {
            info.unplaced.insert(toPlace);            
            f.setVertexAt(r, c, NO_VERTEX);
            f.clearRouting(toPlace);
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
