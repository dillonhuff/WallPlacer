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

      Application app;
      auto a = app.addNode(op);
      auto b = app.addNode(op);
    }
  }

}
