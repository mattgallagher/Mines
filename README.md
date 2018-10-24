# Mines

An implementation of the classic Minesweeper game. Originally written by me in 1999 and updated for Xcode 8 and macOS Sierra in 2017. For a full (but light-hearted) article on the topic, see [Compiling a Mac OS 8 application on macOS Sierra](https://www.cocoawithlove.com/blog/porting-from-macos8-to-sierra.html).

> NOTE: this project in this article no longer compiles in Xcode 10/macOS 10.14 or newer due to i386 deprecation.

Building this project requires a copy of the Mac OS X 10.6 SDK. The easiest way to get this is to visit the [Apple Developer "More Downloads" page](https://developer.apple.com/download/more/) and once you've logged in [download Xcode 4.3.3 for Lion from Apple](https://download.developer.apple.com/Developer_Tools/xcode_4.3.3_for_lion/xcode_4.3.3_for_lion.dmg) (this second link will likely fail unless you've already logged in).

You can't *run* Xcode 4.3.3 on macOS Sierra but you can open the bundle and copy the "Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk" folder and place it at the same location in your Xcode 8's bundle. To force Xcode to include the 10.6 SDK, you must also edit the "MinimumSDKVersion" in the "Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Info.plist" file in your Xcode bundle to "10.6" or lower.

**License note**: my code is available under my ISC-style license (see LICENSE.txt) but this repository also contains code copyright Freescale under a BSD-style license (see LICENSE.txt in the PowerPlant directory).