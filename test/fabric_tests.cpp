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

    SECTION("4 x 4 fabric with ios on edges") {
      Fabric f(6, 6);

      OpType compute = 1;
      OpType load = 2;
      OpType store = 3;

      for (int r = 1; r < f.numRows() - 1; r++) {
        f.addOpToTile(r, 0, load);
        f.addOpToTile(r, 0, store);

        f.addOpToTile(r, 5, load);
        f.addOpToTile(r, 5, store);
      }

      for (int c = 1; c < f.numCols() - 1; c++) {
        f.addOpToTile(0, c, load);
        f.addOpToTile(0, c, store);

        f.addOpToTile(5, c, load);
        f.addOpToTile(5, c, store);
      }
      
      for (int r = 1; r < f.numRows() - 1; r++) {
        for (int c = 1; c < f.numCols() - 1; c++) {
          f.addOpToTile(r, c, compute);
        }
      }
      
    }
  }

}
