//
// Created by marcin on 31/01/16.
//

#ifndef MYMONERO_ACCOUNT_BASE2_H
#define MYMONERO_ACCOUNT_BASE2_H

#include "monero_headers.h"

namespace xmreg {

    class simple_account {

    public:
        void create_from_keys(
                const cryptonote::account_public_address& address,
                const crypto::secret_key& spendkey,
                const crypto::secret_key& viewkey);



        std::string get_public_address_str(bool testnet) const;

        template <class t_archive>
        inline void serialize(t_archive &a, const unsigned int /*ver*/)
        {
            a & m_keys;
            a & m_creation_timestamp;
        }

        BEGIN_KV_SERIALIZE_MAP()
            KV_SERIALIZE(m_keys)
            KV_SERIALIZE(m_creation_timestamp)
        END_KV_SERIALIZE_MAP()

    private:
        void set_null();
        cryptonote::account_keys m_keys;
        uint64_t m_creation_timestamp;
    };


}


#endif //MYMONERO_ACCOUNT_BASE2_H
