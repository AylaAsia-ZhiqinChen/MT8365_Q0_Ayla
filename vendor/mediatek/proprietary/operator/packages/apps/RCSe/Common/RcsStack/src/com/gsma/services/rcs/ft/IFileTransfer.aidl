package com.gsma.services.rcs.ft;

import android.net.Uri;

import com.gsma.services.rcs.contact.ContactId;

/**
 * File transfer interface
 */
interface IFileTransfer {

	String getChatId();

	String getTransferId();

	ContactId getRemoteContact();

	String getFileName();

	long getFileSize();

	String getMimeType();

	Uri getFileIcon();

	String getFileIconMimeType();

	Uri getFile();

	int getState();

	int getReasonCode();
	
	int getDirection();

	long getTimestamp();

	long getTimestampSent();

	long getTimestampDelivered();

	long getTimestampDisplayed();
		
	void acceptInvitation();

	void rejectInvitation();

	void abortTransfer();
	
	void pauseTransfer();
	
	void resumeTransfer();

	boolean isRead();

	boolean isAllowedToResendTransfer();

	void resendTransfer();

	boolean isAllowedToPauseTransfer();

	boolean isAllowedToResumeTransfer();
	
	long getFileExpiration();
	
	long getFileIconExpiration();

	boolean isExpiredDelivery();

	int getDisposition();
}
