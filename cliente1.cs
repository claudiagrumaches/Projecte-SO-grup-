    public Form1()
    {
        InitializeComponent();
        // Se pueden agregar controles dinámicamente o mediante el diseñador.
        // En este ejemplo se asume que se han agregado los siguientes controles:
        // textUsuario, textPassword, textIdPartida (TextBox) y los botones:
        // btnConectar, btnRegistrarse, btnLogin, btnConsultaTotal, btnConsultaPartida, btnConsultaRecord, btnDesconectar.
    }

    // Botón Conectar
    private void btnConectar_Click(object sender, EventArgs e)
    {
        try
        {
            // Se utiliza la IP del servidor y el puerto 9000, que es el definido en el servidor
            IPAddress direc = IPAddress.Parse("192.168.56.102"); //REVISAR QUE LA IP SEA LA CORRECTA
            IPEndPoint ipep = new IPEndPoint(direc, 9000);

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

    // Botón Registrarse
    private void btnRegistrarse_Click(object sender, EventArgs e)
    {
        // Se toman los datos de los cuadros de texto para usuario y contraseña
        string usuario = textUsuario.Text.Trim();
        string password = textPassword.Text.Trim();

        if (usuario == "" || password == "")
        {
            MessageBox.Show("Introduce usuario y contraseña para registrarte.");
            return;
        }

        // Se arma el mensaje de registro (código 3)
        string mensaje = $"3/{usuario}/{password}";
        byte[] msg = Encoding.ASCII.GetBytes(mensaje);
        server.Send(msg);

        // Se espera la respuesta del servidor
        byte[] buffer = new byte[512];
        int bytesRec = server.Receive(buffer);
        string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
        MessageBox.Show(respuesta);
    }

    // Botón Login
    private void btnLogin_Click(object sender, EventArgs e)
    {
        // Dado que el servidor no implementa una operación de login,
        // en este ejemplo simplemente se comprueba que se hayan ingresado datos.
        string usuario = textUsuario.Text.Trim();
        string password = textPassword.Text.Trim();

        if (usuario == "" || password == "")
        {
            MessageBox.Show("Introduce usuario y contraseña para iniciar sesión.");
            return;
        }

        // Se podría mejorar comprobando el usuario en la base de datos, pero dado que no se implementa login en el servidor,
        // asumimos que el login es exitoso.
        usuarioLogueado = usuario;
        MessageBox.Show("Login exitoso. Usuario: " + usuarioLogueado);
    }

    // Botón Consulta de puntos totales (Código 4)
    private void btnConsultaTotal_Click(object sender, EventArgs e)
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
    private void btnConsultaPartida_Click(object sender, EventArgs e)
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
    private void btnConsultaRecord_Click(object sender, EventArgs e)
    {
        // Para el récord no se necesitan parámetros adicionales
        string mensaje = "6/";
        byte[] msg = Encoding.ASCII.GetBytes(mensaje);
        server.Send(msg);

        byte[] buffer = new byte[512];
        int bytesRec = server.Receive(buffer);
        string respuesta = Encoding.ASCII.GetString(buffer, 0, bytesRec);
        MessageBox.Show(respuesta);
    }

    // Botón Desconectar
    private void btnDesconectar_Click(object sender, EventArgs e)
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
