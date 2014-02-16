package butterfly;

/**
 *
 * @author Artemis
 */
public class Point2D extends Object{
    private double x, y;  // ta private den klironomountai!!!!

    //double constructor
    Point2D(double x, double y) {
        setX(x);
        setY(y);
    }

    //X property
    double X() { return x; }
    public void setX(double x) { this.x = x; }
    public void setX(float x) { this.x = (double) x; }
    public void setX(int x) { this.x = (double) x; }

    //Y property
    double Y() { return y; }
    public void setY(double y) { this.y = y; }
    public void setY(float y) { this.y = (double) y; }
    public void setY(int y) { this.y = (double) y; }

    public void add(Point2D p){
        this.x += p.X();
        this.y += p.Y();
    }

    public void sub(Point2D p){
        this.x -= p.X();
        this.y -= p.Y();
    }

    public void scale(double i){
        this.x = this.x * i;
        this.y = this.y * i;
    }

    public void normalize(){
        double norm=Math.sqrt(this.x*this.x + this.y*this.y);
        this.x=this.x / norm;
        this.y=this.y / norm;
    }

}