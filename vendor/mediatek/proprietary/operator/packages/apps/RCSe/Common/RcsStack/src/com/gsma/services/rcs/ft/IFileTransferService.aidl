package com.gsma.services.rcs.ft;

import android.net.Uri;

import com.gsma.services.rcs.IRcsServiceRegistrationListener;
import com.gsma.services.rcs.ft.IFileTransfer;
import com.gsma.services.rcs.ft.IOneToOneFileTransferListener;
import com.gsma.services.rcs.ft.IGroupFileTransferListener;
import com.gsma.services.rcs.ft.IFileTransferServiceConfiguration;
import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.RcsServiceRegistration;
import com.gsma.services.rcs.ICommonServiceConfiguration;

/**
 * File transfer service API
 */
interface IFileTransferService {

	boolean isServiceRegistered();
	
	int getServiceRegistrationReasonCode();

	void addEventListener(IRcsServiceRegistrationListener listener);

	void removeEventListener(IRcsServiceRegistrationListener listener);

	IFileTransferServiceConfiguration getConfiguration();

	IFileTransfer getFileTransfer(in String transferId);

	IFileTransfer transferFile(in ContactId contact, in Uri file, in boolean attachFileicon);
	
	IFileTransfer transferFileLargeMode(in ContactId contact, in Uri file,in int disposition, in boolean attachFileicon);
	
	IFileTransfer transferFileToMultiple(in List<String> chatId, in Uri file,in int disposition, in boolean attachFileicon);

	void markFileTransferAsRead(in String transferId);
	
	void addEventListener2(in IOneToOneFileTransferListener listener);

	void removeEventListener2(in IOneToOneFileTransferListener listener);

	void addEventListener3(in IGroupFileTransferListener listener);

	void removeEventListener3(in IGroupFileTransferListener listener);
	
	int getServiceVersion();

	boolean isAllowedToTransferFile(in ContactId contact);

	boolean isAllowedToTransferFileToGroupChat(in String chatId);

	void deleteOneToOneFileTransfers();

	void deleteGroupFileTransfers();

	void deleteOneToOneFileTransfers2(in ContactId contact);

	void deleteGroupFileTransfers2(in String chatId);

	void deleteFileTransfer(in String transferId);

	void clearFileTransferDeliveryExpiration(in List<String> transferIds);
	
	ICommonServiceConfiguration getCommonConfiguration();

	IFileTransfer transferFile2(in ContactId contact, in Uri file, in int disposition, in boolean attachFileicon);

	IFileTransfer transferFileToGroupChat2(in String chatId, in Uri file, in int disposition, in boolean attachFileicon);

	IFileTransfer resumeFileTransfer(in String transferId);

	IFileTransfer resendFileToGroup(in String transferId, in String chatId, in List<String> contacts, in Uri file,in boolean attachFileicon);

	IFileTransfer resumeGroupFileTransfer(in String chatId, in String transferId);
}