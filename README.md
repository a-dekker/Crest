# Crest

## About the application:

A ``top/ps`` like application for [SailfishOS](https://sailfishos.org/). Shows applications/processes on your mobile device sorted either by CPU, memory usage or name. You can also terminate/kill a selected process. Cover page shows average load, uptime and total swap/memory usage.

:warning: *Important notice*:

* Do **not** kill random stuff unless you know what you are doing, this can lead to lot of stability issues.
* Memory column shows RSS (Resident Set Size) which is not a real memory usage. It includes shared resources, so the number is typically bigger.
* Average load doesn't mean CPU utilization as it includes processes waiting for I/O as well. See [wikipedia](http://en.wikipedia.org/wiki/Load_(computing)).

## Binaries

Binaries are currently available and will be updated at [OpenRepos](https://openrepos.net/content/ade/crest-fork)

## About the origin

This application was initially written by [Michal Hrušecký](http://michal.hrusecky.net) and is distributed under GPL-3.0 license with hope that somebody will find it useful.
