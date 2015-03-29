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
package pretty

import java.awt.Color
import java.awt.Font
import java.awt.image.BufferedImage
import java.io.File

import scala.io.Source

import javax.imageio.ImageIO

object MakeImg {
  val POINT_SIZE = 10
  val LINE_HEIGHT = POINT_SIZE
  
  def main(args: Array[String]): Unit = {
    val lines = Source.fromFile("").getLines().toList
    
    val width = lines.foldLeft(0) { (width, line) =>
      val len = line.length
      if (len > width) len else width
    }
    
    val w = width * POINT_SIZE
    val h = lines.length * LINE_HEIGHT
    
    val bimg = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB)

    val g = bimg.createGraphics()

    g.setColor(Color.white)
    g.fillRect(0, 0, width * POINT_SIZE, lines.length * LINE_HEIGHT)
    println("w = " + width * POINT_SIZE + " h = " + lines.length * LINE_HEIGHT)
    g.setColor(Color.black);
    
    g.setFont(new Font("Courier New", Font.PLAIN, POINT_SIZE))
    
    lines.foldLeft(0) { (y, line) =>
      line.foldLeft(0) { (x, c) =>
        if(c != ' ')
          g.fillRect(x, y, POINT_SIZE-2, POINT_SIZE)
          
        x + POINT_SIZE
      }
      y + LINE_HEIGHT
    }
    
    ImageIO.write(bimg,"png",new File("x6-code.png"))
  }
}