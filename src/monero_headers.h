//
// Created by mwo on 5/11/15.
//

#ifndef XMREG01_MONERO_HEADERS_H_H
#define XMREG01_MONERO_HEADERS_H_H

#define DB_LMDB   2
#define BLOCKCHAIN_DB DB_LMDB

#include "cryptonote_core/cryptonote_basic.h"
#include "cryptonote_core/blockchain_storage.h"
#include "cryptonote_core/blockchain.h"

#include "blockchain_db/lmdb/db_lmdb.h"

#include "common/base58.h"


#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "wallet/wallet2.h"

#include "crypto/crypto.h"
#include "crypto/hash.h"

#include "serialization/binary_utils.h"

#include "storages/http_abstract_invoke.h"
#include "file_io_utils.h"

extern "C" {
    #include "crypto/crypto-ops.h"
    #include "crypto/keccak.h"
}

#endif //XMREG01_MONERO_HEADERS_H_H

