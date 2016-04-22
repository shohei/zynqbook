import java.io.*;

public class Test {
	public static void main(String[] args) {
		int addr;

		System.out.println("Hello World");
		if ( args.length == 0 ) {
			System.out.println("Usage:Test file [r]");
			System.exit(0);
		}
		addr = logicBRICKS.map(0x31fa4000, 2048 * 4 * 1080 * 2);
		System.out.println("addr:" + addr);

		FileInputStream is;
		byte[] b;
		b = new byte[320 * 4 * 240];
		try {
			is = new FileInputStream(args[0]);
			is.read(b);
			is.close();
		} catch ( Exception ex ) {
			ex.printStackTrace();
			System.exit(1);
		}

		System.out.println("2addr:" + addr);
		logicBRICKS.load_image(addr, 320, 240, 2048 * 4, 4, b);

		logicBRICKS.copy_image(1, 0, 0, 320, 240, 2, 320, 240);
		//logicBRICKS.copy_image(2, 0, 0, 320, 240, 2, 320, 240, 0);
		//logicBRICKS.copy_image(2, 0, 0, 320, 240, 2, 320, 240, logicBRICKS.XOR);
		//logicBRICKS.copy_image(2, 0, 0, 320, 240, 2, 320, 240, logicBRICKS.XOR);
		logicBRICKS.box_fill(2, 0, 0, 1024, 768, 0xff000000);
		logicBRICKS.set_buffer(2, logicBRICKS.READ_BUFFER, 0);
		logicBRICKS.set_buffer(2, logicBRICKS.WRITE_BUFFER, 1);
		logicBRICKS.box_fill(2, 0, 0, 1024, 768, 0xff000000);
		try {
			Thread.sleep(10);
			//for( int r = 143 ; r <= 360; r += 180) {
			for( int r = 320 ; r <= 360 * 4; r += 360) {
				/*
				if ( r == 143 ) {
					continue;
				}
				if ( r == (143 + 180)) {
					continue;
				}
				*/
				//System.out.println("rotate:" + r);
				//logicBRICKS.copy_image(2, 0, 0, 320, 320, 2, 320, 240, 0);
				//logicBRICKS.box_fill(2, 280, 240, 320 + 120, 320 + 120, 0xff000000 + (0xff & (r >> 1)));
				logicBRICKS.box_fill(2, 280, 240, 320 + 120, 320 + 120, 0xff000000);
				if ( args.length == 2 ) {
					logicBRICKS.rotate_copy_image(2, 0, 0, 320, 240, 2, 320, 320, Integer.parseInt(args[1]));
					break;
				} 
				//Thread.sleep(10);
				while(logicBRICKS.is_busy_bitblt());
				//logicBRICKS.rotate_copy_image(1, 0, 0, 320, 240, 2, 320, 320, r);
				logicBRICKS.rotate_copy_image(1, 0, 0, 480 * 2, 270 * 2, 2, 0, 0, r);
				//Thread.sleep(100);
				while(logicBRICKS.is_busy_bmp());
				logicBRICKS.switch_buffer(2);
				//Thread.sleep(30);
			}
		} catch (java.lang.InterruptedException ex) {
			ex.printStackTrace();
		}
	}
}
