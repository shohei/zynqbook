#include "HelloWorld.h"

JNIEXPORT void JNICALL Java_HelloWorld_printMessage (JNIEnv *env, jobject obj)
{
	printf("Hello World from JNI.\n");
}
