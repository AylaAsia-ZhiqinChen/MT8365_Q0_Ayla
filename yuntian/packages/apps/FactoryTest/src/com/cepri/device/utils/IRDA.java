package cepri.device.utils;

public class IRDA {
	public static native int Init();
	public static native int DeInit();
	public static native int ClearSendCache();
	public static native int ClearRecvCache();
	public static native int Config(int baudrate,int databits,int parity, int stopbits,int blockmode);
	public static native int SendData(byte[] buf, int offset, int count);
	public static native int RecvData(byte[] buf, int offset, int count);
	public static native int SetTimeOut(int direction, int timeout);
	static{
        //加载打包完毕的 so类库
        System.loadLibrary("infrared");
    }
}
