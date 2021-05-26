//
// Created by wujy on 11/3/18.
//

#ifndef YCSB_C_ROCKSDB_CONFIG_H
#define YCSB_C_ROCKSDB_CONFIG_H

#include "core/db.h"
#include "rocksdb/db.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

using std::string;

namespace ycsbc {
class ConfigRocksDB {
private:
  boost::property_tree::ptree pt_;
  int bloomBits_;
  size_t blockCache_;
  string optionsPath_;

public:
  ConfigRocksDB(){};
  void init(const string dbConfig) {
    boost::property_tree::ini_parser::read_ini(dbConfig, pt_);
    bloomBits_ = pt_.get<int>("config.bloomBits");
    blockCache_ = pt_.get<size_t>("config.blockCache");
    optionsPath_ = pt_.get<string>("config.optionsPath");
  }

  int getBloomBits() { return bloomBits_; }

  size_t getBlockCache() { return blockCache_; }

  string getOptionsPath() { return optionsPath_; }
};
} // namespace ycsbc

#endif // YCSB_C_ROCKSDB_DB_H
