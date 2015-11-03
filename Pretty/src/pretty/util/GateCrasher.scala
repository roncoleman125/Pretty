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

/**
 * @author Ron.Coleman
 */
import java.io.File
import java.io.PrintWriter

import scala.io.Source

/** Extant annotations */
object Annotation extends Enumeration {
  type Annotation = Value
  val METH, INIT, DECL, SKIP = Value
}

/** This class holds all the data need to save the annotated program function */
case class Data(state: Annotation.Value,lines: List[String], fileno: Int )

case class IO(path: String, outdir: String)

/**
 * THis class is the main driver program to "crash" a file.
 */
object GateCrasher {
  
  def main(args: Array[String]): Unit = {
    if(args.length < 2) {
      println("usage: GateCrasher input-dir output-dir")
      System.exit(1)
    }
    
    // Input file
    val indir = args(0)
    
    // Output directory
    val outdir = args(1)
    
    val fileNames = getFiles(indir)
    
    fileNames.foreach { f =>
      fragment(f,outdir)
    }

  }
  
  def fragment(path: String, outdir: String): Unit = {
    println(path)
    val io = IO(path,outdir)
    
    // Process each line and flush the annotated sections
    val lines = Source.fromFile(path).getLines
    
    val terminated = lines.foldLeft(Annotation.SKIP,List[String](),-1) { (state, line) =>
      val (admit, buffer, k) = state
      line.toLowerCase match {
          // Skip preprocessor directives
        case s if s.startsWith("#") =>
          (admit,buffer,k)
          
          // Function or method coming
        case s if s.startsWith("@meth") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.METH, List[String](), k+1)
        
          // Initialization on way
        case s if s.startsWith("@init") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.INIT, List[String](), k)
          
          // Declaration on way
        case s if s.startsWith("@decl") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.DECL, List[String](), k)
          
          // Any of these on way (treated like comments for now)
        case s if s.startsWith("@enum") ||
                  s.startsWith("@struct") ||
                  s.startsWith("@union") ||
                  s.startsWith("@type") ||
                  s.startsWith("@comment") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.SKIP,List[String](), k)                    
          
          // Skipping from here
        case s if s.startsWith("@skip") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.SKIP, List[String](), k)
          
          // Unless this is a skip, accumulate LOC
        case s if admit != Annotation.SKIP =>         
          (admit,buffer ++ List(line), k)
          
        case _ =>
          (admit,buffer,k)
      }
    }
    
    val(admit, buffer, k) = terminated
    
    flush(Data(admit, buffer, k), io)    
  }
  
  def flush(data: Data, io: IO): Unit = {
    if(data.state != Annotation.METH || data.fileno < 0)
      return
      
    val outputPath = io.outdir+"%02d-%s".format(data.fileno,(new File(io.path).getName))
    
    val pw = new PrintWriter(new File(outputPath))
    
    data.lines.foreach(pw.println)
    
    pw.flush
    pw.close
  }

  def getFiles(dir: String): List[String] = {
    val root = new File(dir)
    root.listFiles.filter(_.isFile()).map { f => f.getAbsolutePath }.toList
  }
}