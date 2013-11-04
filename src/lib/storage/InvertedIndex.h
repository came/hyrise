// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#ifndef SRC_LIB_STORAGE_INVERTEDINDEX_H_
#define SRC_LIB_STORAGE_INVERTEDINDEX_H_

#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "helper/types.h"

#include "storage/storage_types.h"
#include "storage/AbstractIndex.h"
#include "storage/AbstractTable.h"

#include <memory>

template<typename T>
class InvertedIndex : public AbstractIndex {
private:
  typedef std::map<T, pos_list_t> inverted_index_t;
  inverted_index_t _index;

  pos_list_t getPositionsBetween(typename inverted_index_t::const_iterator begin, const typename inverted_index_t::const_iterator end) {
    pos_list_t pos;
    while(begin != end) {
      pos.insert(pos.end(), begin->second.begin(), begin->second.end());
      begin++;
    }
    std::sort(pos.begin(), pos.end());
    return pos;
  }

public:
  virtual ~InvertedIndex() {};

  void shrink() {
for (auto & e : _index)
      e.second.shrink_to_fit();
  }

  explicit InvertedIndex(const hyrise::storage::c_atable_ptr_t& in, field_t column) {
    if (in != nullptr) {
      for (size_t row = 0; row < in->size(); ++row) {
        T tmp = in->getValue<T>(column, row);
        typename inverted_index_t::iterator find = _index.find(tmp);
        if (find == _index.end()) {
          pos_list_t pos;
          pos.push_back(row);
          _index[tmp] = pos;
        } else {
          find->second.push_back(row);
        }
      }
    }
  };


  /**
   * returns a list of positions where key was found.
   */
  pos_list_t getPositionsForKey(T key) {
    typename inverted_index_t::iterator it = _index.find(key);
    if (it != _index.end()) {
      return it->second;
    } else {
      pos_list_t empty;
      return empty;
    }
  };

  pos_list_t getPositionsForKeyLT(T key) {
    return getPositionsBetween(_index.cbegin(), _index.lower_bound(key));
  };

  pos_list_t getPositionsForKeyLTE(T key) {
    return getPositionsBetween(_index.cbegin(), _index.upper_bound(key));
  };

  pos_list_t getPositionsForKeyBetween(T a, T b) {
    return getPositionsBetween(_index.lower_bound(a), _index.upper_bound(b));
  };

  pos_list_t getPositionsForKeyGT(T key) {
    return getPositionsBetween(_index.upper_bound(key), _index.cend());
  };

  pos_list_t getPositionsForKeyGTE(T key) {
    return getPositionsBetween(_index.lower_bound(key), _index.cend());
  };

};
#endif  // SRC_LIB_STORAGE_INVERTEDINDEX_H_
