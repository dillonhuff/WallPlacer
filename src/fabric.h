#pragma once

#include <cassert>
#include <cstdint>

#include "algorithm.h"

#define NO_VERTEX 0

namespace WallPlacer {

  typedef int OpType;
  typedef uint64_t VertexId;
  typedef uint64_t EdgeId;

  class GridPosition {
  public:
    int first;
    int second;
  };

  typedef GridPosition GridPos;

  static inline bool notEmpty(const GridPosition p) {
    return (p.first != -1) && (p.second != -1);
  }

  static inline std::ostream& operator<<(std::ostream& out, const GridPosition p) {
    out << "(" << p.first << ", " << p.second << ")";
    return out;
  }

  static inline bool equal(const GridPosition x, const GridPosition y) {
    return (x.first == y.first) && (x.second == y.second);
  }

  static inline bool operator==(const GridPosition x, const GridPosition y) {
    return equal(x, y);
  }

  static inline bool operator<(const GridPosition x, const GridPosition y) {
    if (x.first < y.first) {
      return true;
    }

    if (x.first == y.first) {
      return x.second < y.second;
    }

    return false;
  }
  
  class Fabric {
    int nRows;
    int nCols;

    std::vector<VertexId> mapping;
    std::vector<VertexId> routeMapping;    
    std::vector<std::set<OpType> > supportedOps;
    
  public:
    Fabric(const int nRows_, const int nCols_) : nRows(nRows_), nCols(nCols_) {
      mapping.resize(numRows()*numCols());
      routeMapping.resize(numRows()*numCols());      
      supportedOps.resize(numRows()*numCols());

      for (int r = 0; r < numRows(); r++) {
        for (int c = 0; c < numCols(); c++) {
          setVertexAt(r, c, NO_VERTEX);
          routeVertexAt(r, c, NO_VERTEX);
        }
      }
    }

    bool routedAt(const GridPosition sourcePosition,
                  const GridPosition location) const {
      VertexId id = vertexAt(sourcePosition.first, sourcePosition.second);
      if (id == NO_VERTEX) {
        return false;
      }

      if (sourcePosition == location) {
        return true;
      }

      return false;
    }

    bool isInBounds(const int r, const int c) const {
      if (r < 0 || c < 0) {
        return false;
      }

      if (r >= numRows()) {
        return false;
      }

      if (c >= numCols()) {
        return false;
      }

      return true;
    }

    std::vector<GridPosition> compassNeighbors(const GridPosition p) const {
      int r = p.first;
      int c = p.second;

      assert(0 <= r);
      assert(r < numRows());      

      assert(0 <= c);
      assert(c < numCols());

      std::vector<GridPosition> neighbors;
      for (int i = -1; i < 2; i += 2) {
        GridPosition neighbor{r + i, c};
        if (isInBounds(neighbor.first, neighbor.second)) {
          neighbors.push_back(neighbor);
        }
      }

      for (int i = -1; i < 2; i += 2) {
        GridPosition neighbor{r, c + i};
        if (isInBounds(neighbor.first, neighbor.second)) {
          neighbors.push_back(neighbor);
        }
      }

      return neighbors;
    }

    template<typename F>
    void forAllTiles(F f) {
      for (int r = 0; r < numRows(); r++) {
        for (int c = 0; c < numCols(); c++) {
          f(GridPosition{r, c});
        }
      }
    }

    void clearRouting(const VertexId v) {
      forAllTiles([this, v](GridPosition p){
          if (this->vertexRoutedAt(p.first, p.second) == v) {
            this->routeVertexAt(p.first, p.second, NO_VERTEX);
          }
        });
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

    VertexId vertexRoutedAt(const int row, const int column) const {
      return routeMapping[row*numCols() + column];
    }
    
    void setVertexAt(const int row, const int column, VertexId vert) {
      mapping[row*numCols() + column] = vert;
    }

    void routeVertexAt(const int row, const int column, VertexId vert) {
      routeMapping[row*numCols() + column] = vert;
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

    std::vector<VertexId> receivers(const VertexId a) const {
      return dbhc::map_find(a, outEdges);
    }

    std::vector<VertexId> sources(const VertexId a) const {
      return dbhc::map_find(a, inEdges);
    }
    
    std::pair<VertexId, VertexId> getEdgeVertexes(const EdgeId edge) const {
      return dbhc::map_find(edge, edgeMap);
    }
  };

  bool placeAndRoute(const Application& app, Fabric& f);
}
