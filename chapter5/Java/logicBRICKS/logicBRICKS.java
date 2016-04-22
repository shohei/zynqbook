public class logicBRICKS {
	static {
		System.loadLibrary("logicBRICKS");
		init();
	}

	private static native void init();

	public static native int map(int phy_addr, int size);
	public static native void load_image(int addr, int w, int h, int stride_bytes, int bytes_per_pixel, byte[] image);

	public static int MOVE = 0xC;
	public static int XOR = 6;

	public static native void copy_image(int src_layer_no, int src_x, int src_y, int w, int h, int dst_layer_no, int dst_x, int dst_y, int mode);

	public static void copy_image(int src_layer_no, int src_x, int src_y, int w, int h, int dst_layer_no, int dst_x, int dst_y) {
		copy_image(src_layer_no, src_x, src_y, w, h, dst_layer_no, dst_x, dst_y, MOVE);
	}

	public static native void box_fill(int layer_no, int x, int y, int w, int h, int color);

	public static native void rotate_copy_image(int src_layer_no, int src_x, int src_y, int w, int h, int dst_layer_no, int dst_x, int dst_y, int angle);

	public static native void enable_layer(int layer_no, boolean flag);

	public static int READ_BUFFER = 0;
	public static int WRITE_BUFFER = 1;

	public static native void set_buffer(int layer_no, int buffer_kind, int buffer_no);
	public static native void switch_buffer(int layer_no);

	public static native boolean is_busy_bitblt();
	public static native boolean is_busy_bmp();
}
