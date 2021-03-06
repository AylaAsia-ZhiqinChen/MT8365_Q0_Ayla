// This source code is generated by UdpGeneratorTool, not recommend to modify it directly
package com.mediatek.location.lppe.bluetooth;

import com.mediatek.socket.base.SocketUtils.BaseBuffer;
import com.mediatek.socket.base.SocketUtils.Codable;

public enum BluetoothRTDUnit implements Codable {
    microseconds,
    hundredsOfNanoseconds,
    tensOfNanoseconds,
    nanoseconds,
    tenthsOfNanoseconds,
    end,
;

    public final static BluetoothRTDUnit _instance = BluetoothRTDUnit.microseconds;

    @Override
    public void encode(BaseBuffer buff) {
        switch (this) {
        case microseconds:
            buff.putInt(0);
            break;
        case hundredsOfNanoseconds:
            buff.putInt(1);
            break;
        case tensOfNanoseconds:
            buff.putInt(2);
            break;
        case nanoseconds:
            buff.putInt(3);
            break;
        case tenthsOfNanoseconds:
            buff.putInt(4);
            break;
        case end:
            buff.putInt(2147483647);
            break;
        default:
            break;
        }
    }

    @Override
    public BluetoothRTDUnit decode(BaseBuffer buff) {
        int _type = buff.getInt();
        switch (_type) {
        case 0:
            return microseconds;
        case 1:
            return hundredsOfNanoseconds;
        case 2:
            return tensOfNanoseconds;
        case 3:
            return nanoseconds;
        case 4:
            return tenthsOfNanoseconds;
        case 2147483647:
            return end;
        }
        return null;
    }

    @Override
    public BluetoothRTDUnit[] getArray(Codable[] data) {
        BluetoothRTDUnit[] _out = new BluetoothRTDUnit[data.length];
        for (int _i = 0; _i < data.length; _i++) {
            _out[_i] = (BluetoothRTDUnit) data[_i];
        }
        return _out;
    }

}
