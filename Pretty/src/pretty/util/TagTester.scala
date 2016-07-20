package pretty.util

import java.io.File

object TagTester {
  def main(args: Array[String]): Unit = {
    val file = new File(args(0))
    
    Tag.action(file)
  }
}