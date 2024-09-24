# yagips - Yet another private server for a particular chinese gacha game...

This is a highly work-in-progress server emulator software written in C/C++. It targets client version OSREL1.3, but other game clients may be supported in the future.

Currently, getting in-game with this program is not possible yet by itself. However, the dispatch component is nearly feature-complete and can serve most game clients even if they are running other private server frameworks or are targeting other client versions.

A frontend for the sdkserver component as well as webstatic assets are available in the `php` branch.

**Pull requests are absolutely welcomed!** Please see Todo.md for what still needs to be done for this program to be actually functional.
