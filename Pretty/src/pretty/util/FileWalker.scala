package pretty.util

import java.io.File

object FileWalker {
  def walk(path: String, suffix: String, action: (File) => Unit, recurse: Boolean = false): Unit = {
    val root = new File(path)
    
    val list = root.listFiles();
    val sz = list.size
    
    list.foreach { file =>
      if(file.isDirectory() && recurse)
        walk(file.getAbsolutePath(), suffix, action)
        
      else if(file.toString().endsWith(suffix))
        action(file)
    }
  }

}