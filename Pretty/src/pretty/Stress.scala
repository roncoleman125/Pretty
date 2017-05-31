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

import scala.collection.mutable.HashMap
import scala.io.Source
import scala.util.Random

object Stress { 
  
}

/**
 * This class implements the stresses.
 * @param path File path of the source file to stressor.
 */
class Stress(path: String) { 
  /** Doesn't mango the file. */
  def none(): List[String] = {
    Source.fromFile(path).getLines().toList
  }
  
  /**
   * Makes the input file nonmnemonic.
   * @param tpath File path of the transformations
   */
  def filter(tpath: String): List[String] = {
    val trans = HashMap[String,String]()
    
    val maps = Source.fromFile(tpath).getLines().toList

    // Load the transformations
    maps.foreach { line =>
      val keyValue = line.split(" ")
      trans(keyValue(0)) = keyValue(1)
    }
    
    filter(null,trans)
  }

  def filter(keys: List[String], trans: HashMap[String, String]): List[String] = {
    val lines = Source.fromFile(path).getLines().toList
    lines.map {
      line => transform(line, keys, trans)
    }
  }

  def transform(line: String, keys: List[String], transMap: HashMap[String, String]): String = {
    keys.foldLeft(line) { (revisedLine, key) =>
      val oldSym = key
      val newSym = transMap(key)
      revisedLine.replaceAll(oldSym, newSym)
    }
  }
  
  /** Removes all indents. */
  def noIndent(): List[String] = {
    val lines = Source.fromFile(path).getLines().toList
    lines.map { line =>
      line.trim
    }
  }

  /** Randomizes the indent. */
  def randomIndent(n: Int): List[String] = {
    val lines = Source.fromFile(path).getLines().toList
    
    val seed = Recomment.checksum(lines) + n
    
    val ran = new Random(seed)
    
    lines.map { line =>
      val r = ran.nextInt(n) + 1
      
      val indent = (1 to r).foldLeft("") { (xs, x) => xs + " " }
      println(indent + line)
      indent + line
    }
  }
}