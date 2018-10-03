#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "fabric.h"

using namespace std;

namespace WallPlacer {

  TEST_CASE("Fabric") {

    SECTION("Initializing fabric") {
      Fabric f(2, 2);

      REQUIRE(f.numRows() == 2);
    }

    SECTION("Placing and routing a simple application") {
      OpType op = 0;

      Fabric f(2, 2);
      for (int r = 0; r < f.numRows(); r++) {
        for (int c = 0; c < f.numCols(); c++) {
          f.addOpToTile(r, c, op);
        }
      }

      Application app;
      auto a = app.addNode(op);
      auto b = app.addNode(op);

      app.addEdge(a, b);

      placeAndRoute(app, f);

      bool allPlaced = true;
      vector<VertexId> nodes= {a, b};
      for (auto node : nodes) {
        bool foundNode = false;
        for (int r = 0; r < f.numRows(); r++) {
          for (int c = 0; c < f.numCols(); c++) {
            cout << "Vertex at " << r << ", " << c << " = " << f.vertexAt(r, c) << endl;
            if (f.vertexAt(r, c) == node) {
              foundNode = true;
              break;
            }
          }
        }

        if (!foundNode) {
          allPlaced = false;
        }
      }

      REQUIRE(allPlaced);
    }
  }

}
