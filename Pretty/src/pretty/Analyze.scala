package pretty

import scala.io.Source
import util.FdImage
import fractop.BoxCountingMethod
import fractop.fdresult
import fractop.Util
import fractop.MassRadiusMethod
import javax.imageio.ImageIO
import util.FdReadyImage
import java.io.File

object Analyze {
  def main(args: Array[String]): Unit = {
    val path = args(0)
    val mango = args(1)
    
    val fdi = FdImage.getInstance(path,mango)
//    val bimg = ImageIO.read(new File("data/make-detab.c.png"));
//    val fdi = new FdReadyImage(bimg)
    println("analyzing "+args(0)+" with "+args(1)+ " and "+args(2))
    val fdr = new fdresult(true)

    args(2) match {
      case "boxes" =>
        // Box values were taken from the GUI implementation.
        val boxes = Array(2, 3, 4, 6, 8, 12, 16, 32, 64, 128)

        // Parameters, -10 (start q), 10 (end q), and 1 (increment) used only
        // for multifractal analysis which is false
        BoxCountingMethod.setBoxOptions(boxes, false, -10, 10, 1)

        BoxCountingMethod.BoxCountingMethod(fdr, fdi)

        
      case "mass" =>
//        println("analyzing mass radius with "+args(1))
        Util.setParameters("0.33".toDouble,20)
        Util.Gyration(fdi)
        Util.ReadXY(fdi)
//        Util.setVerbose(true)
                
        MassRadiusMethod.MassRadius(fdr,fdi)
    
      fdr.CalcCumuList
      fdr.CalcLogCumu        
    }

    val slope = fdr.CalcLogSlope

    println("FD = " + slope)
  }
  
  

}

