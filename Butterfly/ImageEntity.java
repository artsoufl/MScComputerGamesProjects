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
import java.net.*;
import java.applet.*;

public class ImageEntity {
    protected Image image;
    protected Applet applet;
    protected Graphics2D g2d;
    protected Point2D pos;   //position of image
    protected boolean alive;

    //default constructor
    ImageEntity(Applet a)  {
        applet = a;
        //setPosition(0,0);
        setAlive(true);
    }

    public boolean isAlive(){return alive;}
    public void setAlive(boolean alive){this.alive=alive;}

    public Image getImage() { return image; }

    public void setPosition(int x, int y){
         pos=new Point2D(x,y);  //position of each image
    }
    
    public void setX(double x){
       pos.setX(x);
    }
    
    public void setY(double y){
       pos.setY(y);
    }

    public double getX(){
        return pos.X();
    }

    public double getY(){
        return pos.Y();
    }
    public Point2D position(){ return pos;}

    private URL getURL(String filename) {
        URL url = null;
        try {
            url = this.getClass().getResource(filename);
            //url = new java.net.URL(applet.getCodeBase() + filename); //isws gia gif
        }
       // catch (MalformedURLException e) { e.printStackTrace(); }
        catch (Exception e) { }

        return url;
    }

    public void load(String filename) {
       Toolkit tk = Toolkit.getDefaultToolkit();
       image = tk.getImage(getURL(filename));
    }
    
    public int width() {
        if (image != null)
            return image.getWidth(applet);
        else
            return 0;
    }
    public int height() {
        if (image != null)
            return image.getHeight(applet);
        else
            return 0;
    }

    //bounding rectangle
    public Rectangle getBounds(int width, int height) {
        Rectangle r;
        r = new Rectangle((int)getX(), (int)getY(), width-5, height-5);
        return r;
    }
}
