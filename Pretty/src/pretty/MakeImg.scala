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