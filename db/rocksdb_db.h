//
//  rocksdb_db.h
//  YCSB-C
//

#ifndef YCSB_C_ROCKSDB_DB_H_
#define YCSB_C_ROCKSDB_DB_H_

#include "core/db.h"
#include "core/properties.h"
#include "rocksdb/db.h"
#include "rocksdb/env.h"
#include "rocksdb/filter_policy.h"
#include <iostream>
#include <string>
#include <memory>

namespace ycsbc {

class RocksDB : public DB {
public:
  RocksDB(const char *dbPath,const std::string dbConfig);

  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields, std::vector<KVPair> &result);

  int Scan(const std::string &table, const std::string &key, int len,
           const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result);

  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);

  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);

  int Delete(const std::string &table, const std::string &key);

  void printStats();

  ~RocksDB();

private:
  rocksdb::DB *db_;
  unsigned noResult;
  rocksdb::Iterator *it{nullptr};
  std::shared_ptr<rocksdb::FileSystem> fs_guard;
  std::shared_ptr<rocksdb::Env> env_guard;
};

} // namespace ycsbc

#endif // YCSB_C_ROCKS_DB_H_
