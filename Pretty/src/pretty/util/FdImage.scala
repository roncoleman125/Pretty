/*
 * Copyright (c) Sclastic Contributors
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Scaly Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package pretty.util

import java.awt.Font
import java.awt.Color
import fractop.fdimage
import scala.io.Source
import javax.imageio.ImageIO
import java.io.File
import java.awt.image.BufferedImage
import scala.util.Random
import pretty.Stress
import pretty.Mango

object FdImage {
  val POINT_SIZE = 10
  val LINE_HEIGHT = POINT_SIZE
  
  /**
   * Gets an instance of the fractal dimension image.
   * @param path Path to source file to make an image
   * @param method Method to stress or "mango" file
   * @param save Set to true to save a png of image
   */
  def getInstance(path: String, method: String, save: Boolean = false): FdImage = {
    val stress = new Stress(path)

    val lines = method match {

      case "nm" =>
        val mango = new Mango(path)
        val trans = mango.getMappings(path)
        stress.nonmnemonic(trans)

      case "base" =>
        stress.none

      case "0" =>
        stress.noIndent()

      case "20" =>
        stress.randomIndent(20)
        
      case "40" =>
        stress.randomIndent(40)

      case _ =>
        println("bad method")
        System.exit(1)
        null
    }
    
    val width = lines.foldLeft(0) { (width, line) =>
      val len = line.length
      if (len > width) len else width
    }

    val fdi = new FdImage(lines, width)
    
    if(save)
      ImageIO.write(fdi,"png",new File(path+".png"))
    
    fdi
  }
}

/**
 * Makes an fractal dimension image from a ready-made image.
 * Useful mainly for testing purposes.
 */
class FdReadyImage(bimg: BufferedImage) extends fdimage(bimg.getWidth,bimg.getHeight,true) {
  val g = createGraphics
  
  g.drawImage(bimg,0,0,null)
}

/**
 * Makes a fractal dimension image from a set of lines.
 * @param lines Lines to image-ize
 * @param width Image width (i.e., the longest line length)
 * @param useText Set to true to write text instead of blocks
 */
class FdImage(lines: List[String], width: Int, useText: Boolean = false) extends fdimage(width * FdImage.POINT_SIZE, lines.length * FdImage.LINE_HEIGHT, true) {
  val g = createGraphics
  
  // Draw white background
  g.setColor(Color.white)
  
  g.fillRect(0, 0, width * FdImage.POINT_SIZE, lines.length * FdImage.LINE_HEIGHT)
  
  if(Constants.verbose) println("creating image w = " + width * FdImage.POINT_SIZE + " h = " + lines.length * FdImage.LINE_HEIGHT)
  
  // Set color to write text or blocks
  g.setColor(Color.black)

  // Set the font which we only use if using text
  g.setFont(new Font("Courier New", Font.PLAIN, FdImage.POINT_SIZE))

  // Write out image
  lines.foldLeft(0) { (y, line) =>
    if(useText)
      g.drawString(line, 0, y)
    else {
      line.foldLeft(0) { (x, c) =>
        if (c != ' ')
          g.fillRect(x, y, FdImage.POINT_SIZE - 2, FdImage.POINT_SIZE)

        x + FdImage.POINT_SIZE
      }
    }
    y + FdImage.LINE_HEIGHT
  }
}