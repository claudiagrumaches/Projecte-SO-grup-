using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace cliente_V1
{
    public partial class Form1 : Form
    {
        Socket server;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {


        }

        // Botón Conectar
        private void Conectar_Click(object sender, EventArgs e)
        {
            try
            {
                // Se utiliza la IP del servidor y el puerto 9000, que es el definido en el servidor
                IPAddress direc = IPAddress.Parse("192.168.56.102"); //REVISAR QUE LA IP SEA LA CORRECTA
                IPEndPoint ipep = new IPEndPoint(direc, 9080);

                server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                server.Connect(ipep);
                this.BackColor = Color.Green;
                MessageBox.Show("Conectado al servidor");
            }
            catch (SocketException ex)
            {
                MessageBox.Show("No se pudo conectar con el servidor:\n" + ex.Message);
            }
        }

        private void Registrarse_Click(object sender, EventArgs e)
        {
            // Se toman los datos de los cuadros de texto para usuario y contraseña
            // Se toman los datos de los cuadros de texto para usuario y contraseña
            string usuari = textUsuario.Text.Trim();
            string contrasenya = textPassword.Text.Trim();

            if (usuari == "" || contrasenya == "")
            {
                MessageBox.Show("Introduce usuario y contraseña para registrarte.");
                return;
            }

            else
            {
                string mensaje = $"3/{usuari}/{contrasenya}";
                byte[] msg = Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                // Se espera la respuesta del servidor
                byte[] buffer = new byte[512];
                int bytesRec = server.Receive(buffer);
                string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
                MessageBox.Show(respuesta);
            }
            // Se arma el mensaje de registro (código 3)
        }

        private string usuarioLogueado = ""; // Declarar la variable a nivel de clase
        private void Login_Click(object sender, EventArgs e)
        {
            // Dado que el servidor no implementa una operación de login,
            // en este ejemplo simplemente se comprueba que se hayan ingresado datos.
            string usuari = textUsuario.Text.Trim();
            string contrasenya = textPassword.Text.Trim();

            if (usuari == "" || contrasenya == "")
            {
                MessageBox.Show("Introduce usuario y contraseña para iniciar sesión.");
                return;
            }

            else
            {
                string mensaje = $"2/{usuari}/{contrasenya}";
                byte[] msg = Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                // Se espera la respuesta del servidor
                byte[] buffer = new byte[512];
                int bytesRec = server.Receive(buffer);
                string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
                MessageBox.Show(respuesta);
                usuarioLogueado = usuari;
            }
        }

        // Botón Consulta de puntos totales (Código 4)
        private void ConsultaTotal_Click(object sender, EventArgs e)
        {
            if (usuarioLogueado == "")
            {
                MessageBox.Show("Debes iniciar sesión primero.");
                return;
            }

            // Se arma el mensaje para consultar los puntos totales (código 4)
            string mensaje = $"4/{usuarioLogueado}";
            byte[] msg = Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            byte[] buffer = new byte[512];
            int bytesRec = server.Receive(buffer);
            string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
            MessageBox.Show(respuesta);
        }
        
        // Botón Consulta de puntos por partida (Código 5)
        private void ConsultaPartida_Click(object sender, EventArgs e)
        {
            // Se toma el id de partida desde el cuadro de texto correspondiente
            string idPartida = textIdPartida.Text.Trim();
            if (idPartida == "")
            {
                MessageBox.Show("Introduce el ID de la partida para la consulta.");
                return;
            }

            string mensaje = $"5/{idPartida}";
            byte[] msg = Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            byte[] buffer = new byte[512];
            int bytesRec = server.Receive(buffer);
            string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
            MessageBox.Show(respuesta);

        }

        // Botón Consulta del récord (Código 6)
        private void ConsultaRecord_Click(object sender, EventArgs e)
        {
            // Se toma el id de partida desde el cuadro de texto correspondiente
            string idPartida = textIdPartida.Text.Trim();
            if (idPartida == "")
            {
                MessageBox.Show("Introduce el ID de la partida para la consulta.");
                return;
            }
            // Para el récord no se necesitan parámetros adicionales
            string mensaje = $"6/{idPartida}";
            byte[] msg = Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            byte[] buffer = new byte[512];
            int bytesRec = server.Receive(buffer);
            string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
            MessageBox.Show(respuesta);
        }
        
        // Botón Desconectar
        private void Desconectar_Click(object sender, EventArgs e)
        {
            try
            {
                // Se envía el mensaje de desconexión (código 0)
                string mensaje = "0/";
                byte[] msg = Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);
                server.Shutdown(SocketShutdown.Both);
                server.Close();
                this.BackColor = Color.Gray;
                MessageBox.Show("Desconectado del servidor");
            }
            catch (SocketException ex)
            {
                MessageBox.Show("Error al desconectar:\n" + ex.Message);
            }
        }

        
    }
}



