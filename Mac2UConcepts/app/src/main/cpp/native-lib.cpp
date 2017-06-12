#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_mac2corvallis_mac2uconcepts_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "";
    return env->NewStringUTF(hello.c_str());
}
