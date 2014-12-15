Encrypted File Container
========================

EFC is a simple system designed for encrypting individual files. Files are encrypted and wrapped in a header that stores the cipher details and original filename.

**Features:**

- Supports both password-based keys (hashes the password string) and keyfiles
- Supports optional compression using zlib

**Currently supported encryption schemes:**

- AES 256-bit CFB Mode

**Usage:**

Three binaries are included:

- `efcencode` - encrypts files
- `efcdecode` - decrypts files
- `efcinfo` - displays header information about an encrypted file


Build dependencies
------------------

Building from source requires the following libraries:

- [Crypto++](http://www.cryptopp.com/)
- libsimple-base (from the [assorted-utils](https://github.com/adamrehn/assorted-utils) repo)
- [Zlib](http://www.zlib.net/)
