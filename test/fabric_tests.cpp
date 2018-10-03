#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "fabric.h"

using namespace std;

namespace WallPlacer {

  bool allPlaced(const Application& app, const Fabric& f) {
    bool allPlaced = true;
    for (auto node : app.nodes()) {
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
        cout << "Error: Could not find " << node << endl;
        allPlaced = false;
      }
    }


    return allPlaced;
  }

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

      REQUIRE(allPlaced(app, f));
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

      REQUIRE(f.allSupportedOps(0, 0).size() == 0);
      REQUIRE(f.allSupportedOps(0, 1).size() == 2);
      REQUIRE(f.allSupportedOps(3, 2).size() == 1);

      Application app;
      auto in0 = app.addNode(load);
      auto c0 = app.addNode(compute);
      auto c1 = app.addNode(compute);
      auto out0 = app.addNode(store);
      auto out1 = app.addNode(store);

      app.addEdge(in0, c0);
      app.addEdge(in0, c1);
      app.addEdge(c0, out0);
      app.addEdge(c1, out1);

      placeAndRoute(app, f);

      REQUIRE(allPlaced(app, f));

      f.print(cout);

      //REQUIRE();
    }
  }

}
