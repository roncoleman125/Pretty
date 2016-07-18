package pretty.util

import java.io.FileInputStream
import scala.io.Source
import java.io.File
import scala.collection.mutable.HashMap

object Helper {
  val state = System.getProperty("debug")
  
  val DEBUG = if(state == null || state != "true") false else true
  
  def logger(s: String): Unit = {
    if(DEBUG)
      println(s)
  }
  
  def basename(s: String): String = {
    val j = s.lastIndexOf('/')
    if(j == -1)
      s
    else
      s.substring(j+1)
  }

  def strip(file: File): String = {
    // Filter out compiler directives since C.g4 cant handle them
    val in = new FileInputStream(file)
    
    import org.apache.commons.io.IOUtils
    val bytes = IOUtils.toByteArray(in)
    
    val path = file.getAbsolutePath
//    println(">>>>> "+path)

    val lines = Source.fromBytes(bytes).getLines.toList
     
    val state = (0 until lines.length).foldLeft((HashMap[Int,Int](),false)) { (state, k) =>
      val (skipLineNums, inhash) = state
      
      val line = lines(k).trim
      
      if(line.startsWith("#") && line.endsWith("\\")) {
        skipLineNums(k) = k
        skipLineNums(k+1) = k+1
        (skipLineNums, true)
      }
      else if(line.startsWith("#")) {
        skipLineNums(k) = k
        (skipLineNums, false)
      }
      else if(line.endsWith("\\") && inhash) {
        skipLineNums(k+1) = k+1
        (skipLineNums, true)
      }
      else {
        (skipLineNums, false)
      }
    }
    
    // Remove the lines by commenting them out
    val (skipLineNums, _) = state
    
    val filteredLinesAsString = (0 until lines.length).foldLeft("") { (accum, k) =>
      if(skipLineNums.contains(k)) 
        accum + "//" + lines(k) + "\n"
      else
        accum + lines(k) + "\n"
    }

//    println(filteredLinesAsString)
    
    in.close
    
//    val bis = new ByteArrayInputStream(filteredLinesAsString.getBytes)
//    
//    bis
    
    filteredLinesAsString
  }

  def decomment(s: String): String = {
    null
  }
}