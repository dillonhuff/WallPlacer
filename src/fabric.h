#pragma once

#include <cassert>
#include <cstdint>

#include "algorithm.h"

#define NO_VERTEX 0

namespace WallPlacer {

  typedef int OpType;
  typedef uint64_t VertexId;
  typedef uint64_t EdgeId;
  typedef std::pair<int, int> GridPosition;
  typedef GridPosition GridPos;

  static inline bool notEmpty(const GridPosition p) {
    return (p.first != -1) && (p.second != -1);
  }

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

    GridPosition findVertex(const VertexId v) const {
      for (int r = 0; r < numRows(); r++) {
        for (int c = 0; c < numCols(); c++) {
          if (vertexAt(r, c) == v) {
            return {r, c};
          }
        }
      }

      return {-1, -1};
    }

    void print(std::ostream& out) {
      for (int r = 0; r < numRows(); r++) {
        
        for (int c = 0; c < numCols(); c++) {
          if (tileOccupied(r, c)) {
            out << vertexAt(r, c) << " ";
          } else {
            out << "X ";
          }
        }

        out << std::endl;
      }
    }

    bool tileOccupied(const int r, const int c) const {
      return mapping[r*numCols() + c] != NO_VERTEX;
    }

    bool tileSupports(const int r, const int c, const OpType tp) const {
      return dbhc::elem(tp, allSupportedOps(r, c));
    }

    const std::set<OpType>& allSupportedOps(const int row, const int column) const {
      return supportedOps[row*numCols() + column];
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
    EdgeId nextEdge;

    std::map<VertexId, OpType> opTypes;
    std::map<EdgeId, std::pair<VertexId, VertexId> > edgeMap;
    std::map<VertexId, std::vector<VertexId> > outEdges;
    std::map<VertexId, std::vector<VertexId> > inEdges;
    
    std::vector<VertexId> allNodes;
    std::vector<EdgeId> allEdges;

  public:

    Application() : nextVert(1), nextEdge(1) {}

    const std::vector<VertexId>& nodes() const { return allNodes; }
    const std::vector<VertexId>& edges() const { return allEdges; }

    OpType opType(const VertexId node) const {
      return dbhc::map_find(node, opTypes);
    }

    VertexId addNode(const OpType tp) {
      auto v = nextVert;
      allNodes.push_back(v);
      opTypes[v] = tp;
      nextVert++;
      return v;
    }

    void addEdge(const VertexId a, const VertexId b) {
      auto e = nextEdge;
      nextEdge++;
      allEdges.push_back(e);
      edgeMap[e] = {a, b};
      dbhc::map_insert(outEdges, a, b);
      dbhc::map_insert(inEdges, b, a);      
    }

    std::pair<VertexId, VertexId> getEdgeVertexes(const EdgeId edge) const {
      return dbhc::map_find(edge, edgeMap);
    }
  };

  void placeAndRoute(const Application& app, Fabric& f);
}
