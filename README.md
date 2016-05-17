# lproxy-local-android
*目前是试验性质的项目，仅支持部分 Android 机型*

lproxy-local-android 借鉴了 [lproxy](https://github.com/DD-L/lproxy) 的另一个子项目 [MySecrect](https://github.com/DD-L/MySecrect)


----------------------

构建环境：

Android for armeabi-v7a(GCC 4.9, Qt 5.5.1)

* 编译器： Android GCC (arm-4.9)
* Qt 版本： Qt5.5.1 for Android armv7
* jdk 1.8.0_71
* Target: Android 6.0 - API Level 23
* CPU/ABI: ARM(armeabi-v7a)

开发时用的测试机型：

* mi4 Android 6.0.1
* 华为荣耀7 Android 5.0.2


目前本人对 Android 了解甚少，所以当前 `lproxy-local-android` 的功能还很不完善，没有“全局代理模式”，更没有“分应用代理模式”，目前只能像在 PC 端那样使用：绑定一个本地 socks5 端口。

使用时，可能还需要借助 [`ProxyDroid`](https://github.com/madeye/proxydroid) 等类似的辅助软件；

或直接使用 Android 版的 火狐（FireFox） 浏览器，火狐 Android 移动版浏览器设置 Socks5 代理的方法： [SetupSocks5ProxyFirefoxAndroid](https://github.com/DD-L/DailyNotes/blob/master/lproxy/lproxy-local-android/SetupSocks5ProxyFirefoxAndroid.md)
