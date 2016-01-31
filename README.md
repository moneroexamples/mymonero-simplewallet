# Generate simplewallet based on MyMonero 13 word mnemonic seed

[MyMonero](https://mymonero.com/) uses 13 word mnemonic seed, which
cant be used in the Monero's `simplewallet`. The reason is, that the `simplewallet`
uses 25 word mnemonic seed, and also, generation of private view and spend keys
by the `simplewallet` is different than that used by [MyMonero](https://mymonero.com/).

Thus, at the moment you cant use your MyMonero account with simplewallet. This is
a problem, because what if MyMonero is online? Or what if it is going to be shutdown?

In this example, an attempt is made to generate a wallet file, based on
MyMonero 13 mnemonic seed that can be used by `simplewallet`.

More information on the differences between the `simplewallet` and MyMonero,
can be found:

  - [Cryptnote Tests](http://xmrtests.llcoins.net/addresstests.html)
  - [Why MyMonero key derivation is different than for the simplewallet (Mnemonic)](https://www.reddit.com/r/Monero/comments/3s80l2/why_mymonero_key_derivation_is_different_than_for/)

The example was prepared and tested on Xubuntu 15.10 x64 and Monero 0.9.

Instruction for Monero 0.9 compilation and setup of Monero's header files and libraries are at:
 - [Compile Monero 0.9 on Ubuntu 15.10 and 14.04 x64](https://github.com/moneroexamples/compile-monero-09-on-ubuntu)


## C++: main.cpp

```c++
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

Output

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

## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.

Some Monero are also welcome:
```
48daf1rG3hE1Txapcsxh6WXNe9MLNKtu7W7tKTivtSoVLHErYzvdcpea2nSTgGkz66RFP4GKVAsTV14v6G3oddBTHfxP6tU
```    
