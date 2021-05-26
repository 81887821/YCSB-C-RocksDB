//
//  rocksdb_db.cc
//  YCSB-C
//

#include "rocksdb_db.h"
#include "rocksdb/cache.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/flush_block_policy.h"
#include "rocksdb/utilities/options_util.h"
#include "utils/rocksdb_config.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

namespace ycsbc {
RocksDB::RocksDB(const char *dbPath, const string dbConfig) : noResult(0) {
  // get rocksdb config
  ConfigRocksDB config;
  if (dbConfig.empty()) {
    config.init("./RocksDBConfig/rocksdb_config.ini");
  } else {
    config.init(dbConfig);
  }
  int bloomBits = config.getBloomBits();
  size_t blockCache = config.getBlockCache();
  auto cache = rocksdb::NewLRUCache(blockCache);
  string optionsPath = config.getOptionsPath();

  rocksdb::DBOptions loaded_db_opt;
  std::vector<rocksdb::ColumnFamilyDescriptor> loaded_cf_descs;
  rocksdb::ConfigOptions config_options;

  rocksdb::Status s = LoadOptionsFromFile(
      config_options, optionsPath, &loaded_db_opt, &loaded_cf_descs, &cache);
  assert(s.ok());
  if (bloomBits > 0) {
    loaded_cf_descs[0]
        .options.table_factory->GetOptions<rocksdb::BlockBasedTableOptions>()
        ->filter_policy.reset(rocksdb::NewBloomFilterPolicy(bloomBits));
  }

  // loaded_db_opt.db_paths = {
  //     {"/mnt/sdb/testRocksdb/test1", (uint64_t)10 * 1024 * 1024 * 1024},
  //     {"/mnt/sdb/testRocksdb/test2", (uint64_t)10 * 1024 * 1024 * 1024},
  //     {"/mnt/sdb/testRocksdb/test3", (uint64_t)10 * 1024 * 1024 * 1024}};

  s = rocksdb::DB::Open(loaded_db_opt, dbPath, loaded_cf_descs, &handles_,
                        &db_);

  if (!s.ok()) {
    cerr << "Can't open rocksdb " << dbPath << endl;
    exit(0);
  }

  cerr << "write buffer size: " << loaded_cf_descs[0].options.write_buffer_size
       << endl;
  cerr << "write buffer number: "
       << loaded_cf_descs[0].options.max_write_buffer_number << endl;
  cerr << "num compaction trigger: "
       << loaded_cf_descs[0].options.level0_file_num_compaction_trigger << endl;
  cerr << "targe file size base: "
       << loaded_cf_descs[0].options.target_file_size_base << endl;
  cerr << "level size base: "
       << loaded_cf_descs[0].options.max_bytes_for_level_base << endl;

  cerr << "bloom bits: " << bloomBits << endl;
  cerr << "blockcache: "
       << loaded_cf_descs[0]
              .options.table_factory
              ->GetOptions<rocksdb::BlockBasedTableOptions>()
              ->block_cache->GetCapacity()
       << endl;
}

int RocksDB::Read(const std::string &table, const std::string &key,
                  const std::vector<std::string> *fields,
                  std::vector<KVPair> &result) {
  string value;
  rocksdb::Status s = db_->Get(rocksdb::ReadOptions(), key, &value);
  if (s.ok())
    return DB::kOK;
  if (s.IsNotFound()) {
    noResult++;
    cout << noResult << "not found" << endl;
    return DB::kOK;
  } else {
    cerr << "read error" << endl;
    exit(0);
  }
}

int RocksDB::Scan(const std::string &table, const std::string &key, int len,
                  const std::vector<std::string> *fields,
                  std::vector<std::vector<KVPair>> &result) {
  auto it = db_->NewIterator(rocksdb::ReadOptions());
  it->Seek(key);
  std::string val;
  std::string k;
  int i;
  int cnt = 0;
  for (i = 0; i < len && it->Valid(); i++) {
    k = it->key().ToString();
    val = it->value().ToString();
    it->Next();
    if (val.empty())
      cnt++;
  }
  if (i < len) {
    std::cout << " get " << i << " for length " << len << "." << std::endl;
    std::cerr << " get " << i << " for length " << len << "." << std::endl;
  }
  if (cnt > 0) {
    std::cout << cnt << "empty values" << std::endl;
  }
  delete it;
  return DB::kOK;
}

int RocksDB::Insert(const std::string &table, const std::string &key,
                    std::vector<KVPair> &values) {
  rocksdb::Status s;
  for (KVPair &p : values) {
    s = db_->Put(rocksdb::WriteOptions(), key, p.second);
    if (!s.ok()) {
      cerr << "insert error" << s.ToString() << "\n" << endl;
      exit(0);
    }
  }
  return DB::kOK;
}

int RocksDB::Update(const std::string &table, const std::string &key,
                    std::vector<KVPair> &values) {
  return Insert(table, key, values);
}

int RocksDB::Delete(const std::string &table, const std::string &key) {
  vector<DB::KVPair> values;
  return Insert(table, key, values);
}

void RocksDB::printStats() {
  string stats;
  db_->GetProperty("rocksdb.stats", &stats);
  cout << stats << endl;
}

RocksDB::~RocksDB() {
  for (auto *handle : handles_) {
    delete handle;
  }
  delete db_;
}
} // namespace ycsbc
