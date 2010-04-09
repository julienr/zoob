#!/bin/sh
exec scala $0 $@
!#

/**
 * Script to create a texture atlas from a true type file
 * The atlas is divided in square of charSize*charSize which contains every characters
 * in "chars"
 */

import java.io._
import java.awt._
import java.awt.image.BufferedImage
import javax.imageio.ImageIO

if (args.length < 2) {
  println("Usage: ft2tex <font> <output file>");
  exit()
}

val fontFile = args(0)
val outFile = args(1)

//the character that will be available in our texture
val chars = "0123456789:=?_@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz"

val imgWidth : Int = 256
val imgHeight : Int = 256
//Store characters in square of charSize*charSize
val charSize : Int = 30 //can store 66 characters of size 30 in a 256*256 font
val image = new BufferedImage(imgWidth, imgHeight, BufferedImage.TYPE_INT_RGB)

val font = loadFont(fontFile).deriveFont(charSize.toFloat-2.0f)
println("Using font : " + font.getName())
println("\t number of glyphs : " + font.getNumGlyphs())
println("\t point size : " + font.getSize2D())
println("\t uniform line metrics : " + font.hasUniformLineMetrics())

printToImage(chars, font, image)
saveImage(outFile, image)

def loadFont (file: String) = Font.createFont(Font.TRUETYPE_FONT, new File(file))

def printToImage (s: String, font: Font, image: BufferedImage) {
  val g = image.getGraphics.asInstanceOf[Graphics2D]
  g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                     RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

  g.setFont(font)

  val bounds = font.getStringBounds(s, g.getFontRenderContext())
  //We are making the font monospaced, so just calculate charWidth
  val charWidth : Int = (bounds.getWidth()/s.length()).toInt
  val numCharPerLine : Int = imgWidth/charSize
  println("string bounds : (" + bounds.getHeight +"," +bounds.getWidth + ")")
  println("string length : " + s.length)
  println("char width : " + charWidth)
  println("num chars per line : " + numCharPerLine)

  val chars : scala.List[Char] = s.toList 
  var line = 0
  var col = 0
  for (c <- chars) {
    if (line*charSize >= imgHeight) {
      println("Error : line*charSize >= imgHeight")
      exit()
    }
    g.drawString(c.toString, col*charSize, (line+1)*charSize)
    if (col >= numCharPerLine) {
      col = 0
      line = line + 1
    } else
      col = col + 1
  }

  //g.drawString(s, 50, 50)
}

def saveImage (file: String, image: BufferedImage) {
  ImageIO.write(image, "PNG", new File(file))
}

