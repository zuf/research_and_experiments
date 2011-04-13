package panoGL;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLException;

import java.awt.Font;
import java.awt.Frame;
import java.awt.event.*;
import java.awt.geom.*;
import java.text.*;

import com.sun.opengl.util.*;
import com.sun.opengl.util.j2d.*;

import java.awt.*;
import java.awt.event.*;
import javax.media.opengl.*;
import com.sun.opengl.util.*;

import java.io.*;
import java.net.URL;

import javax.media.opengl.glu.*;
import com.sun.opengl.util.texture.*;

/**
 * PanoGL - tiny spherical (cube) panorama viewer
 *
 */
public class Pano implements GLEventListener, MouseListener,
    MouseMotionListener, MouseWheelListener, Runnable
{

  private Thread       thr                = null;
  private boolean      textures_loaded    = false;

  private GLU          glu                = new GLU();
  private GLUT         glut               = new GLUT();

  private float        view_rotx          = 20.0f, view_roty = 30.0f,
      view_rotz = 0.0f;
  double               fov                = 90;

  private int          prevMouseX, prevMouseY;
  // private boolean mouseRButtonDown = false;

  private Texture      cubemap            = null;

  private double       g_width            = 0, g_height = 0;

  private String       url                = "";
  private String       texture_extensions = "jpg";

  private TextRenderer renderer;

  public Pano(String url)
  {
    this.url = url;
  }

  public Pano(String url, String extensions)
  {
    this.url = url;
    this.texture_extensions = extensions;
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * javax.media.opengl.GLEventListener#display(javax.media.opengl.GLAutoDrawable
   * )
   */
  @Override
  public void display(GLAutoDrawable drawable)
  {
    // Get the GL corresponding to the drawable we are animating
    GL gl = drawable.getGL();

    // Special handling for the case where the GLJPanel is translucent
    // and wants to be composited with other Java 2D content
    if ((drawable instanceof GLJPanel) && !((GLJPanel) drawable).isOpaque()
        && ((GLJPanel) drawable).shouldPreserveColorBufferIfTranslucent())
    {
      gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
    } else
    {
      gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
    }

    gl.glMatrixMode(GL.GL_PROJECTION);
    gl.glLoadIdentity();
    // gl.glFrustum(-1.0f, 1.0f, -h, h, 1.0f, 6000.0f);
    glu.gluPerspective(fov, g_width / g_height, 0.01, 10000.0);

    gl.glRotatef(view_rotx, 1.0f, 0.0f, 0.0f);
    gl.glRotatef(view_roty, 0.0f, 1.0f, 0.0f);
    gl.glRotatef(view_rotz, 0.0f, 0.0f, 1.0f);

    // Mirror texture
    gl.glMatrixMode(GL.GL_TEXTURE_MATRIX);
    gl.glScalef(-1.0f, 1.0f, 1.0f);

    gl.glMatrixMode(GL.GL_MODELVIEW);
    gl.glLoadIdentity();

    if (textures_loaded)
    {
      if (cubemap == null)
      {
        try
        {
          cubemap = Cubemap.getCubemap();
        } catch (GLException e)
        {
          // TODO Auto-generated catch block
          e.printStackTrace();
        } catch (IOException e)
        {
          // TODO Auto-generated catch block
          e.printStackTrace();
        }
      }

      prepareSkyBox(gl);
      drawSkyBox(gl);
    } else
    {
      // glut.glutSolidSphere(5.0, 40, 20);
      gl.glColor3d(0.66, 0.66, 0.66);
      glut.glutWireSphere(300.0, 40, 20);

      // Prepare to draw text
      renderer.beginRendering(drawable.getWidth(), drawable.getHeight());
      // Draw text
      renderer.draw("Loading textures (" + Cubemap.getLoadetCount() + "/"
          + Cubemap.getLength() + ")", 8, 8);
      // Clean up rendering
      renderer.endRendering();
    }

  }

  /*
   * (non-Javadoc)
   *
   * @see javax.media.opengl.GLEventListener#displayChanged(javax.media.opengl.
   * GLAutoDrawable, boolean, boolean)
   */
  @Override
  public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2)
  {
    // TODO Auto-generated method stub

  }

  private void init_textures()
  {
    try
    {
      Cubemap.loadFromUrl(getClass().getClassLoader(), url, texture_extensions,
          false);

    } catch (IOException e)
    {
      System.err.println("ERROR: Can't load texture!");
      // shutdownDemo();
      System.exit(0);
      throw new RuntimeException(e);
    }

    textures_loaded = true;
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * javax.media.opengl.GLEventListener#init(javax.media.opengl.GLAutoDrawable )
   */
  @Override
  public void init(GLAutoDrawable drawable)
  {
    // Use debug pipeline
    // drawable.setGL(new DebugGL(drawable.getGL()));

    GL gl = drawable.getGL();

    // System.err.println("INIT GL IS: " + gl.getClass().getName());
    // System.err.println("Chosen GLCapabilities: "
    // + drawable.getChosenGLCapabilities());
    //
    // System.err.println("GL_VENDOR: " + gl.glGetString(GL.GL_VENDOR));
    // System.err.println("GL_RENDERER: " + gl.glGetString(GL.GL_RENDERER));
    // System.err.println("GL_VERSION: " + gl.glGetString(GL.GL_VERSION));

    gl.setSwapInterval(0);

    renderer = new TextRenderer(new Font("SansSerif", Font.BOLD, 18));

    drawable.addMouseListener(this);
    drawable.addMouseMotionListener(this);

    // init_textures();
    // prepareSkyBox(gl);
    thr = new Thread(this);
    thr.start();
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * javax.media.opengl.GLEventListener#reshape(javax.media.opengl.GLAutoDrawable
   * , int, int, int, int)
   */
  @Override
  public void reshape(GLAutoDrawable drawable, int x, int y, int width,
      int height)
  {
    GL gl = drawable.getGL();

    // float h = (float) height / (float) width;

    g_width = width;
    g_height = height;

    gl.glMatrixMode(GL.GL_PROJECTION);

    gl.glLoadIdentity();
  }

  /*
   * (non-Javadoc)
   *
   * @see java.awt.event.MouseListener#mouseClicked(java.awt.event.MouseEvent)
   */
  @Override
  public void mouseClicked(MouseEvent e)
  {
    // TODO Auto-generated method stub

  }

  /*
   * (non-Javadoc)
   *
   * @see java.awt.event.MouseListener#mouseEntered(java.awt.event.MouseEvent)
   */
  @Override
  public void mouseEntered(MouseEvent e)
  {
    // TODO Auto-generated method stub

  }

  public void mouseWheelMoved(MouseWheelEvent e)
  {
    fov += e.getWheelRotation();
    if (fov < 1)
      fov = 1;
    if (fov > 179)
      fov = 179;
  }

  /*
   * (non-Javadoc)
   *
   * @see java.awt.event.MouseListener#mouseExited(java.awt.event.MouseEvent)
   */
  @Override
  public void mouseExited(MouseEvent e)
  {
    // TODO Auto-generated method stub

  }

  /*
   * (non-Javadoc)
   *
   * @see java.awt.event.MouseListener#mousePressed(java.awt.event.MouseEvent)
   */
  @Override
  public void mousePressed(MouseEvent e)
  {
    prevMouseX = e.getX();
    prevMouseY = e.getY();
    if ((e.getModifiers() & e.BUTTON3_MASK) != 0)
    {
      // mouseRButtonDown = true;
    }

  }

  /*
   * (non-Javadoc)
   *
   * @see java.awt.event.MouseListener#mouseReleased(java.awt.event.MouseEvent)
   */
  @Override
  public void mouseReleased(MouseEvent e)
  {
    if ((e.getModifiers() & e.BUTTON3_MASK) != 0)
    {
      // mouseRButtonDown = false;
    }
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * java.awt.event.MouseMotionListener#mouseDragged(java.awt.event.MouseEvent )
   */
  @Override
  public void mouseDragged(MouseEvent e)
  {
    int x = e.getX();
    int y = e.getY();
    Dimension size = e.getComponent().getSize();

    float thetaY = 360.0f * ((float) (x - prevMouseX) / (float) size.width);
    float thetaX = 360.0f * ((float) (prevMouseY - y) / (float) size.height);

    prevMouseX = x;
    prevMouseY = y;

    view_rotx += thetaX * fov / 180.0; // Correct rotation angle with FOV weight
    view_roty += thetaY * fov / 180.0;
  }

  /*
   * (non-Javadoc)
   *
   * @see
   * java.awt.event.MouseMotionListener#mouseMoved(java.awt.event.MouseEvent)
   */
  @Override
  public void mouseMoved(MouseEvent e)
  {
    // TODO Auto-generated method stub

  }

  /**
   * @param args
   */
  public static void main(String[] args)
  {
    Frame frame = new Frame("PanoGL test");
    GLCanvas canvas = new GLCanvas();

    // System.out.println(args[0]);

    Pano pn = new Pano(args[0]);
    canvas.addGLEventListener(pn);
    canvas.addMouseWheelListener(pn);

    frame.add(canvas);
    frame.setSize(600, 600);
    final Animator animator = new Animator(canvas);
    frame.addWindowListener(new WindowAdapter()
    {
      public void windowClosing(WindowEvent e)
      {
        // Run this on another thread than the AWT event queue to
        // make sure the call to Animator.stop() completes before
        // exiting
        new Thread(new Runnable()
        {
          public void run()
          {
            animator.stop();
            System.exit(0);
          }
        }).start();
      }
    });
    frame.setVisible(true);
    animator.start();
  }

  private void prepareSkyBox(GL gl)
  {
    gl.glTexParameteri(GL.GL_TEXTURE_CUBE_MAP, GL.GL_TEXTURE_MAG_FILTER,
        GL.GL_LINEAR);
    gl.glTexParameteri(GL.GL_TEXTURE_CUBE_MAP, GL.GL_TEXTURE_MIN_FILTER,
        GL.GL_LINEAR);

    gl.glTexGeni(GL.GL_S, GL.GL_TEXTURE_GEN_MODE, GL.GL_NORMAL_MAP);
    gl.glTexGeni(GL.GL_T, GL.GL_TEXTURE_GEN_MODE, GL.GL_NORMAL_MAP);
    gl.glTexGeni(GL.GL_R, GL.GL_TEXTURE_GEN_MODE, GL.GL_NORMAL_MAP);

    gl.glEnable(GL.GL_TEXTURE_GEN_S);
    gl.glEnable(GL.GL_TEXTURE_GEN_T);
    gl.glEnable(GL.GL_TEXTURE_GEN_R);

    gl.glTexEnvi(GL.GL_TEXTURE_ENV, GL.GL_TEXTURE_ENV_MODE, GL.GL_REPLACE);

    gl.glEnable(GL.GL_TEXTURE_CUBE_MAP);
    cubemap.disable();
  }

  private void drawSkyBox(GL gl)
  {
    gl.glActiveTexture(GL.GL_TEXTURE0);
    cubemap.bind();
    cubemap.enable();
    glut.glutSolidSphere(300.0, 40, 20);
    cubemap.disable();
  }

  private void unsetSkyBox(GL gl)
  {
    cubemap.disable();
    gl.glDisable(GL.GL_TEXTURE_GEN_S);
    gl.glDisable(GL.GL_TEXTURE_GEN_T);
    gl.glDisable(GL.GL_TEXTURE_GEN_R);
  }

  @Override
  public void run()
  {
    // int count = 0;
    while (textures_loaded == false)
    {
      // System.out.println("No textures, " + count++);
      // if (count > 100000)
      init_textures();
    }
  }

}
