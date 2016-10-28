using System;
using System.IO;
using System.Collections;
using System.Text;
using System.Net.Sockets;
using System.Threading;


namespace ClientServeurEntite
{

    class Client
    {

        public TcpClient theServer;
        NetworkStream networkStream;

        public Client()
        {
            theServer = new TcpClient();
        }

        // Connection au serveur
        public void ConnectToServeur(string adresseServeur)
        {
            try
            {
                theServer.Connect(adresseServeur, 1234);
            }
            catch (SocketException)
            {
                Console.WriteLine("Unable to connect to server");
                return;
            }
            networkStream = theServer.GetStream();
        }

        public void EnvoiFichier(string filename)
        {
            FileStream fs = new FileStream(filename, FileMode.Open);

            //envoi de l'entete + taille
            string entete = "06-" + fs.Length + "-i";
            networkStream.Write(Encoding.ASCII.GetBytes(entete), 0, entete.Length);
            networkStream.Flush();
            Thread.Sleep(500);


            int bufferSize = 1024;
            byte[] buffer = null;
            byte[] header = null;

            bool read = true;
            theServer.SendTimeout = 600000;
            theServer.ReceiveTimeout = 600000;

            int bufferCount = Convert.ToInt32(Math.Ceiling((double)fs.Length / (double)bufferSize));

            for (int i = 0; i < bufferCount; i++)
            {
                buffer = new byte[bufferSize];
                int size = fs.Read(buffer, 0, bufferSize);

                theServer.Client.Send(buffer, size, SocketFlags.Partial);
            }
            fs.Close();
        }

        // Reception d'un fichier du serveur

        public void ReceptionFichier()
        {
            int bufferSize = 1024;
            byte[] buffer = null;
            byte[] header = null;

            theServer.SendTimeout = 600000;
            theServer.ReceiveTimeout = 600000;

            //reception de l'entete
            header = new byte[bufferSize];
            //theServer.Client.Receive(buffer);

            // on boucle pour eviter la reception des coordonnees entite
            int byteRead;
            //do
            //{
                header = new byte[bufferSize];
                byteRead = networkStream.Read(header, 0, bufferSize);
                
            //} while (Receptionniste(header,byteRead) == 3);

            //entete fichier
            string entete = System.Text.Encoding.ASCII.GetString(header,0, byteRead);
            //07-[tailleFichier]-[type]-[nomFichier]
            Char delimiter = '-';
            String[] substrings = entete.Split(delimiter);
            int tailleFichier = int.Parse(substrings[1]);
            char type = substrings[2][0];
            string nomFichier = substrings[3];

            //creation du fichier
            string  s = @".\";
            s += nomFichier;
            FileStream fs = File.Create(s, tailleFichier);
            int thisRead = 0;
            buffer = new byte[bufferSize];
            while (tailleFichier>0)
            {
                thisRead= networkStream.Read(buffer, 0, bufferSize);
                fs.Write(buffer,0,thisRead);
                tailleFichier -= thisRead;
            }
            fs.Close();

            Console.Write("vous avez recu le fichier : "+nomFichier);
        }

        // Receptionniste en charge de trié les messages
        public int Receptionniste(byte[] buffer, int nbRecu)
        {
            string str = System.Text.Encoding.ASCII.GetString(buffer,0,nbRecu);
            int numRep;
            if (str.Substring(0,3) == "03-")
            {
                //majCoord
                numRep = 3;

            }
            else if(str.Substring(0, 3) != "07-")
            {
                numRep = 3;
            }
            else
            {
                numRep = 0;
            }

            return numRep;
            //int recv = ns.Read(data, 0, data.Length);
            //    stringData = Encoding.ASCII.GetString(data, 0, recv);
            //    Console.WriteLine(stringData);
        }

    }
}
