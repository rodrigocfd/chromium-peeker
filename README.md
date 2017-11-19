# Chromium Peeker

A native C++11 Win32 utility to download a list of [Chromium builds](http://commondatastorage.googleapis.com/chromium-browser-continuous) for Windows, allowing info fetching and ZIP package download.

![Screenshot](screenshot-75.png)

## WinLamb library

This project uses [WinLamb](https://github.com/rodrigocfd/winlamb) library in a [subtree](http://bluedesk.blogspot.com.br/2017/06/trying-out-git-subtree.html). To add the subtree, run:

```
git remote add winlamb --no-tags https://github.com/rodrigocfd/winlamb.git
git subtree add --prefix winlamb winlamb master --squash
```

If you want to update the library, pull the code from the repo:

```
git subtree pull --prefix winlamb winlamb master --squash
```
