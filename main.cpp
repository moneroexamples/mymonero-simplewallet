
#include "src/CmdLineOptions.h"
#include "src/tools.h"
#include "src/electrum-words.h"
#include "src/simple_account.h"

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
    auto wallet_file_opt = opts.get_option<string>("wallet-file");
    auto password_opt = opts.get_option<string>("password");

    // get the program command line options, or
    // some default values for quick check
    string mnemonic_str = mnemonic_opt
                          ? *mnemonic_opt
                          : "slid otherwise jeers lurk swung tawny zodiac tusks twang cajun swagger peaches tawny";


    // simplewallet wallet file name, e.g., mmwallet.bin
    // actually we do not directy create this file. we
    // create a file *.keys containing the address and the private keys
    // the wallet file is going to be created by simplewallet after
    // refresh to preserve the state of the wallet.
    // we only need keys files to create the wallet.
    string wallet_file = wallet_file_opt
                         ? *wallet_file_opt
                         : xmreg::get_home_folder() + string("mmwallet.bin");

    // name of the keys files
    string keys_file_name = wallet_file + string(".keys");

    string password = password_opt ? *password_opt : "password";

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


    // Once we have all keys and address from the mnemonic seed
    // we can proceed to generating wallet *.keys file
    // that can be read by simplewallet


    // we start this by creating instance of simple_account class
    // and populate with the address, and private spend and view keys
    // obtained in the previous steps
    xmreg::simple_account account;

    account.create_from_keys(address, private_spend_key, private_view_key);

    std::string account_data;


    if (!epee::serialization::store_t_to_binary(account, account_data))
    {
        cerr << "Something went wrong with serializing simple_account instance" << endl;
        return 1;
    }

    rapidjson::Document json;
    json.SetObject();
    rapidjson::Value value(rapidjson::kStringType);


    value.SetString(account_data.c_str(), account_data.length());

    json.AddMember("key_data", value, json.GetAllocator());

    tools::wallet2::keys_file_data keys_file_data
            = boost::value_initialized<tools::wallet2::keys_file_data>();

    // Serialize the JSON object
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    account_data = buffer.GetString();

    // Encrypt the entire JSON object.
    crypto::chacha8_key key;
    crypto::generate_chacha8_key(password, key);
    std::string cipher;
    cipher.resize(account_data.size());
    keys_file_data.iv = crypto::rand<crypto::chacha8_iv>();
    crypto::chacha8(account_data.data(), account_data.size(),
                    key, keys_file_data.iv, &cipher[0]);
    keys_file_data.account_data = cipher;

    std::string buf;

    // serialize key file data
    if (!serialization::dump_binary(keys_file_data, buf))
    {
        cerr << "Something went wrong with serializing keys_file_data" << endl;
        return 1;
    }

    // save serialized keys into the wallet file
    if (!epee::file_io_utils::save_string_to_file(keys_file_name, buf))
    {
        cerr << "Something went wrong with writing file: " << keys_file_name << endl;
        return 1;
    }

    cout << "\nKeys file \"" << keys_file_name << "\" created." << endl;

    cout << "\nStart simplewallet using: \n"
         << "/opt/bitmonero/simplewallet --wallet-file " << wallet_file
         << endl;

    cout << "\nPassord given is: \"" << password <<"\"" << endl;

    cout << "\nUse 'refresh' command in the simplewallet "
                    "to scan the blockchain "
                    "for your transactions. "
         << endl;


    cout << "\nEnd of program." << endl;

    return 0;
}