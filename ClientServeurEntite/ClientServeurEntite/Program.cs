using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClientServeurEntite
{
    class Program
    {

        public static void Main()
        {
            Client myInterface = new ClientServeurEntite.Client();
            myInterface.ConnectToServeur("10.1.63.231");

            //myInterface.EnvoiFichier("Vintale.jpg");
            //Console.Write("fin envoie fichier");
            //Console.Write("reception fichier");
            //myInterface.ReceptionFichier();
            //Console.Write("fin reception fichier");
            string input;
            Console.Write("entrez un nom de fichier dans le rep");
            input = Console.ReadLine();
            myInterface.EnvoiFichier(input);
            myInterface.ReceptionFichier();
            while (true) { };

        }

    }
}
//192.168.1.12