# Things that still need to be done before the first beta release \(last updated 2014-09-24\)

Pull requests are absolutely welcome! Feel free to contribute if you're interested in doing any of these.

This list is subject to change.

## core
- [ ] Add Gettext \(or some other local i18n backend\)
	- In-game server chat should use connected player's locale, either user-supplied or determined from the client's language setting
- [ ] Add Getopt \(for command-line arguments\)
- [ ] Add server console \(optionally using Readline\)
- [ ] Permissions system
- [x] Store `git describe` string into Autoconf `PACKAGE_VERSION` string
- [ ] Robust logging system
- [x] Add fallbacks for some non-standard C functions such as `strsep` and `getrandom` \(Most are now provided through gnulib\)
- [ ] Stop ignoring the return value of functions like `getrandom`, `fread`, and `recv`
- [ ] Actually check for needed functions from external libraries
- [ ] Add a test suite and/or test programs
	- [ ] KCP->TCP tunnel \(for use with netcat/socat\)

## dbgate
- [ ] Implement storage of player data \([LevelDB](https://github.com/google/leveldb) is currently the most likely candidate backend for this\). (In progress)
- [ ] ~~Cache session seed in SQLite `players` table along with a timestamp~~ (SQLite is being removed soon in favor of exclusively using LevelDB)
- [ ] ~~Remove `token` from SQLite `accounts` table~~
- [ ] ~~Versioned SQLite tables?~~
- [ ] Implement bans \(IP, `aid`, `uid`, username/email and email domain filters\)

## dispatch
- [ ] Multi-language config for region titles and "server is down" messages
- [ ] Split HTTP and account stuff into `sdk.cpp`

## sdk
- [ ] Add password verification
	- Hashes are to	be stored as HMAC-SHA256 with config-supplied HMAC key
- [ ] Interface with other sdk/dispatch server implmentations
	- [ ] Grasscutter: Websocket
	- [ ] GIO
		- [ ] GIO -> yagips: needs a patched sdkserver or disabled verification on yagips side
		- [ ] yagips -> GIO: either manual parsing \(through picohttpparser\) or direct HTTP request via libcurl
	- [ ] Ourself: TCP/Protobuf, possibly with TLS, fall back to HTTP
- [ ] Add optional TLS support
- [ ] Add semi-optional support for gnutls for crypto functions instead of forcing OpenSSL
	- Either gnutls or OpenSSL must be used, not both nor neither
- [ ] HTTP command parser
- [ ] Barebones "get player data" endpoint
- [ ] Barebones "set player data" endpoint
- [ ] Better support for Chinese clients
- [ ] Web tokens \(distinct from in-game session tokens\)
- [ ] Browser-based frontend UI

## gameserver
- [ ] Player data \(Traveler name, signature, namecard, avatar showcase, etc.\)
- [ ] Player props \(adventure rank, world level, stanima, etc.\)
- [ ] Avatar props \(current and max HP, battle stats, etc.\)
- [x] Figure out why session timeout always triggers
- [x] Figure out why KCP update never triggers
- [ ] Proper scene handling
- [ ] Proper avatar storage and switching
- [ ] Team management
- [ ] Save and load player's last position
- [ ] Barebones inventory
- [ ] Barebones scene tags and openstates
- [ ] Barebones scene points \(teleport\)
- [ ] In-game chat command parser \("Ayaka bot"\)
	- albeit different, since Ayaka wasn't added until client version 2.0
- [ ] Use dispatch key to encrypt token exchange packets

# Things that still need to be done before the first full release
- [ ] Gacha
- [ ] Combat
- [ ] Co-op
- [ ] Friends
- [ ] Quests
	- Not the quests themselves, but just the ability to accept and manually progress them
- [ ] Domains
- [ ] Drops
- [ ] Trial avatars and test runs
- [ ] Simple events
- [ ] Complete inventory
- [ ] Support for cmake in addition to autoconf
