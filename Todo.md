# Things that still need to be done before the first beta release \(last updated 2025-01-24\)

Pull requests are absolutely welcome! Feel free to contribute if you're interested in doing any of these.

This list is subject to change.

## core
- [ ] Add Gettext \(or some other local i18n backend\)
	- In-game server chat should use connected player's locale, either user-supplied or determined from the client's language setting
- [ ] Add Getopt \(for command-line arguments\)
- [x] Add server console \(optionally using Readline\)
	- [ ] Add command history support when using Readline
- [x] Permissions system
	- Backend is in place, things actually have to use it now
- [x] Store `git describe` string into Autoconf `PACKAGE_VERSION` string
- [ ] Robust logging system
- [x] Add fallbacks for some non-standard C functions such as `strsep` and `getrandom` \(Most are now provided through gnulib\)
- [ ] Stop ignoring the return value of functions like `getrandom`, `fread`, and `recv`
- [ ] Actually check for needed functions from external libraries
- [ ] Add a test suite and/or test programs
	- [ ] KCP->TCP tunnel \(for use with netcat/socat\)

## dbgate
- [x] Implement storage of player data
	- Backend is in place, but most functionality has yet to be implemented
- [ ] Implement bans \(IP, `aid`, `uid`, username/email and email domain filters\)

## dispatch
- [ ] Multi-language config for region titles and "server is down" messages
- [ ] On version mismatch, present a URL to a compatible game client and/or a patching howto
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
- [ ] Add semi-optional support for gnutls or mbedtls for crypto functions instead of forcing OpenSSL
	- Either one of these must be used, not all of them nor none at all
- [ ] HTTP command parser
- [ ] Barebones "get player data" endpoint
- [ ] Barebones "set player data" endpoint
- [ ] Better support for Chinese clients
	- [x] Account binder \(ID verification\)
- [ ] Web tokens \(distinct from in-game session tokens\)
- [ ] Browser-based frontend UI
	- Work in progress \(about 5% complete\)
- [ ] Support for logging in with multiple devices
- [ ] Third-party authentication
	- planned: Google, Facebook/Meta, Twitter/X, Discord
- [ ] Allow alternative URIs for auth-related endpoints (for use without PHP proxy)

## gameserver
- [ ] Player data \(Traveler name, signature, namecard, avatar showcase, etc.\)
	- Work in progress \(about 40% complete\)
- [x] Player props \(adventure rank, world level, stanima, etc.\)
- [ ] Avatar props \(current and max HP, battle stats, etc.\)
	- Work in progress \(about 10% complete\)
- [x] Figure out why session timeout always triggers
- [x] Figure out why KCP update never triggers
- [ ] Proper scene handling
- [ ] Proper avatar storage and switching
- [ ] Team management
	- Work in progress \(about 60% complete\)
- [x] Save and load player's last position
- [ ] Barebones inventory
	- Work in progress \(about 20% complete\)
- [ ] Barebones scene tags and openstates
	- Openstate system is in place. Scene tags need more work on "Proper scene handling" first
- [ ] Barebones scene points \(teleport\)
- [ ] In-game chat command parser \("Ayaka bot"\)
	- albeit different, since Ayaka wasn't added until client version 2.0
- [ ] Use dispatch key to encrypt token exchange packets

## Other
- [ ] Auto-deploy PHP assets based on host enviornment
- [ ] Auto-deploy frontend \(possibly into root of PHP branch\)

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

# Things that would be nice to have, but aren't strictly required
- Client patch for auth RSA and \(for later client versions\) dispatch RSA
	- Android has priority, as PC version already has readily avalable dispatch RSA patches
	- Though for PC, a launcher like Weedwacker would be nice too
- Support more client versions
	- Configure-time proto selection
- Decentralized server core \(like GIO\)
	- Ability to directly interface with internal GIO cross-server protocol
- Set up for porting core server functionality to support sister game clients \(`hkrpg`/`zzz`/`bk3`\)
