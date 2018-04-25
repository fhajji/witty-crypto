# Witty Crypto (WtCrypto)

Welcome to Witty Crypto.

This is a tool to learn cryptography over the web.
It is written in modern C++, using [OpenSSL](https://www.openssl.org/) for the
cryptographic bits, and [Witty](https://www.webtoolkit.eu/wt/) for the web-based GUI.

## Current status

Right now, Witty Crypto is in its **very early** development
stages. All there is at the moment is a web form where:

* a *symmetric cipher*, mode, and standard key length can be selected,
* plaintext and ciphertext can be shown / edited...
* ... both in textarea und in an editable hexdump view,
* and of course encrypting and decrypting.

### Future plans

I'm not promising anything, but...

additional forms may be added later, to showcase various
aspects of cryptography and OpenSSL (hashing, public key
crypto, ec/rsa, key exchange, bignums, ...).

I'm writing this tool with the aim to learn Witty. Choosing
OpenSSL as a "backend" is merely one way to fill this app
with some meaning. Nevertheless, it can prove useful for
students of basic cryptography.

## Requirements

* Libraries:
  * [Witty](https://github.com/emweb/wt/releases) 4.0.3+
    * Source [.tar.gz](https://github.com/emweb/wt/archive/4.0.3.tar.gz) [zip](https://github.com/emweb/wt/archive/4.0.3.zip)
    * Windows binaries for Visual Studio 2017: [x86](https://github.com/emweb/wt/releases/download/4.0.3/Wt-4.0.3-msvs2017-Windows-x86-SDK.zip), [x64](https://github.com/emweb/wt/releases/download/4.0.3/Wt-4.0.3-msvs2017-Windows-x64-SDK.zip)
  * [Boost](https://www.boost.org/) 1.66.0+
    * Source [.tar.gz](https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz)
  * [OpenSSL](https://www.openssl.org/) 1.0.2+
    * Source [.tar.gz](https://www.openssl.org/source/openssl-1.0.2o.tar.gz)

* Build System:
  * (Unix) [CMake](https://cmake.org/) 3.5.1+
    * Source [.tar.gz](https://cmake.org/files/v3.11/cmake-3.11.1.tar.gz)
	* Binary for Linux [.tar.gz](https://cmake.org/files/v3.11/cmake-3.11.1-Linux-x86_64.tar.gz)
	* Binary for Windows: [x86](https://cmake.org/files/v3.11/cmake-3.11.1-win32-x86.msi), [x64](https://cmake.org/files/v3.11/cmake-3.11.1-win64-x64.msi). Note that Visual Studio includes a reasonably recent CMake as well. You don't need CMake on Windows if using the provided VS solution and project files.
  * A C++14 capable/compatible compiler:
    * (Unix) [Clang](https://clang.llvm.org/) 3.8.0+
	* (Unix) [GCC](https://gcc.gnu.org/) 5.4.0+
	* (Windows) [Microsoft Visual Studio 2017](https://www.visualstudio.com/vs/) 15.6.6+

## Building

### Building on Unix (*BSD, Linux, ...)

First of all, get and install all the prerequisites above.
If your package manager has installed older versions of witty, boost,
cmake etc in /usr prefix, get the newest ones and install them into
prefix /usr/local. Make sure that /usr/local/bin precedes /usr/bin
in PATH.

Assuming that you have compiled Witty and installed it in /usr/local:

```
cd witty-crypto/WtCrypto    # where CMakeLists.txt is located
mkdir build
cd build
cmake ..
make
```

If you wish CMake to choose a specific compiler, set CXX and
CC environment variables accordingly:

```
env CXX=clang++ CC=clang cmake ..
```

or

```
env CXX=g++ CC=gcc cmake ..
```

### Building on Windows

1. First of all, you'll need to install Visual Studio 2017.
2. Then, install [vcpkg](https://docs.microsoft.com/en-us/cpp/vcpkg):
     * In "Developer Command Prompt for VS 2017", [fetched via git](https://github.com/Microsoft/vcpkg), and compiled with bootstrap-vcpkg.bat
	 * `vcpkg --vcpkg-root PATH-TO-VCPKG-FOLDER`
3. Add boost and openssl dependencies:
     * `vcpkg search boost`
     * `vcpkg install boost:x64-windows`
	 * `vcpkg install boost:x86-windows`
     * `vcpkg integrate install`
	 * repeat for openssl

Thanks to the magic of vcpkg, installed dependencies will be automatically
found by Visual Studio: there is no need to add include or library folders
to your VS projects for them.

Should Witty-4.0.3+ have been added to vcpkg's "ports directory",
you could add it in the same way as boost and openssl. Meanwhile, you'll
need to do it manually:

Assuming that you've compiled Witty yourself, or (easier), that
you've unpacked the pre-compiled windows binaries somewhere,
just open the solution "WtCrypto.sln" in Visual Studio 2017.

You may wish to [follow the instructions](https://redmine.webtoolkit.eu/projects/wt/wiki/Installing_Wt_on_MS_Windows) on setting various
paths before compiling / debugging.

## Running the executable

The executable ("wtcrypto.wt" on Unix) needs access to both
our custom "WtCrypto.css" and to Witty's "resources" directory.

On Unix, try this, assuming you're still in the "build" directory:

```
ln -s /usr/local/share/Wt/resources .
cp ../WtCrypto.css .
./wtcrypto.wt --docroot="." --http-address="0.0.0.0" --http-port="8080"
```

Use `[0::0]` instead of `0.0.0.0` to listen to all IPv6 interfaces.
Then, point a browser to the machine running wtcrypto.wt at port 8080.

Running on Windows is similar, with the additional twist that you
need to add the folder containing Witty's DLLs to PATH [as per the instructions](https://redmine.webtoolkit.eu/projects/wt/wiki/Installing_Wt_on_MS_Windows#Running-the-Examples).

## Copyright

Witty Crypto is Copyright (C) 2018 Farid Hajji. It is released under
the ISC License. Please refer to the file LICENSE.
