package util

import java.awt.Font
import java.awt.Color
import fractop.fdimage
import scala.io.Source
import javax.imageio.ImageIO
import java.io.File
import java.awt.image.BufferedImage
import scala.util.Random
import pretty.Mango

object FdImage {
  val POINT_SIZE = 10
  val LINE_HEIGHT = POINT_SIZE
  
  def getInstance(path: String, method: String, save: Boolean = false): FdImage = {
    val mango = new Mango(path+".mango")

    val lines = method match {

      case "mnemonics" =>
        mango.nonmnemonic(path)

      case "none" =>
        mango.none(path)

      case "0" =>
        mango.noIndent(path)

      case "20" =>
        mango.randomIndent(path,20)
        
      case "40" =>
        mango.randomIndent(path,40)

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
  
  def getReadyInstance(path: String): FdImage = {
    null
  }
}

class FdReadyImage(bimg: BufferedImage) extends fdimage(bimg.getWidth,bimg.getHeight,true) {
  val g = createGraphics
  
  g.drawImage(bimg,0,0,null)
//  for(x <- 0 to bimg.getWidth)
//    for(y <- 0 to bimg.getHeight)
//      this.setRGB(x, y, bimg.getRGB(x,y))
}

class FdImage(lines: List[String], width: Int) extends fdimage(width * FdImage.POINT_SIZE, lines.length * FdImage.LINE_HEIGHT, true) {
  val g = createGraphics
  
  g.setColor(Color.white)
  
  g.fillRect(0, 0, width * FdImage.POINT_SIZE, lines.length * FdImage.LINE_HEIGHT)
  
  println("w = " + width * FdImage.POINT_SIZE + " h = " + lines.length * FdImage.LINE_HEIGHT)
  
  g.setColor(Color.black)

  g.setFont(new Font("Courier New", Font.PLAIN, FdImage.POINT_SIZE))

  lines.foldLeft(0) { (y, line) =>
    line.foldLeft(0) { (x, c) =>
      if (c != ' ')
        g.fillRect(x, y, FdImage.POINT_SIZE - 2, FdImage.POINT_SIZE)

      x + FdImage.POINT_SIZE
    }
    y + FdImage.LINE_HEIGHT
  }
}