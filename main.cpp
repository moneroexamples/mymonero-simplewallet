
#include "src/CmdLineOptions.h"
#include "src/tools.h"
#include "src/electrum-words.h"

#include <iostream>
#include <string>

using namespace std;

using xmreg::operator<<;

// without this it wont work. I'm not sure what it does.
// it has something to do with locking the blockchain and tx pool
// during certain operations to avoid deadlocks.
unsigned int epee::g_test_dbg_lock_sleep = 0;



int main(int ac, const char* av[]) {

    // get command line options
    xmreg::CmdLineOptions opts {ac, av};

    auto help_opt = opts.get_option<bool>("help");

    // if help was chosen, display help text and finish
    if (*help_opt)
    {
        return 0;
    }

    // default language for the mnemonic
    // representation of the private spend key
    string language {"English"};

    // get 13 word mnemonic seed from MyMonero
    auto mnemonic_opt = opts.get_option<string>("mnemonic");

    // get the program command line options, or
    // some default values for quick check
    string mnemonic_str = mnemonic_opt
                          ? *mnemonic_opt
                          : "slid otherwise jeers lurk swung tawny zodiac tusks twang cajun swagger peaches tawny";

    cout << "\n"
         << "Mnemonic seed    : " << mnemonic_str << endl;


    // change the MyMonero 13 word mnemonic seed
    // to its 16 byte hexadecimal version
    xmreg::secret_key16 hexadecimal_seed;

    // use modified words_to_bytes function.
    xmreg::ElectrumWords::words_to_bytes(mnemonic_str, hexadecimal_seed, language);

    cout << "\n"
         << "Hexadecimal seed : " << hexadecimal_seed << endl;


    // take the 16 byte hexadecimal_seed, and
    // and perform Keccak hash on it. It will
    // produce 32 byte hash.
    crypto::hash hash_of_seed;

    cn_fast_hash(hexadecimal_seed.data, sizeof(hexadecimal_seed.data), hash_of_seed);

    cout << "\n"
         << "Hash of seed     : " << hash_of_seed<< endl;


    // having the hashed seed, we can proceed
    // with generation of private and public spend keys.
    // the keccak hash of the seed is used as a seed
    // to generate the spend keys.
    crypto::public_key public_spend_key;
    crypto::secret_key private_spend_key;

    crypto::generate_keys(public_spend_key, private_spend_key,
                          xmreg::get_key_from_hash<crypto::secret_key>(hash_of_seed),
                          true);

    cout << "\n"
         << "Private spend key: " << private_spend_key << "\n"
         << "Public spend key : " << public_spend_key  << endl;

    // now we get private and public view keys.
    // to do this, we keccak hash the hash_of_seed again
    crypto::hash hash_of_hash;
    cn_fast_hash(hash_of_seed.data, sizeof(hash_of_seed.data), hash_of_hash);

    crypto::public_key public_view_key;
    crypto::secret_key private_view_key;


    crypto::generate_keys(public_view_key, private_view_key,
                          xmreg::get_key_from_hash<crypto::secret_key>(hash_of_hash),
                          true);

    cout << "\n"
         << "Private view key : " << private_view_key << "\n"
         << "Public view key  : " << public_view_key  << endl;



    // having all keys, we can get the corresponding monero address
    cryptonote::account_public_address address {public_spend_key, public_view_key};


    cout << "\n"
         << "Monero address   : " << address << endl;


    cout << "\nEnd of program." << endl;

    return 0;
}