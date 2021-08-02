package com.mediatek.gallerybasic.gl;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.RectF;

import java.nio.Buffer;


//
// GLCanvas gives a convenient interface to draw using OpenGL.
//
// When a rectangle is specified in this interface, it means the region
// [x, x+width) * [y, y+height)
//
public interface MGLCanvas {
    // Tells GLCanvas the size of the underlying GL surface. This should be
    // called before first drawing and when the size of GL surface is changed.
    // This is called by GLRoot and should not be called by the clients
    // who only want to draw on the GLCanvas. Both width and height must be
    // nonnegative.
    public abstract void setSize(int width, int height);

    // Clear the drawing buffers. This should only be used by GLRoot.
    public abstract void clearBuffer();

    public abstract void clearBuffer(float[] argb);

    // Sets and gets the current alpha, alpha must be in [0, 1].
    public abstract void setAlpha(float alpha);

    public abstract float getAlpha();

    // (current alpha) = (current alpha) * alpha
    public abstract void multiplyAlpha(float alpha);

    // Change the current transform matrix.
    public abstract void translate(float x, float y, float z);

    public abstract void translate(float x, float y);

    public abstract void scale(float sx, float sy, float sz);

    public abstract void rotate(float angle, float x, float y, float z);

    public abstract void multiplyMatrix(float[] mMatrix, int offset);

    // Pushes the configuration state (matrix, and alpha) onto
    // a private stack.
    public abstract void save();

    // Same as save(), but only save those specified in saveFlags.
    public abstract void save(int saveFlags);

    public static final int SAVE_FLAG_ALL = 0xFFFFFFFF;
    public static final int SAVE_FLAG_ALPHA = 0x01;
    public static final int SAVE_FLAG_MATRIX = 0x02;

    // Pops from the top of the stack as current configuration state (matrix,
    // alpha, and clip). This call balances a previous call to save(), and is
    // used to remove all modifications to the configuration state since the
    // last save call.
    public abstract void restore();

    // Fills the specified rectangle with the specified color.
    public abstract void fillRect(float x, float y, float width, float height, int color);

    // Draws a texture to the specified rectangle.
    public abstract void drawTexture(
            MBasicTexture texture, int x, int y, int width, int height);

    public abstract void drawMesh(MBasicTexture tex, int x, int y, int xyBuffer,
            int uvBuffer, int indexBuffer, int indexCount);

    // Draws the source rectangle part of the texture to the target rectangle.
    public abstract void drawTexture(MBasicTexture texture, RectF source, RectF target);

    // Draw a texture with a specified texture transform.
    public abstract void drawTexture(MBasicTexture texture, float[] mTextureTransform,
                int x, int y, int w, int h);

    // Draw two textures to the specified rectangle. The actual texture used is
    // from * (1 - ratio) + to * ratio
    // The two textures must have the same size.
    public abstract void drawMixed(MBasicTexture from, int toColor,
            float ratio, int x, int y, int w, int h);

    // Draw a region of a texture and a specified color to the specified
    // rectangle. The actual color used is from * (1 - ratio) + to * ratio.
    // The region of the texture is defined by parameter "src". The target
    // rectangle is specified by parameter "target".
    public abstract void drawMixed(MBasicTexture from, int toColor,
            float ratio, RectF src, RectF target);

    // Unloads the specified texture from the canvas. The resource allocated
    // to draw the texture will be released. The specified texture will return
    // to the unloaded state. This function should be called only from
    // MBasicTexture or its descendant
    public abstract boolean unloadTexture(MBasicTexture texture);

    // Delete the specified buffer object, similar to unloadTexture.
    public abstract void deleteBuffer(int bufferId);

    // Delete the textures and buffers in GL side. This function should only be
    // called in the GL thread.
    public abstract void deleteRecycledResources();

    // Dump statistics information and clear the counters. For debug only.
    public abstract void dumpStatisticsAndClear();

    public abstract void beginRenderTarget(MRawTexture texture);

    public abstract void endRenderTarget();

