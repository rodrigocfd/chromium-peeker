# Chromium Peeker

A native C++11 Win32 utility to download a list of [Chromium builds](http://commondatastorage.googleapis.com/chromium-browser-continuous) for Windows, allowing info fetching and ZIP package download.

![Screenshot](screenshot-75.png)

## WinLamb libraries

This project uses [WinLamb](https://github.com/rodrigocfd/winlamb) and [Winlamb More](https://github.com/rodrigocfd/winlamb-more) libraries in [subtrees](http://bluedesk.blogspot.com.br/2017/06/trying-out-git-subtree.html). To add the subtrees, run:

```
git remote add winlamb --no-tags https://github.com/rodrigocfd/winlamb.git
git subtree add --prefix winlamb winlamb master --squash

git remote add winlamb-more --no-tags https://github.com/rodrigocfd/winlamb-more.git
git subtree add --prefix winlamb-more winlamb-more master --squash
```

Then, when you want to pull the code from the external library repos, run:

```
git subtree pull --prefix winlamb winlamb master --squash
git subtree pull --prefix winlamb-more winlamb-more master --squash
```
