#pragma once

namespace WallPlacer {

  typedef int OpType;
  typedef uint64_t VertexId;

  class Fabric {
    int nRows;
    int nCols;
    
  public:
    Fabric(const int nRows_, const int nCols_) : nRows(nRows_), nCols(nCols_) {}

    int numRows() const { return nRows; }
    int numCols() const { return nCols; }    
  };

  class Application {
    VertexId nextVert;

  public:

    Application() : nextVert(1) {}

    VertexId addNode(const OpType tp) {
      auto v = nextVert;
      nextVert++;
      return v;
    }
  };
}
