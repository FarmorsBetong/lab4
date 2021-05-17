package connection;

import java.io.IOException;
import java.net.*;
import java.util.Scanner;


/**
 * The client class represents a client that starts and close a socket which it sends out datagrams to a server.
 * socket and address is variables that is being used to make a datagram.
 *
 * @author Roblof-8
 *
 * Version 1.0
 * Date : 2019-10-26
 *
 */
public class Client {
    private DatagramSocket socket;
    private Inet6Address address;

    private byte[] buf;

    /**
     * Constructor, creates a new socket and saves the local IPv6 address
     */
    public Client() {
        try {
            //Create a new socket (UDP socket)
            socket = new DatagramSocket();
            //Takes the local IPv6 address, ::1 is the loopback (localhost) address
            address = (Inet6Address) Inet6Address.getByName("::1");
        }catch (SocketException e) {
            System.out.println("socket exception");
        }catch (UnknownHostException e){
            System.out.println("unknown host exception");
        }
    }

    /**
     * SendEcho is a method used to pack a datagram and send it to a server that is listening to
     * the specified port, in this case 4445.
     * *precondition* String format "X:Y:color".
     *
     *
     * @param msg the String that are then packet to a byte[] and send to the server
     * @return a String message from the server
     *
     */
    public String sendEcho(String msg) {


            //Creates a byte array with the bytes in the message that are suppose to be sent.
            buf = msg.getBytes();
            //Makes a datagram packet with the
            DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 4445);
            try {
                //Sends the packet to the one who is listening to the port 4445
                socket.send(packet);
                System.out.println("packet sent");
            }catch (IOException e) {
                System.out.println("IO EXC");
            }

            //creates a new packet that is used to listen for server response
            packet = new DatagramPacket(new byte[30],30);
            try {
                //trying to receive
                socket.receive(packet);
            }catch (IOException e){
                System.out.println("IO EXE");
            }
            //makes a string with the data received from the server
            String received = new String(packet.getData(), 0, packet.getLength());
            return received;

    }

    /**
     * method is used to close the client UDP socket.
     *
     */
    public void close() {
        socket.close();
    }

    /**
     * Main method that starts the client and and takes in Strings from the keyboard.
     *
     * type "end" to close the application.
     * @param args String array.
     */
    public static void main(String[] args){
        Scanner scanner = new Scanner(System.in);

        Client client = new Client();

        boolean endScanner = true;

        while(endScanner) {
            System.out.print("Enter:");
            String msg = scanner.nextLine();

            if(msg.equals("end")){
                System.out.println("closing...");
                client.close();
                endScanner = false;
            }
            System.out.println(client.sendEcho(msg));
        }

    }

}