    /**
     * Sets texture parameters to use GL_CLAMP_TO_EDGE for both
     * GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T. Sets texture parameters to be
     * GL_LINEAR for GL_TEXTURE_MIN_FILTER and GL_TEXTURE_MAG_FILTER.
     * bindTexture() must be called prior to this.
     *
     * @param texture The texture to set parameters on.
     */
    public abstract void setTextureParameters(MBasicTexture texture);

    /**
     * Initializes the texture to a size by calling texImage2D on it.
     *
     * @param texture The texture to initialize the size.
     * @param format The texture format (e.g. GL_RGBA)
     * @param type The texture type (e.g. GL_UNSIGNED_BYTE)
     */
    public abstract void initializeTextureSize(MBasicTexture texture, int format, int type);

    /**
     * Initializes the texture to a size by calling texImage2D on it.
     *
     * @param texture The texture to initialize the size.
     * @param bitmap The bitmap to initialize the bitmap with.
     */
    public abstract void initializeTexture(MBasicTexture texture, Bitmap bitmap);

    /**
     * Calls glTexSubImage2D to upload a bitmap to the texture.
     *
     * @param texture The target texture to write to.
     * @param xOffset Specifies a texel offset in the x direction within the
     *            texture array.
     * @param yOffset Specifies a texel offset in the y direction within the
     *            texture array.
     * @param format The texture format (e.g. GL_RGBA)
     * @param type The texture type (e.g. GL_UNSIGNED_BYTE)
     */
    public abstract void texSubImage2D(MBasicTexture texture, int xOffset, int yOffset,
            Bitmap bitmap,
            int format, int type);

    /**
     * Generates buffers and uploads the buffer data.
     *
     * @param buffer The buffer to upload
     * @return The buffer ID that was generated.
     */
    public abstract int uploadBuffer(java.nio.FloatBuffer buffer);

    /**
     * Generates buffers and uploads the element array buffer data.
     *
     * @param buffer The buffer to upload
     * @return The buffer ID that was generated.
     */
    public abstract int uploadBuffer(java.nio.ByteBuffer buffer);

    /**
     * After LightCycle makes GL calls, this method is called to restore the GL
     * configuration to the one expected by GLCanvas.
     */
    public abstract void recoverFromLightCycle();

    /**
     * Gets the bounds given by x, y, width, and height as well as the internal
     * matrix state. There is no special handling for non-90-degree rotations.
     * It only considers the lower-left and upper-right corners as the bounds.
     *
     * @param bounds The output bounds to write to.
     * @param x The left side of the input rectangle.
     * @param y The bottom of the input rectangle.
     * @param width The width of the input rectangle.
     * @param height The height of the input rectangle.
     */
    public abstract void getBounds(Rect bounds, int x, int y, int width, int height);
    public abstract void setPerspective(float fovy, float aspect, float zNear, float zFar);
    public abstract void setLookAt(float eyeX, float eyeY, float eyeZ,
            float centerX, float centerY, float centerZ,
            float upX, float upY, float upZ);
    public abstract void drawMesh(MBasicTexture tex, float degree, float x, float y, float z,
            int xyzBuffer, int uvBuffer, int count);
    public abstract void readPixels(int x, int y, int width, int height, int type, int format,
            Buffer buffer);
    public abstract int getGLVersion();

    public int generateTexture();
    public void glGenBuffers(int n, int[] buffers, int offset);
    public void glDeleteTextures(int n, int[] textures, int offset);
    public void glDeleteBuffers(int n, int[] buffers, int offset);
    public void glDeleteFramebuffers(int n, int[] buffers, int offset);

    public interface Generator {
        public int generateTexture();
        public void glGenBuffers(int n, int[] buffers, int offset);
    }
    public void setGenerator(Generator gen);
    public int getWidth();
    public int getHeight();
    /**
     * Read Pixel buffer from texture, and create bitmap by it.
     * @param texture the texture read and copy from.
     * @param width the texture width.
     * @param height the texture height.
     * @return the bitmap create from texture.
     */
    public Bitmap saveTexture(int texture, int width, int height);
}
