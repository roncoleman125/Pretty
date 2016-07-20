/*
 * Copyright (c) Pretty Contributors
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

import java.io.PrintWriter

import scala.io.Source

import fractop.BoxCountingMethod
import fractop.MassRadiusMethod
import fractop.Util
import fractop.fdresult
import pretty.util.Config
import pretty.util.Constants
import pretty.util.Helper
import pretty.util.Imagery

object Analyze {
  def main(args: Array[String]): Unit = {
    val t0 = System.currentTimeMillis
    
    // Path to source file
    val (path, tabstop) = retab(args(0))
    
    // Stress method
    val method = args(1)
    
    // If stress is M, then just process it and we're done
    if(method == "em") {
      val m = Cyclomatic.em(path)
      println("M = "+m)
      System.exit(0)
    }
    
    // Get the configuration
    val configPath = args(2)
    
    // Fractal dimension measurement specification
    val measure = Config.getInstance(configPath).measure

    if(Constants.verbose) println("analyzing "+path+" with "+method+ " and "+measure+ " tabstop "+ tabstop) 
    
    val fdi = Imagery.getInstance(path, method)

    val fdr = new fdresult(true)
   
    if(Constants.verbose) println("measuring fractal dimension...")
    measure match {
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

    val t1 = System.currentTimeMillis
    
    println("FD = %f ".format(slope))
  }
  
  /** Retabs all lines in a file */
  def retab(path: String): (String, Int) = {
    val ts = System.getenv("TABSTOP")
    
    if(ts == null || ts.toInt == 0)
      return (path, 0)
    
    val tabstop = ts.toInt
    
    val TMPDIR = System.getenv("TMPDIR")
    
    if(TMPDIR == null) {
      Console.err.println("TMPDIR env var undefined to write retabbed file")
      System.exit(1)
    }
    
    val dest = TMPDIR + "RETABBED-" + Helper.basename(path)
    
    val out = new PrintWriter(dest)
    
    val lines = Source.fromFile(path).getLines
    
    lines.foreach { line =>
      val retabbed = JavaHelper.retab(line, tabstop)
      out.println(retabbed)
    }
    
    out.flush
    out.close
    
    (dest, tabstop)
  }
}

