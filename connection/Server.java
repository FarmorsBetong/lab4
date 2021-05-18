
import java.io.IOException;
import java.net.*;
import java.util.Random;

/**
 * The Server class represents a server that takes UDP datagrams and answers on them.
 *
 * Variables:
 * socket: Is a UDP socket.
 * running: boolean for the program so it keeps running.
 * buf: is a byte[] with the information.
 * gui: instance of the GUI
 * data: instance of the PanelData
 *
 *
 * @author Roblof-8
 *
 * Version 1.0
 * Date : 2019-10-26
 *
 */

public class Server {

    private DatagramSocket socket;
    private boolean running;
    private byte[] buf = new byte[50];
    private GUI gui ;
    private PanelData data;


    /**
     * Constructor
     */
    public Server(int port) {
        try {
            //creates a new UDP socket that listening to port 4445.
            socket = new DatagramSocket(port);
        }catch (SocketException e){
            System.out.println("Socket Exception");
        }
            //new panel and data instances.
            data = new PanelData();
            gui = new GUI(data);
    }

    /**
     * Run method runs the server.
     * run method will end when the server receives a UDP packet with the String "close server".
     * Every loop cycle it creates a new datagram packet that the socket receive and reads.
     * Depending on what the String sent by the sender achieved at the server a message back
     * to the client will be sent back, with corresponding to what the String sent was achieved.
     *
     */
    public void run() {

            running = true;

            while (running) {


                    DatagramPacket packet = new DatagramPacket(buf, buf.length);

                    try {
                        socket.receive(packet);
                    }catch (IOException e){
                        System.out.println("IO Exception");
                    }

                    System.out.println("packet received");


                    InetAddress address = packet.getAddress();
                    int port = packet.getPort();

                    String received = new String(packet.getData(), 0, packet.getLength());

                    received = received.trim();

                    if(received.equals("close server")){
                        this.returnMSG("Closing server",address,port);
                        socket.close();
                        gui.shutDown();
                    }
                    else if(received.equals("randomize")){
                        Random random = new Random(1000);

                        for(int i = 0; i < data.getGridSize(); i++){
                            for(int j = 0; j < data.getGridSize(); j++){
                                data.setGridLocation(i,j,random.nextInt(((7)+1)-1));
                            }
                        }
                        this.returnMSG("randomized",address,port);
                        continue;
                    }
                    else if(received.equals("clear")){
                        for(int i = 0; i < data.getGridSize(); i++){
                            for(int j = 0; j < data.getGridSize(); j++){
                                data.setGridLocation(i,j,data.NO_COLOR);
                            }
                        }
                        this.returnMSG("cleared",address,port);
                        continue;
                    }

                try {


                    // X : Y : Color
                    String[] tokens = received.split(":");

                    int x;
                    int y;
                    String color = tokens[2].trim();

                    x = Integer.parseInt(tokens[0]);
                    y = Integer.parseInt(tokens[1]);


                    switch (color) {
                        case "blue":
                            data.setGridLocation(x, y, data.BLUE);
                            break;
                        case "red":
                            data.setGridLocation(x, y, data.RED);
                            break;
                        case "green":
                            data.setGridLocation(x, y, data.GREEN);
                            break;
                        case "black":
                            data.setGridLocation(x, y, data.BLACK);
                            break;
                        case "orange":
                            data.setGridLocation(x, y, data.ORANGE);
                            break;
                        case "grey":
                            data.setGridLocation(x, y, data.GREY);
                            break;
                        case "yellow":
                            data.setGridLocation(x, y, data.YELLOW);
                            break;
                        case "pink":
                            data.setGridLocation(x, y, data.PINK);
                            break;
                        case "white":
                            data.setGridLocation(x,y, data.NO_COLOR);
                        default:
                            System.out.println("noo thats no color bro...");
                            break;
                    }

                    returnMSG("Message receieved",address,port);


                }catch (Exception e){
                    returnMSG("Try the format 'X:Y:Color' ",address,port);
                }
            }

    }

    /**
     *
     * @param msg what message should be sent.
     * @param address address to the message.
     * @param port what port it should sent the message to.
     */
    private void returnMSG(String msg, InetAddress address, int port){
        String wrongMSG = msg;
        byte[] buff = wrongMSG.getBytes();
        DatagramPacket responseMSG = new DatagramPacket(buff, buff.length,address,port);
        try {
            socket.send(responseMSG);
        }catch (IOException e){
            System.out.println("IO");
        }
    }

    /**
     * Main method, starts the server, and the calls the run method.
     *
     * @param args String array
     */
    public static void main(String[] args) {

        Server server = new Server(4445);
        server.run();
    }
}
