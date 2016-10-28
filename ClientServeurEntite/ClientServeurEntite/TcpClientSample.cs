/*
C# Network Programming 
by Richard Blum

Publisher: Sybex 
ISBN: 0782141765
*/
using System;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.IO;

public class TcpClientSample
{
    static void EnvoiFichier(string filename, NetworkStream networkStream, TcpClient tcpClient)
    {
        FileStream fs = new FileStream(filename, FileMode.Open);
 
        //envoi de l'entete + taille
        string entete = "06-" + fs.Length+"-i";
        networkStream.Write(Encoding.ASCII.GetBytes(entete), 0, entete.Length);
        networkStream.Flush();

        int bufferSize = 1024;
        byte[] buffer = null;
        byte[] header = null;

        bool read = true;
        tcpClient.SendTimeout = 600000;
        tcpClient.ReceiveTimeout = 600000;

        int bufferCount = Convert.ToInt32(Math.Ceiling((double)fs.Length / (double)bufferSize));

        for (int i = 0; i < bufferCount; i++)
        {
            buffer = new byte[bufferSize];
            int size = fs.Read(buffer, 0, bufferSize);

            tcpClient.Client.Send(buffer, size, SocketFlags.Partial);

        }

        fs.Close();
    }

    //public static void Main()
    //{

    //    byte[] data = new byte[1024];
    //    string input, stringData;
    //    TcpClient server;

    //    try
    //    {
    //        server = new TcpClient("127.0.0.1", 1234);
    //    }
    //    catch (SocketException)
    //    {
    //        Console.WriteLine("Unable to connect to server");
    //        return;
    //    }
    //    NetworkStream ns = server.GetStream();

    //    int recv = ns.Read(data, 0, data.Length);
    //    stringData = Encoding.ASCII.GetString(data, 0, recv);
    //    Console.WriteLine(stringData);

    //    while (true)
    //    {
    //        input = Console.ReadLine();
    //        if (input == "exit")
    //        {
    //            ns.Close();
    //            server.Close();
    //            break;
    //        }
    //        else
    //        {

    //            EnvoiFichier(input, ns, server);
    //        }

    //        ns.Write(Encoding.ASCII.GetBytes(input), 0, input.Length);
    //        ns.Flush();

    //        data = new byte[1024];
    //        recv = ns.Read(data, 0, data.Length);
    //        stringData = Encoding.ASCII.GetString(data, 0, recv);
    //        Console.WriteLine(stringData);
    //    }
    //    Console.WriteLine("Disconnecting from server...");
    //    while (true) { }
    //}


}