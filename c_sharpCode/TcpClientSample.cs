/*
C# Network Programming 
by Richard Blum

Publisher: Sybex 
ISBN: 0782141765
*/
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class TcpClientSample
{
    public static void Main()
    {
        byte[] data = new byte[1024];
        string input, stringData;
        TcpClient server;

        try
        {
            server = new TcpClient("127.0.0.1", 1234);
        }
        catch (SocketException)
        {
            Console.WriteLine("Unable to connect to server");
            return;
        }
        NetworkStream ns = server.GetStream();

        int recv = ns.Read(data, 0, data.Length);
        stringData = Encoding.ASCII.GetString(data, 0, recv);
        Console.WriteLine(stringData);

        while (true)
        {
            //input = Console.ReadLine();
            //if (input == "exit")
            //    break;
            //ns.Write(Encoding.ASCII.GetBytes(input), 0, input.Length);
            //ns.Flush();

            data = new byte[1024];
            recv = ns.Read(data, 0, data.Length);
            stringData = Encoding.ASCII.GetString(data, 0, recv);
            Console.WriteLine(stringData);
        }
        Console.WriteLine("Disconnecting from server...");
        ns.Close();
        server.Close();
    }
}