#pragma once

#include <cassert>
#include <cstdint>

#include "algorithm.h"

#define NO_VERTEX 0

namespace WallPlacer {

  typedef int OpType;
  typedef uint64_t VertexId;

  class Fabric {
    int nRows;
    int nCols;

    std::vector<VertexId> mapping;
    std::vector<std::set<OpType> > supportedOps;
    
  public:
    Fabric(const int nRows_, const int nCols_) : nRows(nRows_), nCols(nCols_) {
      mapping.resize(numRows()*numCols());
      supportedOps.resize(numRows()*numCols());

      for (int r = 0; r < numRows(); r++) {
        for (int c = 0; c < numCols(); c++) {
          setVertexAt(r, c, NO_VERTEX);
        }
      }
    }

    int numRows() const { return nRows; }
    int numCols() const { return nCols; }

    VertexId vertexAt(const int row, const int column) const {
      return mapping[row*numCols() + column];
    }

    void setVertexAt(const int row, const int column, VertexId vert) {
      mapping[row*numCols() + column] = vert;
    }
    
    void addOpToTile(const int row, const int column, const OpType op) {
      supportedOps[row*numCols() + column].insert(op);
      assert(dbhc::elem(op, supportedOps[row*numCols() + column]));
    }
  };

  class Application {
    VertexId nextVert;

    std::vector<VertexId> allNodes;

  public:

    Application() : nextVert(1) {}

    const std::vector<VertexId>& nodes() const { return allNodes; }

    VertexId addNode(const OpType tp) {
      auto v = nextVert;
      allNodes.push_back(v);
      nextVert++;
      return v;
    }

    void addEdge(const VertexId a, const VertexId b) {
    }
  };

  void placeAndRoute(const Application& app, Fabric& f);
}
