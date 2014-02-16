/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package butterfly;

/**
 *
 * @author Artemis
 */
import java.awt.*;
import java.applet.*;
import java.awt.image.*;

public class Sprite {
    ImageEntity entity;
    BufferedImage tempImage;
    private int currFrame, totFrames;
    private int animDir;
    private int frCount, frDelay;
    private int frWidth, frHeight;
    private int cols;
    private int mywidth,myheight;
    protected double dx, dy;
    Graphics2D tempSurface;


    Sprite(Applet applet){
        entity=new ImageEntity(applet);
        currFrame = 0;
        totFrames = 0;
        animDir = 1;
        frCount = 0;
        frDelay = 0;
        frWidth = 0;
        frHeight = 0;
        cols = 0;
    }

    public void update(int delta) {
        entity.setX(entity.getX()+delta*dx); 
        entity.setY(entity.getY()+delta*dy);
    }

    public double getDX() {
        return dx;
    }

    public double getDY() {
        return dy;
    }

    public void setDX(double dx) {
        this.dx = dx;
    }

    public void setDY(double dy) {
        this.dy = dy;
    }

    //load bitmap file
    public void load(String filename) {
        entity.load(filename);
    }

    public Image getImage(){
        return entity.getImage();
    }

    public int getFrameX(){
        return (currentFrame() % columns()) * frameWidth();
    }

    public int getFrameY(){
        return (currentFrame() / columns()) * frameHeight();
    }

    public void updateFrame() {
      //  if (totFrames > 0) {
            //calculate the current frame's X and Y position
            int frameX = (currentFrame() % columns()) * frameWidth();
            int frameY = (currentFrame() / columns()) * frameHeight();

            /*
            if (tempImage == null) {
                tempImage = new BufferedImage(frameWidth(), frameHeight(),
                                              BufferedImage.TYPE_INT_ARGB);
                //tempSurface = tempImage.createGraphics();
            }
            */
            //copy the frame onto the temp image
            /*
            if (animImage.getImage() != null) {
                tempSurface.drawImage(animImage.getImage(), 0, 0, frameWidth() - 1,
                frameHeight() - 1, frameX, frameY,
                frameX + frameWidth(),
                frameY + frameHeight(), applet());
            }
             * */

            //pass the temp image on to the parent class and draw it
            //super.setImage(tempImage);
        //}
    }

    //returns a bounding rectangle
    public Rectangle getBounds() { return entity.getBounds(myWidth(),myHeight()); }
     //check for collision with another sprite
    public boolean collidesWith(Sprite sprite) {
        return (getBounds().intersects(sprite.getBounds()));
    }

    public ImageEntity getEntity(){return entity;}

    public int myWidth(){return mywidth;}
    public void setMyWidth(int width){mywidth=width;}

    public int myHeight(){return myheight;}
    public void setMyHeight(int height){myheight=height;}

    public int currentFrame() { return currFrame; }
    public void setCurrentFrame(int frame) { currFrame = frame; }

    public int frameWidth() { return frWidth; }
    public void setFrameWidth(int width) { frWidth = width; }

    public int frameHeight() { return frHeight; }
    public void setFrameHeight(int height) { frHeight = height; }

    public int totalFrames() { return totFrames; }
    public void setTotalFrames(int total) { totFrames = total; }

    public int animationDirection() { return animDir; }
    public void setAnimationDirection(int dir) { animDir = dir; }

    public int frameDelay() { return frDelay; }
    public void setFrameDelay(int delay) { frDelay = delay; }

    public int columns() { return cols; }
    public void setColumns(int num) { cols = num; }
}
