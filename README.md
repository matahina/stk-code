<!--- [![Linux build status](https://github.com/kimden/stk-code/actions/workflows/linux.yml/badge.svg)](https://github.com/kimden/stk-code/actions/workflows/linux.yml) --->
<!--- [![Apple build status](https://github.com/kimden/stk-code/actions/workflows/apple.yml/badge.svg)](https://github.com/kimden/stk-code/actions/workflows/apple.yml) --->
<!--- [![Windows build status](https://github.com/kimden/stk-code/actions/workflows/windows.yml/badge.svg)](https://github.com/kimden/stk-code/actions/workflows/windows.yml) --->
<!--- [![Switch build status](https://github.com/kimden/stk-code/actions/workflows/switch.yml/badge.svg)](https://github.com/kimden/stk-code/actions/workflows/switch.yml) --->
This repository contains a **modified** version of SuperTuxKart (STK), mainly intended for server-side usage, but you can also use it as a client.

Most important changes are listed [here](/FORK_CHANGES.md). The official version of STK can be found [here](https://github.com/supertuxkart/stk-code/), and the changes between it and latest commits of this repo can be found [here](https://github.com/supertuxkart/stk-code/compare/master...matahina:stk-code:freethewhale-1x).


---

## Note
This fork is a continuation of kimden's one, as kimden has [left](<https://youtu.be/a1kPc_G2QVw>) the game.

It is mostly a playground for me, an opportunity to dive deeper into C++ through a large codebase, starting with a bit of vibe-coding, with the goal of progressively becoming more and more autonomous when working on the codebase, its architecture, and its APIs.

---

## Branches

* [freethewhale-1x](https://github.com/matahina/stk-code/): contains the latest stable version. Use this branch if you want to deploy your own server. It is currently used on [freethewhale](https://freethewhale.ovh) Team GP servers.

* [freethewhale-1x-staging](https://github.com/matahina/stk-code/tree/freethewhale-1x-staging): contains ongoing changes (new features or few refactoring attempts). It may occasionally be unstable. Once changes are considered stable enough, they are merged into default `freethewhale-1x` branch.

* [freethewhale-1x-unstable](https://github.com/matahina/stk-code/tree/freethewhale-1x-unstable): contains large or invasive changes, including integration of changes from the official STK codebase and major refactoring. It is expected to be unstable and is not recommended for production use. Once changes are considered safe enough, they are merged into either `freethewhale-1x-staging` or `freethewhale-1x` branch.

* [anonymousecoder-1x-speed](https://github.com/matahina/stk-code/tree/anonymousecoder-1x-speed): an old fork from `anonymouse_coder`, few years ago, mainly tweaking game speed and physics.

* [freethewhale-2x](https://github.com/matahina/stk-code/tree/freethewhale-2x): ongoing work toward the future 2.x stable branch.

* [freethewhale-2x-staging](https://github.com/matahina/stk-code/tree/freethewhale-2x-staging): contains ongoing low-risk changes and refactoring for the future 2.x version. It may occasionally be unstable. Once changes are considered stable enough, they are merged into `freethewhale-2x` branch.

* [freethewhale-2x-unstable](https://github.com/matahina/stk-code/tree/freethewhale-2x-unstable): contains large or invasive changes, including integration of changes from the official STK 2.x branch. It is expected to be unstable and is not recommended for production use. Once changes are considered safe enough, they are merged into either `freethewhale-2x-staging` or `freethewhale-2x` branch.

* [freethewhale-2xTME-staging](https://github.com/matahina/stk-code/tree/freethewhale-2xTME-staging): contains proposed changes to [Tyre Mod Edition](https://github.com/Nomagno/stk-code/tree/tyre2X) and may be unstable.

## Mirrored branches

There are also branches regularly synchronized with other STK codebases.

* [nomagno-2xTME](https://github.com/matahina/stk-code/tree/nomagno-2xTME): mirror of [Tyre Mod Edition](https://github.com/Nomagno/stk-code/tree/tyre2X) by [Nomagno](https://github.com/Nomagno/).

* [official-1x](https://github.com/matahina/stk-code/tree/official-1x): mirror of the [official SuperTuxKart codebase](https://github.com/supertuxkart/stk-code/).

* [official-2x](https://github.com/matahina/stk-code/tree/official-2x): mirror of the [official SuperTuxKart 2.x development branch](https://github.com/supertuxkart/stk-code/tree/BalanceSTK2).

---

The software is released under the GNU General Public License (GPL) which can be found in the file [`COPYING`](/COPYING) in the same directory as this file.

Building instructions can be found in [`INSTALL.md`](/INSTALL.md).

Info on experimental anti-troll system can be found in [`ANTI_TROLL.md`](/ANTI_TROLL.md).

---

## About SuperTuxKart (Official version!)

[![#supertuxkart on the libera IRC network](https://img.shields.io/badge/libera-%23supertuxkart-brightgreen.svg)](https://web.libera.chat/?channels=#supertuxkart)

SuperTuxKart is a free kart racing game. It focuses on fun and not on realistic kart physics. Instructions can be found on the in-game help page.

The SuperTuxKart homepage can be found at <https://supertuxkart.net/>. There is also the [FAQ](https://supertuxkart.net/FAQ) and information on how get in touch with the [community](https://supertuxkart.net/Community).

Latest release binaries can be found [here](https://github.com/supertuxkart/stk-code/releases/latest), and preview release [here](https://github.com/supertuxkart/stk-code/releases/preview).
