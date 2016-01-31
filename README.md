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

## Example 1



## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.

Some Monero are also welcome:
```
48daf1rG3hE1Txapcsxh6WXNe9MLNKtu7W7tKTivtSoVLHErYzvdcpea2nSTgGkz66RFP4GKVAsTV14v6G3oddBTHfxP6tU
```    
