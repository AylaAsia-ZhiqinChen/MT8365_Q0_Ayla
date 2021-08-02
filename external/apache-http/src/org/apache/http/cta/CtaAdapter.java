package org.apache.http.cta;

import dalvik.system.PathClassLoader;

import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpResponse;
import org.apache.http.HttpRequest;
import org.apache.http.HttpStatus;
import org.apache.http.HttpVersion;
import org.apache.http.RequestLine;
import org.apache.http.StatusLine;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.message.BasicHttpResponse;
import org.apache.http.message.BasicStatusLine;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EncodingUtils;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.Map;
import java.util.zip.GZIPInputStream;

public final class CtaAdapter {

    private static Method ctaPermissionChecker = null;
    private static String jarPath = "system/framework/mediatek-cta.jar";
    private static String className = "com.mediatek.cta.CtaUtils";
    private static String methodName = "enforceCheckPermission";
    private static String mmsPermission = "com.mediatek.permission.CTA_SEND_MMS";
    private static String emailPermission = "com.mediatek.permission.CTA_SEND_EMAIL";
    private static String mmsMessage = "Send MMS";
    private static String emailMessage = "Send emails";

    public static boolean isSendingPermitted(HttpRequest request, HttpParams defaultParams) {
        System.out.println("[apache]:check permission begin!");
        try {
            if (ctaPermissionChecker == null) {
                ClassLoader classLoader = new PathClassLoader(jarPath,
                        CtaAdapter.class.getClassLoader());
                Class<?> cls = Class.forName(className, false, classLoader);
                ctaPermissionChecker = cls.getDeclaredMethod(methodName,
                        String.class, String.class);
                ctaPermissionChecker.setAccessible(true);
            }

            if (isMms(request, defaultParams)) {
                return (Boolean) ctaPermissionChecker.invoke(null, mmsPermission, mmsMessage);
            } else if (isEmail(request)) {
                return (Boolean) ctaPermissionChecker.invoke(null, emailPermission, emailMessage);
            }

        } catch (ReflectiveOperationException e) {
            System.out.println("[apache] e:" + e);
            if (e.getCause() instanceof SecurityException) {
                throw new SecurityException(e.getCause());
            } else if (e.getCause() instanceof ClassNotFoundException) {
                // for project not include CTA feature
            }
        } catch (Throwable ee) {
            if (ee instanceof NoClassDefFoundError) {
                System.out.println("[apache] ee:" + ee);
            }
        }
        return true;
    }

    private static boolean isMms(HttpRequest request, HttpParams defaultParams) {
        final String mimetype = "application/vnd.wap.mms-message";
        RequestLine reqLine = request.getRequestLine();

        if (reqLine.getMethod().equals(HttpPost.METHOD_NAME)) {
            String userAgent = HttpProtocolParams.getUserAgent(defaultParams);
            if (userAgent != null && userAgent.indexOf("MMS") != -1) {
                return isMmsSendPdu(request);
            } else {
                if (request instanceof HttpEntityEnclosingRequest) {

                    HttpEntity entity = ((HttpEntityEnclosingRequest) request).getEntity();
                    if (entity != null) {
                        Header httpHeader = entity.getContentType();
                        if (httpHeader != null && httpHeader.getValue() != null) {
                            if (httpHeader.getValue().startsWith(mimetype)) {
                                return isMmsSendPdu(request);
                            }
                        }
                    }

                    Header[] headers = request.getHeaders(HTTP.CONTENT_TYPE);
                    if (headers != null) {
                        for (Header header : headers) {
                            if (header.getValue().indexOf(mimetype) != -1) {
                                return isMmsSendPdu(request);
                            }
                        }
                    }

                    headers = request.getHeaders("ACCEPT");
                    if (headers != null) {
                        for (Header header : headers) {
                            if (header.getValue().indexOf(mimetype) != -1) {
                                System.out.println("header done");
                                return isMmsSendPdu(request);
                            }
                        }
                    }
                }
            }
        }
        System.out.println("[apache]:not MMS!");
        return false;
    }

    private static boolean isEmail(HttpRequest request) {
        RequestLine reqLine = request.getRequestLine();
        final String mimetype = "application/vnd.ms-sync.wbxml";
        System.out.println("isEmailSend:" + reqLine.getMethod());

         if (reqLine.getMethod().equals(HttpPost.METHOD_NAME)
                    || reqLine.getMethod().equals(HttpPut.METHOD_NAME)) {

            // For debugging purpose
            Header[] hs = request.getAllHeaders();

            System.out.println("getAllHeaders:" + reqLine.getMethod());
            for (Header h : hs) {
                System.out.println("test:" + h.getName() + ":" + h.getValue());
            }

            if (request instanceof HttpEntityEnclosingRequest) {

                HttpEntity entity = ((HttpEntityEnclosingRequest) request).getEntity();
                if (entity != null) {
                    Header httpHeader = entity.getContentType();
                    System.out.println("httpHeader:" + httpHeader);
                    if (httpHeader != null && httpHeader.getValue() != null) {
                        if (httpHeader.getValue().startsWith("message/rfc822")
                            || httpHeader.getValue().startsWith(mimetype)) {
                            return true;
                        }
                    }
                }

                Header[] headers = request.getHeaders(HTTP.CONTENT_TYPE);
                if (headers != null) {
                    for (Header header : headers) {
                        if (header.getValue().startsWith("message/rfc822")
                            || header.getValue().startsWith(mimetype)) {
                            return true;
                        }
                    }
                }
            }
        }
        System.out.println("[apache]:not Email!");
        return false;
    }

    private static boolean isMmsSendPdu(HttpRequest request) {
        if (request instanceof HttpEntityEnclosingRequest) {
            System.out.println("[apache]:Check isMmsSendPdu");
            HttpEntity entity = ((HttpEntityEnclosingRequest) request).getEntity();
            if (entity != null) {
                InputStream nis = null;
                byte[] buf = new byte[2];
                int len = 0;

                try {
                    InputStream is = entity.getContent();
                    Header contentEncoding = entity.getContentEncoding();
                    if (contentEncoding != null
                            && contentEncoding.getValue().equals("gzip")) {
                        nis = new GZIPInputStream(is);
                    } else {
                        nis = is;
                    }

                    len = nis.read(buf);
                    System.out.println("PDU read len:" + len);
                    if (len == 2) {
                        //Convert to unsigned byte
                        System.out.println("MMS PDU Type:"
                            + (buf[0] & 0xFF) + ":" + (buf[1] & 0xFF));
                        //X-Mms-Message-Type: m-send-req (0x80)
                        if ((buf[0] & 0xFF) == 0x8C && (buf[1] & 0xFF) == 0x80) {
                            return true;
                        }
                    }
                } catch (IOException e) {
                    System.out.println("[apache]:" + e);
                } catch (IndexOutOfBoundsException ee) {
                    System.out.println("[apache]:" + ee);
                }
            }
        }
        System.out.println("[apache]:not MMS!");
        return false;
    }

    public static HttpResponse returnBadHttpResponse() {
        StatusLine statusLine = new BasicStatusLine(HttpVersion.HTTP_1_1,
                                HttpStatus.SC_BAD_REQUEST, "Bad Request");
        HttpResponse response = new BasicHttpResponse(statusLine);

        byte[] msg = EncodingUtils.getAsciiBytes("User Permission is denied");
        ByteArrayEntity entity = new ByteArrayEntity(msg);
        entity.setContentType("text/plain; charset=US-ASCII");
        response.setEntity(entity);

        return response;
    }

}
