# Generate simplewallet based on MyMonero 13 word mnemonic seed

[MyMonero](https://mymonero.com/) uses 13 word mnemonic seed, which
cant be used in the Monero's `simplewallet`. The reason is, that the `simplewallet`
uses 25 word mnemonic seed, and also, generation of private view and spend keys
by the `simplewallet` is different than that used by [MyMonero](https://mymonero.com/).

Thus, at the moment you cant use your MyMonero account with `simplewallet`. This is
a problem, because what if MyMonero is offline? Or what if it is going to be shutdown?

In this example, an attempt is made to generate a wallet file, based on
MyMonero 13 mnemonic seed that can be used by `simplewallet`.

More information on the differences between the `simplewallet` and MyMonero,
can be found:

  - [Cryptnote Tests](http://xmrtests.llcoins.net/addresstests.html)
  - [Why MyMonero key derivation is different than for the simplewallet (Mnemonic)](https://www.reddit.com/r/Monero/comments/3s80l2/why_mymonero_key_derivation_is_different_than_for/)

The example was prepared and tested on Xubuntu 15.10 x64 and Monero 0.9.

Instruction for Monero 0.9 compilation and setup of Monero's header files and libraries are at:
 - [Compile Monero 0.9 on Ubuntu 15.10 and 14.04 x64](https://github.com/moneroexamples/compile-monero-09-on-ubuntu)

English is assumed to be seed's language. Thus, if your seed is other language
then English, the example probably will not work. I don't know. Havenit checked
any other languages except English.

## C++: main.cpp

```c++
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
```

## Program options

```bash
./mymonerowallet -h
mymonero-keys, generate simplewallet keys file based on MyMonero's 13 world mnemonic seed:
  -h [ --help ] [=arg(=1)] (=0) produce help message
  -m [ --mnemonic ] arg         13 word mnemonic seed from MyMonero
  -w [ --wallet-file ] arg      output wallet file, e.g. mmwallet.bin
  -p [ --password ] arg         wallet password
```

## Example 1

Execute `mymonerowallet` with default options

```bash
./mymonerowallet
```

Output:

```bash
Mnemonic seed    : slid otherwise jeers lurk swung tawny zodiac tusks twang cajun swagger peaches tawny

Hexadecimal seed : <5878efd0a6d45b0374b49c000da07cd2>

Hash of seed     : <6d23fe14606a0d5fe62d05a78c4b5b1cae2f38f9330e42e86a50286db16ad61e>

Private spend key: <804f08b84507fb0610910d04ae517c07ae2f38f9330e42e86a50286db16ad60e>
Public spend key : <b54264728412c71bb62caca5b3cb57eb96a48a580c97a65257290243e3adf401>

Private view key : <dbb024c981cf7dc797593713f5f426c3b1cad50542c0814d4786e12e768be504>
Public view key  : <75fc0c90732a3db632bc0a169328067fa3f4c52e80ae067aa60bae8c4ccd8711>

Monero address   : <48VWHdLTEpE5dqa5VpAy5xgQWBHZARiGmEmojNLqD4ef1FAzkPxCe9JXUYwCShRR5XNMGnyusrnkmMWr2HMdfDRx2vsrG7c>

Keys file "/home/mwo/mmwallet.bin.keys" created.

Start simplewallet using:
/opt/bitmonero/simplewallet --wallet-file /home/marcin/mmwallet.bin.keys

Passord given is: "password"

Use 'refresh' command in the simplewallet to scan the blockchain for your transactions.
```

The comparison of MyMonero website with the wallet generated is shown on the following screenshot:

![After](https://raw.githubusercontent.com/moneroexamples/mymonero-simplewallet/master/img/example_01.jpg)

As can be seen on the screenshot, address, private view and spend keys, as well as balance of the
`simplewallet` wallet generated agree with that of MyMonero website.

Obviously, we can use `simplewallet` now to transfer founds normally available only
through MyMonero, as demonstrated on the following screenshot:

![After](https://raw.githubusercontent.com/moneroexamples/mymonero-simplewallet/master/img/example_01a.jpg)

After sending out xmr from `simplewallet`, MyMonero after few minutes refreshes my account status
correctly showing that the founds are being send out.

## Example 2

Execute `mymonerowallet` with MyMonero seed

```bash
./mymonerowallet -m "drying venomous baffles unusual northern cobra mobile unnoticed hedgehog oncoming neon gyrate unusual" -w ~/mmwallet2.dat -p somepassword12
```

Output:

```bash
Mnemonic seed    : drying venomous baffles unusual northern cobra mobile unnoticed hedgehog oncoming neon gyrate unusual

Hexadecimal seed : <4fd72328c8cc9b9327cac47896eec0cb>

Hash of seed     : <ae114efe5eea0c72d95b8cb2f366d2b3d51a9a0edb6d5d91801b06f2cff68c70>

Private spend key: <33469573a6348c09fd11c73ddd91b921d51a9a0edb6d5d91801b06f2cff68c00>
Public spend key : <0ccebf6b53e9acb923da7a9f7c81633d49e562fb726fa480ead525967aa70088>

Private view key : <4baa54bdac1ec957982899b197c1810b279eccab4243d46b889c7c274570c606>
Public view key  : <b4fd6af8d1515e81bf32b4dd6bda99f6702473514d7e690dde7a5bb8fa312a42>

Monero address   : <427FyKsPfaBXy6FByZLujQBFaT9zU1JXdNZfFNA3i6bqPsEHajfcKRFNhi1ptUEyLLiDkxPq2KQwJ3KYjnhYseuo8W9d5in>

Keys file "/home/marcin/mmwallet2.dat.keys" created.

Start simplewallet using:
/opt/bitmonero/simplewallet --wallet-file /home/mwo/mmwallet2.dat
```

The comparison of MyMonero website with the wallet generated is shown on the following screenshot (only address and private keys shown):

![After](https://raw.githubusercontent.com/moneroexamples/mymonero-simplewallet/master/img/example_02.jpg)


## Compile this example
The dependencies are same as those for Monero, so I assume Monero compiles
correctly.

Monero C++ headers and libraries are set as shown here:
 - [Compile Monero 0.9 on Ubuntu 15.10 and 14.04 x64](https://github.com/moneroexamples/compile-monero-09-on-ubuntu)

If so then to download and compile this example, the following
steps can be executed:

```bash
# download the source code
git clone https://github.com/moneroexamples/mymonero-simplewallet.git

# enter the downloaded sourced code folder
cd mymonero-simplewallet

# create the makefile
cmake .

# compile
make
```

After this, `mymonerowallet` executable file should be present in the mymonero-simplewallet
folder.

## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.

Some Monero are also welcome:
```
48daf1rG3hE1Txapcsxh6WXNe9MLNKtu7W7tKTivtSoVLHErYzvdcpea2nSTgGkz66RFP4GKVAsTV14v6G3oddBTHfxP6tU
```    
