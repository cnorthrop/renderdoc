#/bin/sh

if [ -z "${ANDROID_SDK}" ];
then echo "Please set ANDROID_SDK, exiting"; exit;
else echo "ANDROID_SDK is ${ANDROID_SDK}";
fi

if [ -z "$1" ];
then echo "CMAKE_CURRENT_BINARY_DIR not set"; exit;
else echo "CMAKE_CURRENT_BINARY_DIR is $1";
fi

if aapt package -f -m -S res -J src -M AndroidManifest.xml -I $ANDROID_SDK/platforms/android-23/android.jar;
then echo Created R.java;
else exit 1;
fi

if javac -d $1/obj -source 1.7 -target 1.7 -classpath $ANDROID_SDK/platforms/android-23/android.jar:obj -sourcepath src src/org/renderdoc/renderdoccmd/*.java;
then echo Compiled java;
else exit 1;
fi

if dx --dex --output=bin/classes.dex $1/obj;
then echo Created DEX file;
else exit 1;
fi

if aapt package -f -M AndroidManifest.xml -S res -I $ANDROID_SDK/platforms/android-23/android.jar -F bin/RenderDocCmd-unaligned.apk bin libs;
then echo Created APK;
else exit 1;
fi

if zipalign -f 4 bin/RenderDocCmd-unaligned.apk bin/RenderDocCmd.apk;
then echo Aligned APK;
else exit 1;
fi

if [ -f $1/debug.keystore ];
then echo Using existing debug.keystore;
elif keytool -genkey -keystore $1/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=, OU=, O=, L=, S=, C=";
then echo Generated debug.keystore;
else exit 1;
fi

if apksigner sign --ks $1/debug.keystore --ks-pass pass:android --key-pass pass:android --ks-key-alias androiddebugkey bin/RenderDocCmd.apk;
then echo Debug signed APK;
else exit 1;
fi
