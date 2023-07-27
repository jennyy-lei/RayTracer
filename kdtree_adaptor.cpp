#include "kdtree_adaptor.hpp"
#include <iostream>

KDTreeMetric::KDTreeMetric(const KDTreeAdaptor& _data_source) : data_source(_data_source) {}

float KDTreeMetric::evalMetric(const KNNQuery *a, const uint b_idx, size_t size) const {
  // std::cout << "afsfd " << a[2] << " " << a[3] <<std::endl;
  // if (a[0].id != data_source.kdtree_get_pt_id(b_idx)) {
  //   return INFINITY;
  // }

  const float diff0 = a[0].val - data_source.kdtree_get_pt(b_idx, 0);
  const float diff1 = a[1].val - data_source.kdtree_get_pt(b_idx, 1);
  const float diff2 = a[2].val - data_source.kdtree_get_pt(b_idx, 2);
  
  float result = diff0 * diff0 + diff1 * diff1 + diff2 * diff2;
  return result;
}


KDTreeAdaptor::KDTreeAdaptor(const std::vector<Photon>& photons) : m_photons(photons) {
  assert(photons.size() != 0);
  index = new index_t(
    3, *this /* adaptor */,
    nanoflann::KDTreeSingleIndexAdaptorParams(15));
}
KDTreeAdaptor::~KDTreeAdaptor() { delete index; }

const KDTreeAdaptor& KDTreeAdaptor::derived() const { return *this; }
KDTreeAdaptor& KDTreeAdaptor::derived() { return *this; }

template <class BBOX>
bool KDTreeAdaptor::kdtree_get_bbox(BBOX& /*bb*/) const {
  return false;
}