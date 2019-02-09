HTMLCOIN 2.3 Core
=========

http://www.htmlcoin.com/

What is HTMLCOIN?
-------------

HTMLCOIN is a new blockchain based on Qtum which uses Bitcoin Core and integrates Ethereum based smart contracts. It implements an extensible design which is capable of adding more VMs, enabled primarily through the Account Abstraction Layer, which allows for an account based virtual machine to function on a UTXO based blockchain. 


Quickstart
----------
### Build on Ubuntu

    This is a quick start script for compiling HTMLCOIN on Ubuntu

    sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils git cmake libboost-all-dev
    sudo apt-get install software-properties-common
    sudo add-apt-repository ppa:bitcoin/bitcoin
    sudo apt-get update
    sudo apt-get install libdb4.8-dev libdb4.8++-dev

    # If you want to build the Qt GUI:
    sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler

    git clone https://github.com/HTMLCOIN/HTMLCOIN --recursive
    cd HTMLCOIN

    # Note autogen will prompt to install some more dependencies if needed
    ./autogen.sh
    ./configure --disable-shared --enable-static 
    make -j2

### Build on OSX

The commands in this guide should be executed in a Terminal application.
The built-in one is located in `/Applications/Utilities/Terminal.app`.

#### Preparation

Install the OS X command line tools:

`xcode-select --install`

When the popup appears, click `Install`.

Then install [Homebrew](https://brew.sh).

#### Dependencies

    brew install cmake automake berkeley-db4 libtool boost --c++11 --without-single --without-static miniupnpc openssl pkg-config protobuf qt5 libevent imagemagick --with-librsvg

NOTE: Building with Qt4 is still supported, however, could result in a broken UI. Building with Qt5 is recommended.

#### Build HTMLCOIN Core

1. Clone the HTMLCOIN source code and cd into `HTMLCOIN`

        git clone --recursive https://github.com/HTMLCOIN/HTMLCOIN
        cd HTMLCOIN

2.  Build HTMLCOIN Core:

    Configure and build the HTMLCOIN binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.

        ./autogen.sh
        ./configure
        make

3.  It is recommended to build and run the unit tests:

        make check

### Run

Then you can either run the command-line daemon using `src/htmlcoind` and `src/htmlcoin-cli`, or you can run the Qt GUI using `src/qt/htmlcoin-qt`

For in-depth description of Sparknet and how to use HTMLCOIN for interacting with contracts, please see [sparknet-guide](doc/sparknet-guide.md).

License
-------

HTMLCOIN is GPLv3 licensed.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/HTMLCOIN/HTMLCOIN/tags) are created
regularly to indicate new official, stable release versions of HTMLCOIN.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).
