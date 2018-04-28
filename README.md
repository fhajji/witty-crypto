# Witty Crypto (WtCrypto)

Welcome to Witty Crypto.

This is a tool to learn cryptography over the web.
It is written in modern C++, using [OpenSSL](https://www.openssl.org/) for the
cryptographic bits, and [Witty](https://www.webtoolkit.eu/wt/) (Wt) for the
web-based GUI.

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
  * [Boost](https://www.boost.org/) 1.66.0+
  * [OpenSSL](https://www.openssl.org/) 1.0.2o+

* Build System:
  * [CMake](https://cmake.org/) 3.5.1+
  * A C++14 capable/compatible compiler:
    * (Unix) [Clang](https://clang.llvm.org/) 3.8.0+
	* (Unix) [GCC](https://gcc.gnu.org/) 5.4.0+
	* (Windows) [Microsoft Visual Studio 2017](https://www.visualstudio.com/vs/) 15.6.6+ and [vcpkg](https://github.com/Microsoft/vcpkg).

## Building

### Building on Unix (*BSD, Linux, ...)

First of all, get and install all the prerequisites above.
If your package manager has installed older versions of witty, boost,
cmake etc in /usr prefix, get the newest ones as source, compile and
install them into prefix /usr/local. Make sure that /usr/local/bin precedes
/usr/bin in PATH.

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

1. First of all, install [Visual Studio 2017](https://www.visualstudio.com/vs/).
2. Then, install [vcpkg](https://docs.microsoft.com/en-us/cpp/vcpkg):
     * Open "Developer Command Prompt for VS 2017"
	 * `cd ROOTDIR_OF_VCPKG` # e.g. \Users\YOU, will install \Users\YOU\vcpkg
	 * `git clone https://github.com/Microsoft/vcpkg.git`
	 * `cd vcpkg`
	 * `.\bootstrap-vcpkg.bat`
	 * `vcpkg integrate install`
	 * `vcpkg integrate powershell`
3. Fetch and compile wt and dependencies :
     * `vcpkg install boost:x86-windows`
	 * `vcpkg install boost:x64-windows`
	 * `vcpkg install wt:x86-windows`
	 * `vcpkg install wt:x64-windows`

vcpkg will fetch, compile, and install boost, wt, and
all of their dependencies (including openssl) in both
32-bit (x86-windows) and 64-bit (x64-windows) debug and
release architectures.
	 
Thanks to the magic of vcpkg, installed packages will be automatically
found by Visual Studio: there is no need to add include or library folders
to your VS projects for them.

4. You can then use the supplied solution "WtCrypto.sln". Or, try your
luck with CMake (untested, need to edit WtCrypto\CMakeLists.txt
accordingly, and [follow the instructions](https://github.com/Microsoft/vcpkg)
w.r.t the build chain).

5. If you prefer the manual approach, you may wish to [follow the instructions](https://redmine.webtoolkit.eu/projects/wt/wiki/Installing_Wt_on_MS_Windows)
on using the pre-compiled libraries, on setting various paths before compiling / debugging etc.

## Running the executable

The executable ("wtcrypto.wt" on Unix) needs access to both
our custom "WtCrypto.css" and to Witty's "resources" directory.

On Unix, try this, assuming you're still in the "build" directory:

```
ln -s /usr/local/share/Wt/resources .
cp ../WtCrypto.css .
./wtcrypto.wt --docroot="." --http-address="0.0.0.0" --http-port="8080"
```

Use `0::0` instead of `0.0.0.0` to listen to all IPv6 interfaces.
Then, point a browser to the machine running wtcrypto.wt at port 8080.

Running on Windows is similar, with the additional twist that you
need to add the folder containing Witty's DLLs to PATH [as per the instructions](https://redmine.webtoolkit.eu/projects/wt/wiki/Installing_Wt_on_MS_Windows#Running-the-Examples). If you've used vcpkg to build and install wt, a copy of the
"resources" directory is:

```
ROOTDIR_OF_VCPKG\vcpkg\packages\wt_x64-windows\share\Wt\resources
```

Good luck.

## Copyright

Witty Crypto is Copyright (C) 2018 Farid Hajji. It is released under
the ISC License. Please refer to the file LICENSE.
