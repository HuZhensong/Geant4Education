# Geant4 Installation 安装

This document is an index on how to install Geant4 and related softwares in a new Mac or Linux. You can both scan [geant4 official guide](https://geant4.web.cern.ch/support/getting_started) and this document before any operations. Note, use international operation system instead of localized versions because I met problem when using Ubuntu Kylin.  If you owns a Windows computer, I suggest you install a virtual machine at first, then run Geant4 in it. 

这个文档是一个索引，关于如何在Mac和Linux上安装Geant4，以及Geant4所需的其它软件. 您可以在动手前先把本文档和[geant4官方手册](https://geant4.web.cern.ch/support/getting_started)都浏览一遍。请使用国际版的操作系统版本，不要使用本地化版本，比如我无法在Ubuntu-Kylin麒麟系统上正常使用Geant4。如果你的电脑是Windows，我建议你首先安装虚拟机，然后在它上面跑Geant4。

## Install cmake  
## 安装 cmake

Guide 教程: https://agenda.infn.it/getFile.py/access?contribId=12&resId=0&materialId=slides&confId=7698


## For Mac, install Xcode
## 仅对于Mac, 安装 Xcode

Find Xcode in your App Store, make sure there's enough space in your disk, then lie on your bed and have a nice dream. It takes more than an hour for the installation to be done, actually it took me an afternoon and a night due to poor internet connection. 

在苹果商店里找到Xcode，确认你的硬盘空间足够，然后回家睡一觉。安装时间在1小时以上，实际上我装了一个下午+晚上。

## If you need a visualization, install QT5 
## 如果你需要可视化，安装QT5

If you need to see how the physical process works, which I strongly recommend since you can check whether it's correct, you need a visualization. There are several visualization tools in fact, but according to official document, QT is the most general one, that was why I chose it. QT4 is no longer supported, you can only download QT5 from official website. You'd better try QT5 first. But I'll show how to install QT4 later. 

如果你需要看到物理过程的运行情况，你需要安装可视化工具。我建议安装，因为这样你可以确认物理过程是否正确。Geant4有很多的可视化工具，但是官方文档显示QT是通用的，所以我选择QT。QT4已经停止更新，所以我建议大家首先安装QT5，但是我还是把QT4安装方法写在下面。

QT official download site QT官网：https://www.qt.io/download?hsCtaTracking=f24f249b-61fb-4dec-9869-50512342f8d9%7Cf3adf380-4740-4f7e-9e49-d06fa99445fa

However, you might see the following error when installing Geant4, even if you already installed QT5. 

但是即便你安装了QT5，你可能会看到以下错误。

```
Found unsuitable Qt version "5.12.6" from /usr/bin/qmake, this code requires Qt 4.x
```

See solution 解决方法: https://geant4-forum.web.cern.ch/t/geant4-10-6-and-qt/1472

## If you still want to install QT4
## 如果还是要装QT4

See 请见： https://github.com/cartr/homebrew-qt4

It needs package brew, here's the installation guide for brew. 
它需要brew包，这是brew的安装教程

https://stackoverflow.com/questions/20381128/how-to-install-homebrew-on-os-x

## For Mac, XQUARTZ
## 对于MAC，安装XQUARTZ

XQUARTZ is included in old mac , but not in newer one (at least High Seirra and later versions).

XQUARTZ 在老的mac里面有，新的（至少High Seirra以及之后的）里没有。

Official Site 官网： https://www.xquartz.org

## NOW, It's time for downloading Geant4!
## 是时候装Geant4了！

See 请见：https://blog.whatsroot.xyz/2014/10/07/geant4-install/

This website is in Chinese. For non-Chinese speaking people, I think you can just see the code, it's enough. If you cannot open the website, you can open the web archive 'linux下安装Geant4.10全过程' in this repository.

如果网站无法打开，你可以看此目录下‘linux下安装Geant4.10全过程’网页缓存。

Also, here, official document for reference
同时可参阅官网

http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/InstallationGuide/html/


## Things are done, ENJOY!
## 解放了，好好玩。



