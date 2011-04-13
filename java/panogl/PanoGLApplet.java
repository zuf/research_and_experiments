import java.applet.Applet;

import java.applet.*;
import java.awt.*;
import java.io.*;

import javax.media.opengl.*;

import com.sun.corba.se.impl.protocol.giopmsgheaders.MessageBase;
import com.sun.opengl.util.*;
import com.sun.org.apache.xml.internal.serializer.utils.Messages;

import panoGL.Pano;

public class PanoGLApplet extends Applet
{
  private Animator animator  = null;

  private boolean  loaded    = false;

  private String   panos_url = null;
  private String   panos_ext = null;
  private Pano     pn        = null;
  private Label    loading   = null;
  private GLCanvas canvas    = null;

  public void init()
  {
    setLayout(new BorderLayout());

    // setBackground(Color.red);

    loading = new Label("Loading panorama textures. Please wait.");
    loading.setVisible(true);
    add(loading, BorderLayout.CENTER);

    panos_url = getParameter("panos_url");
    panos_ext = getParameter("image_format");

    if (panos_ext == null)
      panos_ext = "jpg";

    if (panos_url != null && !loaded)
    {
      loaded = true;
      pn = new Pano(panos_url);
      canvas = new GLCanvas();
      canvas.addGLEventListener(pn);
      canvas.addMouseWheelListener(pn);

      canvas.setSize(getSize());
      this.remove(loading);
      add(canvas, BorderLayout.CENTER);

      animator = new FPSAnimator(canvas, 60);

      loading = null;
    } else
    {
      System.err.println("ERROR: Wrong panos URL!");
      loading.setText("ERROR: Wrong panos URL!");
    }
  }

  public void start()
  {
    if (animator != null)
      animator.start();
  }

  public void stop()
  {
    if (animator != null)
      animator.stop();
  }

}
