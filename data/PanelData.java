package data;

import java.util.Observable;

/**
 * PanelData holds all the viable information about the game state.'
 *
 * Variables:
 * All variables are static and finals, and are used to define what box in the
 * game has what color.
 * the two-dim array is used to define the playground.
 *
 * @author Roblof-8
 *
 * Version 1.0
 * Date: 2019-11-03
 */
public class PanelData extends Observable {

    public static final int NO_COLOR = 0;
    public static final int BLUE = 1;
    public static final int RED = 2;
    public static final int GREEN = 3;
    public static final int BLACK = 4;
    public static final int ORANGE = 5;
    public static final int GREY = 6;
    public static final int YELLOW = 7;
    public static final int PINK = 8;
    public static final int PANELSIZE = 100;

    private int[][] gamePanel;

    /**
     * Constructor creates a playground that is 201-201 big.
     */
    public PanelData(){
        gamePanel = new int[PANELSIZE][PANELSIZE];
    }

    /**
     *
     * @return length of the two-dim array.
     */
    public int getGridSize() {
        return gamePanel.length;
    }


    /**
     *
     * @param x input of x cordinate
     * @param y input of y cordinate
     * @return the value of the cordinate (x,y)
     */
    public int getGridLocation(int x, int y){
        return gamePanel[x][y];
    }

    /**
     *
     * @param x input of x cordinate
     * @param y input of y cordinate
     * @param color what color that specific cordinate should have
     */
    public void setGridLocation(int x, int y, int color){
        gamePanel[x][y] = color;
        setChanged();
        notifyObservers();
    }
}
