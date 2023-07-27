#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Light/Light.hpp"
#include "Primitive.hpp"
#include "GeometryNode.hpp"
#include "nanoflann.hpp"

struct Photon {
  POI *poi;
  glm::vec3 i;
  uint depth;
  Light *light;
  glm::vec3 ray_d;

  Photon(POI *poi, glm::vec3 i, uint depth, Light *light, glm::vec3 ray_d)
    : poi{poi}, i{i}, depth{depth}, light{light}, ray_d{ray_d} {}
};

struct KNNResult {
  Photon photon;
  float distance;

  KNNResult(Photon photon, float distance) : photon{photon}, distance{distance} {}
};

struct KNNQuery {
  float val;
  uint id;

  KNNQuery() : val{0}, id{0} {}
  KNNQuery(float val) : val{val}, id{0} {}
  KNNQuery(float val, uint id) : val{val}, id{id} {}

  float operator-(float x) const { return val - x; }
  float operator-(KNNQuery x) const { return val - x.val; }
  float operator+(float x) const { return val + x; }
  float operator+(KNNQuery x) const { return val + x.val; }
  float operator=(float x) { val = x; return val; }
  KNNQuery operator=(const KNNQuery x) {
    val = x.val;
    id = x.id;
    return KNNQuery(val, id);
  }
  bool operator<(const KNNQuery x) const { return id == x.id && val < x.val; }
  bool operator<(float x) const { return val < x; }
  bool operator>(const KNNQuery x) const { return id == x.id && val > x.val; }
  bool operator>(float x) const { return val > x; }
  operator float() { return val; }
};

struct KDTreeAdaptor;
struct KDTreeMetric {
  using ElementType = KNNQuery;
  using DistanceType = float;
  
  const KDTreeAdaptor& data_source;

  KDTreeMetric(const KDTreeAdaptor& _data_source);
  
  float evalMetric(const KNNQuery *a, const uint b_idx, size_t size) const;

  template <typename U, typename V>
  inline float accum_dist(const U a, const V b, const size_t) const {
    return (a - b) * (a - b);
  }
};

struct KDTreeAdaptor {
  typedef nanoflann::metric_L2::traits<float, KDTreeAdaptor>::distance_t metric_t;
  typedef nanoflann::KDTreeSingleIndexAdaptor<metric_t, KDTreeAdaptor, 3, uint> index_t;
  // typedef nanoflann::KDTreeSingleIndexAdaptor<KDTreeMetric, KDTreeAdaptor, 3, uint> index_t;

  /** The kd-tree index for the user to call its methods as usual with any
   * other FLANN index */
  index_t* index = nullptr;

  /// Constructor: takes a const ref to the vector of vectors object with the
  /// data points
  KDTreeAdaptor(const std::vector<Photon>& photons);
  ~KDTreeAdaptor();

  const std::vector<Photon>& m_photons;

  /** @name Interface expected by KDTreeSingleIndexAdaptor
   * @{ */
  const KDTreeAdaptor& derived() const;
  KDTreeAdaptor&       derived();

  // Must return the number of data points
  inline size_t kdtree_get_point_count() const { return m_photons.size(); }

  // Returns the dim'th component of the idx'th point in the class:
  inline float kdtree_get_pt(const size_t idx, const size_t dim) const {
      return m_photons[idx].poi->point[dim];
  }

  // Returns the dim'th component of the idx'th point in the class:
  inline float kdtree_get_pt_id(const size_t idx) const {
      return m_photons[idx].poi->gn->m_nodeId;
  }

  // Optional bounding-box computation: return false to default to a standard
  // bbox computation loop.
  // Return true if the BBOX was already computed by the class and returned
  // in "bb" so it can be avoided to redo it again. Look at bb.size() to
  // find out the expected dimensionality (e.g. 2 or 3 for point clouds)
  template <class BBOX>
  bool kdtree_get_bbox(BBOX& /*bb*/) const;
};
