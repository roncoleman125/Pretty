package pretty.util

import java.io.PrintWriter
import java.io.File
/** Extant annotations */
object Annotation extends Enumeration {
  type Annotation = Value
  val METH, INIT, DECL, SKIP = Value
}

/** This class holds all the data needed to save the annotated program function */
case class Data(state: Annotation.Value,lines: List[String], fileno: Int )

/** This class contains info to write the method to a unique file name */
case class IO(path: String, outdir: String)

object Tester {
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
    
    fileNames.foreach { path =>
      println(path)
      fragment(path,outdir)
    }

  }
  
  def fragment(path: String, outdir: String): Unit = {
    val io = IO(path,outdir)
    
    // Process each line and flush the annotated sections
    val commentDirectives = true
    val lines = Helper.stripDirectives(new File(path), commentDirectives).split("\n")
    
    val terminated = lines.foldLeft(Annotation.SKIP,List[String](),-1) { (state, line) =>
      val (admit, buffer, k) = state
      line.toLowerCase match {
          // Skip preprocessor directives
        case s: String if s.startsWith("#") =>
          (admit,buffer,k)
          
          // Function or method coming
        case s: String if s.startsWith("@meth") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.METH, List[String](), k+1)
        
          // Initialization on way
        case s: String if s.startsWith("@init") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.INIT, List[String](), k)
          
          // Declaration on way
        case s: String if s.startsWith("@decl") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.DECL, List[String](), k)
          
          // Any of these on way (treated like comments for now)
        case s:String if s.startsWith("@enum") ||
                  s.startsWith("@struct") ||
                  s.startsWith("@union") ||
                  s.startsWith("@type") ||
                  s.startsWith("@comment") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.SKIP,List[String](), k)                    
          
          // Skipping from here
        case s: String if s.startsWith("@skip") =>
          flush(Data(admit, buffer, k), io)
          
          (Annotation.SKIP, List[String](), k)
          
        // Detect bogus annotations
        case s: String if s.startsWith("@") =>
          println("WARNING unknown annotation '" + s + "'")
          (admit,buffer,k)
          
          // Unless this is a skip, accumulate LOC
        case s: String if admit != Annotation.SKIP =>  
          val retabbed = pretty.JavaHelper.retab(line, 8)
          (admit,buffer ++ List(retabbed), k)
          
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
      
    val outputPath = io.outdir+"%03d-%s".format(data.fileno,(new File(io.path).getName))
    
    val pw = new PrintWriter(new File(outputPath))

    val lines = data.lines
    
    val sz = lines.size
    
    // Test method ending as "}" on last or next to last line
    val lastCurly = sz >= 3 && lines(sz-1).startsWith("}")
    val lastNextCurly = sz >= 3 && lines(sz-2).startsWith("}")
    
    if(sz >= 3 &&
       !lines(sz-1).startsWith("}") && 
       !lines(sz-1).startsWith("{}") &&
       !lines(sz-1).endsWith("}") &&
       !lines(sz-2).startsWith("}") &&
       !lines(sz-2).startsWith("{}") &&
       !lines(sz-2).endsWith("}"))
      println("WARNING doubtful method ending in "+Helper.basename(outputPath))
      
    // Write out all the lines except blank last lines
    (0 until sz).foreach { k =>
      val line = lines(k)
      if(line.length != 0 || k != (sz-1))
        pw.println(line)
    }
//    lines.foreach(pw.println)
    
    pw.flush
    pw.close
  }

  def getFiles(dir: String): List[String] = {
    val root = new File(dir)
    root.listFiles.filter(_.isFile()).map { f => f.getAbsolutePath }.toList
  }
}