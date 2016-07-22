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
   
  def stripDirectives(file: File, comment: Boolean): String = {
    stripDirectives(Source.fromFile(file).mkString, comment)
  }
  
  def stripDirectives(s: String, comment: Boolean): String = {
    val lines = s.split("\n")
     
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
    
    // Remove the lines by deleting or commenting them
    val (skipLineNums, _) = state
    
    val filteredLinesAsString = (0 until lines.length).foldLeft("") { (accum, k) =>
      val line = lines(k)
      
      if(skipLineNums.contains(k)) {
        if(comment)
          accum + "//" + line + "\n"
        else
          accum
      }
      else
        accum + line + "\n"
    }
    
//    println(filteredLinesAsString)
    filteredLinesAsString
  }

  def decomment(s: String): String = {
    null
  }
}