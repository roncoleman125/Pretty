package pretty

import java.util.zip.ZipFile
import scala.collection.JavaConverters.enumerationAsScalaIteratorConverter
import scala.io.Source
import scala.util.Random
import scala.collection.mutable.HashMap
import java.util.zip.ZipEntry

/**
 * This object "re"-comments a file using comments from the comment db.
 * @author Ron Coleman
 */
object Recomment {
  // These statistics were gathered from comments.awk on GLCU files -- before they were "atomized".
  val TOTAL_COMMENTS =  4627
  val TOTAL_LOC = 69722
  
  // Probability of encountering a comment
  val COMMENT_PROB = TOTAL_COMMENTS / TOTAL_LOC.toDouble
  
  // Tabs stops to format comments properly -- this may be off but it appears
  // to be the style in GLCU
  val TAB_STOP = "  "
  
  def main(args: Array[String]): Unit = {
    // Get the lines in the input file
    val lines = Source.fromFile(args(0)).getLines.toList

    // Open compressed comment db and get number of entries
    val zip = new ZipFile(args(1))

    import collection.JavaConverters._
    val entries = zip.entries.asScala.toList
    
    // This seed is so each file will be potentially unique
    val seed = checksum(lines)

    val ran = new Random(seed)
    
    // Calculate number of comments to add as a fraction of number of lines but...
    // we'll add at least one comment
    val numComments = ran.nextInt((lines.length * COMMENT_PROB).ceil.toInt) max 1
    
    // Choose place to put comment
    // If insert points collide this will reduce total number of comments
    val insertPoints = for(i <- (0 until numComments)) yield ran.nextInt(lines.length)

    // Chosse the comment
    val retrievalPoints = for(i <- (0 until insertPoints.length)) yield ran.nextInt(entries.length)
    
    // Build map of insert line number to comment number
    val insertThese = (0 until insertPoints.length).foldLeft(HashMap[Int,Int]()) { (map, index) =>
      map(insertPoints(index)) = retrievalPoints(index)
      map
    }
    
    // For each line, check if we need to write a comment

    (0 until lines.length).foreach { k =>
      // Calculate how much to indent the comment

      val nest = getNest(lines(k))
      
      // Output comment before the LOC
      if(insertThese.keySet.exists(_ == k)) {
        val commentLines = getComment(insertThese(k),zip,entries)
        commentLines.foreach { commentLine =>
          // Output blank before a comment: Ronz style!
          println
          
          // Make the indent
          val spaces = (0 until nest).foldLeft("") { (spaces, k) => spaces + TAB_STOP}
          println(spaces + commentLine)
        }
      }

      // Output the line
      println(lines(k))
    }
  }
  
  /** Retrieves comment from database */
  def getComment(index: Int,zip: ZipFile, entries: List[ZipEntry]): List[String] = {
      val entry = entries(index)
      
      val is = zip.getInputStream(entry)
          
      // Process entry line by line
      val lines = scala.io.Source.fromInputStream(is).getLines.toList
      
      lines
  }
  
  /** Computes a check sum for the file */
  def checksum(lines: List[String]): Long = {
    val sum = lines.filter(line => line.length > 0).foldLeft(0L) { (sum, line) =>
      sum + line.map {c => c.toLong}.reduce { (a,b) => a + b }
    }
    Math.abs(sum)
  }
  
  /**
   *  Compute how much to nest based on { and }.
   *  This will be wrong for if, for, and while without braces!
   */
  val depths = HashMap[String,Int]()  
  
  def getNest(line: String): Int = {
    // If we encounter { increment the indent
    val indent = if (line.indexOf("{") >= 0) depths.getOrElse("{", 0) + 1 else depths.getOrElse("{", 0)
    depths.put("{", indent)

    // If we encount } increment the outdent
    val outdent = if (line.indexOf("}") >= 0) depths.getOrElse("}", 0) + 1 else depths.getOrElse("}", 0)
    depths.put("}", outdent)

    // The nesting level is the difference between number of { and } we've encountered
    val nest = depths.getOrElse("{", 0) - depths.getOrElse("}", 0)

    nest
  }
}