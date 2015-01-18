//
//  slib_hashtable.h
//
//  Created by Jinglei Ren on 12/23/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_LIB_SLIB_HASHTABLE_H_
#define YCSB_C_LIB_SLIB_HASHTABLE_H_

#include "slib/hashtable.h"
#include "lib/string.h"
#include "lib/string_hashtable.h"

namespace vmp {

template <typename V, class Alloc>
class SlibHashtable : public StringHashtable<V> {
 public:
  typedef typename StringHashtable<V>::KVPair KVPair;

  V Get(const char *key) const; ///< Returns NULL if the key is not found
  bool Insert(const char *key, V value);
  V Update(const char *key, V value);
  V Remove(const char *key);
  std::vector<KVPair> Entries(const char *key = NULL, size_t n = -1) const;
  size_t Size() const { return table_.size(); }

 private:
  struct HashEqual {
    __attribute__((transaction_safe))
    uint64_t hash(const String &hstr) const { return hstr.hash(); }
    __attribute__((transaction_safe))
    bool equal(const String &a, const String &b) const { return a == b; }
  };

  typedef typename
      slib::hashtable<String, V, HashEqual, Alloc> Hashtable;
  Hashtable table_;
};

template <typename V, class Alloc>
V SlibHashtable<V, Alloc>::Get(const char *key) const {
  V value;
  if (!table_.find(String::Wrap(key), value)) return NULL;
  else return value;
}

template <typename V, class Alloc>
bool SlibHashtable<V, Alloc>::Insert(const char *key, V value) {
  if (!key) return false;
  String skey = String::Copy<Alloc>(key);
  return table_.insert(skey, value);
}

template <typename V, class Alloc>
V SlibHashtable<V, Alloc>::Update(const char *key, V value) {
  if (!table_.update(String::Wrap(key), value)) return NULL;
  return value;
}

template <typename V, class Alloc>
V SlibHashtable<V, Alloc>::Remove(const char *key) {
  std::pair<String, V> removed;
  if (!table_.erase(String::Wrap(key), removed)) return NULL;
  String::Free<Alloc>(removed.first);
  return removed.second;
}

template <typename V, class Alloc>
std::vector<typename SlibHashtable<V, Alloc>::KVPair>
SlibHashtable<V, Alloc>::Entries(const char *key, size_t n) const {
  String skey;
  std::vector<std::pair<String, V>> entries = table_.entries(
      (key ? skey = String::Wrap(key), &skey : NULL), n);
  std::vector<KVPair> results;
  for (auto entry : entries) {
    results.push_back(std::make_pair(entry.first.value(), entry.second));
  }
  return results;
}

} // vmp

#endif // YCSB_C_LIB_SLIB_HASHTABLE_H_

