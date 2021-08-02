package com.mediatek.op08.dialer;

import android.content.Context;
import com.mediatek.dialer.ext.ICallDetailsExtension;
import com.mediatek.dialer.ext.ICallLogExt;
import com.mediatek.dialer.ext.IDialPadExtension;
import com.mediatek.dialer.ext.IDialerSearchExtension;
import com.mediatek.dialer.calllog.presence.CallLogExt;
import com.mediatek.dialer.search.presence.DialerSearchExtension;
import com.mediatek.dialer.calldetails.presence.CallDetailsExtension;
import com.mediatek.dialer.ext.OpDialerCustomizationFactoryBase;

public class Op08DialerCustomizationFactory extends OpDialerCustomizationFactoryBase {
    private Context mContext;

    public Op08DialerCustomizationFactory(Context context) {
        mContext = context;
    }

    public ICallLogExt makeCallLogExt() {
        return new CallLogExt(mContext);
    }

    @Override
    public IDialerSearchExtension makeDialerSearchExt() {
        return new DialerSearchExtension(mContext);
    }

    public ICallDetailsExtension makeCallDetailsExt() {
        return new CallDetailsExtension(mContext);
    }
}
