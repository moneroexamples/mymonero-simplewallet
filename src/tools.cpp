//
// Created by marcin on 5/11/15.
//

#include "tools.h"

namespace xmreg
{


    /**
     * Parse key string, e.g., a viewkey in a string
     * into crypto::secret_key or crypto::public_key
     * depending on the template argument.
     */
    template <typename T>
    bool
    parse_str_secret_key(const string& key_str, T& secret_key)
    {

        // hash and keys have same structure, so to parse string of
        // a key, e.g., a view key, we can first parse it into the hash
        // object using parse_hash256 function, and then copy the reslting
        // hash data into secret key.
        crypto::hash hash_;

        if(!parse_hash256(key_str, hash_))
        {
            cerr << "Cant parse a key (e.g. viewkey): " << key_str << endl;
            return false;
        }

        // crypto::hash and crypto::secret_key have basicly same
        // structure. They both keep they key/hash as c-style char array
        // of fixed size. Thus we can just copy data from hash
        // to key
        copy(begin(hash_.data), end(hash_.data), secret_key.data);

        return true;
    }

    // explicit instantiations of get template function
    template bool parse_str_secret_key<crypto::secret_key>(const string& key_str, crypto::secret_key& secret_key);
    template bool parse_str_secret_key<crypto::public_key>(const string& key_str, crypto::public_key& secret_key);


    /**
     * Get transaction tx using given tx hash. Hash is represent as string here,
     * so before we can tap into the blockchain, we need to pare it into
     * crypto::hash object.
     */
    bool
    get_tx_pub_key_from_str_hash(Blockchain& core_storage, const string& hash_str, transaction& tx)
    {
        crypto::hash tx_hash;
        parse_hash256(hash_str, tx_hash);

        try
        {
            // get transaction with given hash
            tx = core_storage.get_db().get_tx(tx_hash);
        }
        catch (const TX_DNE& e)
        {
            cerr << e.what() << endl;
            return false;
        }

        return true;
    }

    /**
     * Parse monero address in a string form into
     * cryptonote::account_public_address object
     */
    bool
    parse_str_address(const string& address_str, account_public_address& address)
    {

        if (!get_account_address_from_str(address, false, address_str))
        {
            cerr << "Error getting address: " << address_str << endl;
            return false;
        }

        return true;
    }


    /**
     * Return string representation of monero address
     */
    string
    print_address(const account_public_address& address)
    {
        return get_account_address_as_str(false, address);
    }



    /**
     * Check if a character is a path seprator
     */
    inline bool
    is_separator(char c)
    {
        // default linux path separator
        const char separator = PATH_SEPARARTOR;

        return c == separator;
    }



    /**
     * Remove trailinig path separator.
     */
    string
    remove_trailing_path_separator(const string& in_path)
    {
        string new_string = in_path;
        if (!new_string.empty() && is_separator(new_string[new_string.size() - 1]))
            new_string.erase(new_string.size() - 1);
        return new_string;
    }

    bf::path
    remove_trailing_path_separator(const bf::path& in_path)
    {
        string path_str = in_path.native();
        return bf::path(remove_trailing_path_separator(path_str));
    }


    ostream&
    operator<< (ostream& os, const account_public_address& addr)
    {
        os << "<" << get_account_address_as_str(false, addr) << ">";
        return os;
    }


    ostream&
    operator<< (ostream& os, const xmreg::secret_key16& sec16)
    {
        os << "<" << epee::string_tools::pod_to_hex(sec16)  << ">";
        return os;
    }


    /**
     * Performs keecak hash on the private key provided
     *
     */
    void
    keccak_hash(secret_key& prv, crypto::hash& result)
    {
        keccak(reinterpret_cast<uint8_t *>(&prv), sizeof(secret_key),
              (uint8_t *) &result, sizeof(secret_key));
    }


    /**
     * Converts crypto::hash into crypto::secret_key or crypto::public_key
     *
     * Both hash and secret have the same internal structure, i.e.,
     * they are both struct containing char[32]. Thus
     * we can use reinterperter_cast to cast hash into
     * corresponding key.
     */
    template <typename T>
    T get_key_from_hash(crypto::hash& in_hash)
    {
        T* key;
        key = reinterpret_cast<T*>(&in_hash);
        return *key;
    }

    // explicit instantiations of get_key_from_hash function
    template secret_key get_key_from_hash<secret_key>(crypto::hash& in_hash);
    template public_key get_key_from_hash<public_key>(crypto::hash& in_hash);



}
