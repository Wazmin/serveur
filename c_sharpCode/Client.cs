using System;
using System.IO;
using System.Collections;
using System.Text;
using System.Net.Sockets;


namespace ClientServeurEntite
{

    class Client
    {
        const int READ_BUFFER_SIZE = 512;
        const int PORT_NUM = 12345;
        private TcpClient client;
        private byte[] readBuffer = new byte[READ_BUFFER_SIZE];
        public ArrayList lstUsers = new ArrayList();
        public string strMessage = string.Empty;
        public string res = String.Empty;
        private string pUserName;

        public Client(string adresseServeur)
        {
            client = new TcpClient();
            client.Connect("127.0.0.1", 1234);
        }

        // Use a StreamWriter to send a message to server.
        public void SendData()
        {
            //String str = Console.ReadLine();
            Stream stm = client.GetStream();
            ASCIIEncoding asen = new ASCIIEncoding();
            byte[] ba = asen.GetBytes("Salut noob");

            stm.Write(ba, 0, ba.Length);

            byte[] bb = new byte[100];
            int k = stm.Read(bb, 0, 100);

            for (int i = 0; i < k; i++)
                Console.Write(Convert.ToChar(bb[i]));


            client.Close();


        }
        public void Receive()
        {

        }

    }
}
