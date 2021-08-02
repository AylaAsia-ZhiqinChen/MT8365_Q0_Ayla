package com.gsma.services.rcs.ft;

import com.gsma.services.rcs.contact.ContactId;

/**
 * Callback methods for one-to-one file transfer events
 */
interface IOneToOneFileTransferListener {

	void onStateChanged(in ContactId contact, in String transferId, in int state, in int reasonCode);

	void onProgressUpdate(in ContactId contact, in String transferId, in long currentSize, in long totalSize);

	void onDeleted(in ContactId contact, in List<String> transferIds);
}