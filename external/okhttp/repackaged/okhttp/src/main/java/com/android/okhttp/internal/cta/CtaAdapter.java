package com.android.okhttp.internal.cta;

import com.android.okhttp.MediaType;
import com.android.okhttp.Protocol;
import com.android.okhttp.Request;
import com.android.okhttp.Response;
import com.android.okhttp.ResponseBody;

import com.android.okhttp.okio.Buffer;
import com.android.okhttp.okio.BufferedSource;

import dalvik.system.PathClassLoader;

import java.lang.reflect.Method;
import java.util.List;

public final class CtaAdapter {

  private static Method ctaPermissionChecker = null;
  private static String jarPath = "system/framework/mediatek-cta.jar";
  private static String className = "com.mediatek.cta.CtaUtils";
  private static String methodName = "enforceCheckPermission";
  private static String mmsPermission = "com.mediatek.permission.CTA_SEND_MMS";
  private static String emailPermission = "com.mediatek.permission.CTA_SEND_EMAIL";
  private static String mmsMessage = "Send MMS";
  private static String emailMessage = "Send emails";

  public static boolean isSendingPermitted(Request request) {
    System.out.println("[okhttp]:check permission begin!");
    try {
      if (ctaPermissionChecker == null) {
        ClassLoader classLoader = new PathClassLoader(jarPath,CtaAdapter.class.getClassLoader());
        Class<?> cls = Class.forName(className, false, classLoader);
        ctaPermissionChecker = cls.getDeclaredMethod(methodName, String.class, String.class);
        ctaPermissionChecker.setAccessible(true);
      }

      if (isMms(request)) {
        return (Boolean) ctaPermissionChecker.invoke(null, mmsPermission, mmsMessage);
      } else if (isEmail(request)) {
        return (Boolean) ctaPermissionChecker.invoke(null, emailPermission, emailMessage);
      }

    } catch (ReflectiveOperationException e) {
      System.out.println("[okhttp] e:" + e);
      if (e.getCause() instanceof SecurityException) {
        throw new SecurityException(e.getCause());
      } else if (e.getCause() instanceof ClassNotFoundException) {
        // for project not include CTA feature
      }
    } catch (Throwable ee) {
      if (ee instanceof NoClassDefFoundError) {
        System.out.println("[okhttp] ee:" + ee);
      }
    }
    return true;
  }

  /** Check if {@code request} is a MMS http request. */
  private static boolean isMms(Request request) {
    final String mimetype = "application/vnd.wap.mms-message";
    if ("POST".equals(request.method())) {
      String userAgent = request.header("User-Agent");
      if (userAgent != null && userAgent.indexOf("MMS") != -1) {
        return true;
      } else {
        String contentType = request.header("Content-Type");
        if ((contentType != null) && (contentType.indexOf(mimetype) != -1)) {
          return true;
        }
        String acceptType = request.header("Accept");
        if ((acceptType != null) && (acceptType.indexOf(mimetype) != -1)) {
          return true;
        }
        List<String> contentTypes = request.headers().values("Content-Type");
        for (String value : contentTypes) {
          if (value.indexOf(mimetype) != -1) {
            return true;
          }
        }
      }
    }
    System.out.println("[okhttp]:not MMS!");
    return false;
  }

  /** Check if {@code request} is a Email http request. */
  private static boolean isEmail(Request request) {
    if ("POST".equals(request.method()) || "PUT".equals(request.method())) {
      String contentType = request.header("Content-Type");
      if ((contentType != null) && (contentType.startsWith("message/rfc822"))) {
        return true;
      }
      List<String> contentTypes = request.headers().values("Content-Type");
      for (String value : contentTypes) {
        if (value.startsWith("message/rfc822")) {
          return true;
        }
      }
    }
    System.out.println("[okhttp]:not Email!");
    return false;
  }

  /**
   * Create a {@code response}.
   * If apps send MMS or Email but don't have permission, return this response.
   */
  public static Response getBadHttpResponse(Request request) {
    ResponseBody emptyResponseBody = new ResponseBody() {
      @Override public MediaType contentType() {
        return null;
      }
      @Override public long contentLength() {
        return 0;
      }
      @Override public BufferedSource source() {
        return new Buffer();
      }
    };

    Response badResponse = new Response.Builder()
        .request(request)
        .protocol(Protocol.HTTP_1_1)
        .code(400)
        .message("User Permission is denied")
        .body(emptyResponseBody)
        .build();

    return badResponse;
  }

}
