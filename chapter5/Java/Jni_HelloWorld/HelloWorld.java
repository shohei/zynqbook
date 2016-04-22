// HelloWorld.java
//
import java.io.*;
public class HelloWorld {
  public native void printMessage();
  void init() {
    System.loadLibrary("HelloWorld");
  }
  public static void main (String args[]) {
    HelloWorld hello = new HelloWorld();
    hello.init();
    hello.printMessage();
  }
}

