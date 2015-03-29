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
        Util.setParameters("0.33".toDouble, 20)
        Util.Gyration(fdi)
        Util.ReadXY(fdi)

        MassRadiusMethod.MassRadius(fdr, fdi)

        fdr.CalcCumuList
        fdr.CalcLogCumu        
    }

    val slope = fdr.CalcLogSlope

    println("FD = " + slope)
  }
}

