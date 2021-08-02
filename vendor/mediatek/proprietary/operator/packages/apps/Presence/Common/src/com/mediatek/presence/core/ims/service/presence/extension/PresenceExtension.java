package com.mediatek.presence.core.ims.service.presence.extension;

import com.mediatek.presence.utils.logger.Logger;

public abstract class PresenceExtension {

   public static final int EXTENSION_VILTE = 1;
   public static final int EXTENSION_RCS = 2;
 
   
    /**
    * The logger
    */
   public Logger logger = Logger.getLogger(this.getClass().getName());
   
   protected String extensionName;
   private int extension;
   private PresenceExtensionListener ExtensionChangeListener = null;
   
   PresenceExtension(int extensionCode){
       this.extension = extensionCode;
   }
   
   //attach extension and use its services
   public void attachExtension(PresenceExtensionListener listener){
       ExtensionChangeListener = listener;
   }
   
   //detach extension and discontinue with its services
   public void detachExtension(){
       ExtensionChangeListener = null;
   }
   
   public String getName(){
       return extensionName;
   }
   
   public  void notifyListener(){
       if(ExtensionChangeListener !=null){
           ExtensionChangeListener.handleCapabilityChanged(this);
       }
   }
   
}
