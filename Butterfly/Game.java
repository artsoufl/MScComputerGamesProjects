/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package butterfly;

/**
 *
 * @author Artemis
 */
import java.applet.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.util.LinkedList;

abstract public class Game extends Applet implements Runnable, MouseListener{

    private BufferedImage backbuffer;
    private Graphics2D g2d;
    private int screenWidth, screenHeight;
    private long startTime = System.currentTimeMillis();
    private long updateTime = System.currentTimeMillis();
    private Thread gameloop;
    private int update=0;
    public Point2D mousePos = new Point2D(300,300);

    public AudioClip[] sound= {newAudioClip(getClass().getResource("ding.wav")),newAudioClip(getClass().getResource("beep.wav"))};

    abstract void gameRefreshScreen();
    abstract void gameStartup();
    abstract void gameMouseLeft();
    abstract void gameMouseRight();

    public void playSound(int i){
        sound[i].play();
    }


     //internal list of sprites
    private LinkedList _sprites;
    public LinkedList sprites() { return _sprites; }

    public Graphics2D graphics() { return g2d; }
    public Point2D mousePosition() { return mousePos; }

    public Game(int width, int height) {
        screenWidth = width;
        screenHeight = height;
    }

     public void init() {
        setSize(screenWidth,screenHeight);

	backbuffer = new BufferedImage(screenWidth, screenHeight, BufferedImage.TYPE_INT_RGB);
        g2d = backbuffer.createGraphics();
        g2d.setColor( Color.green );
        g2d.fillRect( 0, 0, screenWidth, screenHeight );

        _sprites = new LinkedList<Sprite>();

        addMouseListener(this);
        gameStartup();

    }
     
      public void run() {
         //acquire the current thread
         Thread t = Thread.currentThread();

         //process the main game loop thread
         while (t == gameloop) {
             try {
                 //set a  frame rate
                 Thread.sleep(60);
             }
             catch(InterruptedException e) {
                 e.printStackTrace();
             }

             //testCollisions();
             //refresh the screen
             repaint();
         }
    }

      
        protected void testCollisions() {
         //iterate through the sprite list, test each sprite against
         //every other sprite in the list
         Sprite spr1 = (Sprite) _sprites.get(0);// butterfly
         if (spr1.getEntity().isAlive()) {
                 //look through all sprites for collisions
                 for (int second = 1; second < _sprites.size(); second++) {
                         Sprite spr2 = (Sprite) _sprites.get(second);
                         if (spr2.getEntity().isAlive()) {
                             if (spr2.collidesWith(spr1)) {
                                 spr2.getEntity().setAlive(false);
                                 break;
                             }
                         }
                  }
         }
     }
       


      private void purgeSprites() {
         for (int n=0; n < _sprites.size(); n++) {
             Sprite spr = (Sprite) _sprites.get(n);
             if (!spr.getEntity().isAlive()) {
                 _sprites.remove(n);
             }
         }
     }

      public void update(Graphics g){
        if (System.currentTimeMillis() > startTime + 250) {
            startTime = System.currentTimeMillis();
            update=0;
            //sec++;         //once every second
        }
        if (System.currentTimeMillis() > updateTime + 1000) {
            updateTime = System.currentTimeMillis();
           // purgeSprites();
        }
       
        gameRefreshScreen();
        paint(g);
    }

      public long getUpdateTime(){return updateTime;}

      public int getUpdate(){return update;}
      public void setUpdate(int i){update=i;}


     public void paint(Graphics g){
               g.drawImage(backbuffer, 0, 0, this);
               gameRefreshScreen();
    }

     public void mousePressed(MouseEvent e) {}
     public void mouseReleased(MouseEvent e) { }
     public void mouseEntered(MouseEvent e) { }
     public void mouseExited(MouseEvent e) { }
     public void mouseClicked(MouseEvent e) {
         mousePos.setX(e.getX());
         mousePos.setY(e.getY());
         switch(e.getModifiers()) {
              case InputEvent.BUTTON1_MASK: {   //left clicked
                  gameMouseLeft();
                  break;
              }
              case InputEvent.BUTTON3_MASK: {  //right clicked
                  gameMouseRight();
                  break;
              }
         }
     }

}
