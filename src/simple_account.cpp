//
// Created by mwo on 31/01/16.
//

#include "simple_account.h"

namespace xmreg {


    void
    simple_account::set_null()
    {
        m_keys = cryptonote::account_keys();
    }

    std::string
    simple_account::get_public_address_str(bool testnet) const
    {
        return get_account_address_as_str(testnet, m_keys.m_account_address);
    }

    void
    simple_account::create_from_keys(
            const cryptonote::account_public_address& address,
            const crypto::secret_key& spendkey,
            const crypto::secret_key& viewkey)
    {

        m_keys = cryptonote::account_keys();

        m_keys.m_account_address = address;
        m_keys.m_spend_secret_key = spendkey;
        m_keys.m_view_secret_key = viewkey;

    }

}
