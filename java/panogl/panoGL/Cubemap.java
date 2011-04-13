package panoGL;

/*
 * Copyright (c) 2003-2005 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistribution of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistribution in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. SUN
 * MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL SUN OR
 * ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE
 * DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY,
 * ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF
 * SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * You acknowledge that this software is not designed or intended for use
 * in the design, construction, operation or maintenance of any nuclear
 * facility.
 *
 * Sun gratefully acknowledges that this software was originally authored
 * and developed by Kenneth Bradley Russell and Christopher John Kline.
 */

import java.io.*;

import javax.media.opengl.*;
import com.sun.opengl.util.*;
import com.sun.opengl.util.texture.*;

import java.net.URL;

/** Helper class for loading cubemaps from a set of textures. */

public class Cubemap
{
  //private static final String[] suffixes = { "posx", "negx", "posy", "negy", "posz", "negz"};
  private static final String[] suffixes = { "1", "3", "4", "5", "0", "2"};
  private static final int[]    targets  = { GL.GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      GL.GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL.GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
      GL.GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL.GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      GL.GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
  private static final TextureData datas[] = new TextureData[suffixes.length];

  private static int loadet_datas = 0;


  public static int getLoadetCount()
  {
    return loadet_datas;
  }

  public static int getLength()
  {
    return suffixes.length;
  }

  public static void loadFromUrl(ClassLoader scope, String basename,
      String suffix, boolean mipmapped) throws IOException, GLException
  {
    for (int i = 0; i < suffixes.length; i++)
    {
      String resourceName = basename + suffixes[i] + "." + suffix;
      TextureData data = TextureIO.newTextureData(// scope.getResourceAsStream(
          // resourceName),
          new URL(resourceName), mipmapped, FileUtil
              .getFileSuffix(resourceName));
      if (data == null)
      {
        throw new IOException("Unable to load texture " + resourceName);
      }
      datas[i] = data;

      // try
      // {
      // Thread.sleep(1000);
      // } catch (InterruptedException e)
      // {
      // // TODO Auto-generated catch block
      // e.printStackTrace();
      // }

      loadet_datas++;
    }
  }

  public static Texture getCubemap() throws IOException, GLException
  {
    Texture cubemap = TextureIO.newTexture(GL.GL_TEXTURE_CUBE_MAP);

    for (int i = 0; i < suffixes.length; i++)
    {
      TextureData data = datas[i];
      if (data == null)
      {
        throw new IOException("Unable to update cubemap texture #" + i);
      }

      cubemap.updateImage(data, targets[i]);

    }

    return cubemap;
  }

}
