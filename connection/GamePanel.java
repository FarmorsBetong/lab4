import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;

/**
 * GamePanel class represents a game panel.
 * Variables:
 * dim: used to set the dimension of the panel
 * UNIT_SIZE: is a final int that is used to describe the size of each rectangle on the panel
 * data: is a instance of the PanelData, which is a class that contains all the information and methods of the
 * game.
 *
 * @author Roblof-8
 *
 * Version 1.0
 * Date : 2019-10-26
 */

public class GamePanel extends JPanel implements Observer {

    Dimension dim;
    private final int UNIT_SIZE = 4;
    private PanelData data;

    /**
     * Constructor creates the game panel
     * @param data instance of the PanelData
     */

    public GamePanel(PanelData data){
        this.data = data;
        data.addObserver(this);

        dim = new Dimension(this.data.getGridSize() * UNIT_SIZE +1, this.data.getGridSize()* UNIT_SIZE +1);
        this.setMinimumSize(dim);
        this.setPreferredSize(dim);
        this.setBackground(Color.WHITE);

    }
    /**
     * After the observable changes this method will be called and calls the method repaint(), which calls
     * paintComponent(Graphics g).
     *
     * @param o Observable (PanelData)
     * @param arg object
     */
    @Override
    public void update(Observable o, Object arg) {
        this.repaint();
    }

    /**
     * Called with something is being updated on the screen or in the PanelData instance.
     * Paints the game panel.
     *
     * @param g grapthics instance
     */
    public void paintComponent(Graphics g){
        int row;
        int colum;
        super.paintComponent(g);


        for(int i = 0; i < data.getGridSize(); i++) {
            for (int j = 0; j < data.getGridSize(); j++) {
                g.setColor(Color.BLACK);
                row = i * UNIT_SIZE;
                colum = j * UNIT_SIZE;
                g.drawRect(row, colum, UNIT_SIZE, UNIT_SIZE);

                if (data.getGridLocation(i,j) == PanelData.BLUE) {
                    g.setColor(Color.BLUE);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.RED) {
                    g.setColor(Color.RED);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.GREEN) {
                    g.setColor(Color.GREEN);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.BLACK) {
                    g.setColor(Color.BLACK);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.ORANGE) {
                    g.setColor(Color.ORANGE);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.GREY) {
                    g.setColor(Color.GRAY);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.YELLOW) {
                    g.setColor(Color.YELLOW);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }
                if (data.getGridLocation(i,j) == PanelData.PINK) {
                    g.setColor(Color.PINK);
                    g.fillRect(row,colum,UNIT_SIZE,UNIT_SIZE);
                }

            }
        }

    }

}
