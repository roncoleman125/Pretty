package util

import java.awt.Font
import java.awt.Color
import fractop.fdimage

object FdImage {
  val POINT_SIZE = 10
  val LINE_HEIGHT = POINT_SIZE
}

class FdImage(lines: List[String], width: Int) extends 
  fdimage(width*FdImage.POINT_SIZE,lines.length*FdImage.LINE_HEIGHT,true) {
  
  val g = createGraphics
    g.setColor(Color.white)
    g.fillRect(0, 0, width * FdImage.POINT_SIZE, lines.length * FdImage.LINE_HEIGHT)
    println("w = " + width * FdImage.POINT_SIZE + " h = " + lines.length * FdImage.LINE_HEIGHT)
    g.setColor(Color.black);
    
    g.setFont(new Font("Courier New", Font.PLAIN, FdImage.POINT_SIZE))
    
    lines.foldLeft(0) { (y, line) =>
      line.foldLeft(0) { (x, c) =>
        if(c != ' ')
          g.fillRect(x, y, FdImage.POINT_SIZE-2, FdImage.POINT_SIZE)
          
        x + FdImage.POINT_SIZE
      }
      y + FdImage.LINE_HEIGHT
    }
}