

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowEvent;

/**
 *
 * The GUI class creates a Jframe that holds a Jpanel object
 *
 * Variables:
 * gamePanel : Holds the state of the game panel.
 * PanelData : Holds the state of the data of the game
 * frame : The Object that holds the state of the frame.
 *
 * Version 1.0
 * Date: 2019-11-03
 *
 * @author Roblof-8
 */
public class GUI {

    private GamePanel gamePanel;
    private PanelData data;
    private JFrame frame;

    /**
     * The constructor creates a new frame and a objecct of gamePanel.
     *
     * @param data object that holds information about the game
     */
    public GUI(PanelData data){

        //variables
        this.data = data;
        SpringLayout layout = new SpringLayout();

        this.frame = new JFrame("THE GUI GOD");
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.setLocation(200,50);
        frame.setResizable(true);


        Container container = frame.getContentPane();
        container.setLayout(layout);
        gamePanel = new GamePanel(this.data);

        Dimension containerDim = new Dimension(gamePanel.dim.width + 100,gamePanel.dim.height + 100);
        container.setPreferredSize(containerDim);
        container.setBackground(Color.WHITE);

        //add components to the container
        container.add(gamePanel);


        layout.putConstraint(SpringLayout.HORIZONTAL_CENTER,gamePanel, 0, SpringLayout.HORIZONTAL_CENTER,container);
        layout.putConstraint(SpringLayout.VERTICAL_CENTER, gamePanel, 0, SpringLayout.VERTICAL_CENTER,container);

        //creates the the connection with the frame and cotentenpane and makes it visable.
        frame.setContentPane(container);
        frame.pack();
        frame.setVisible(true);

    }

    /**
     * Shutdown is used to close the Jframe window when the server is disconnected.
     */
    public void shutDown(){
     frame.dispatchEvent(new WindowEvent(frame,WindowEvent.WINDOW_CLOSING));
    }
}
