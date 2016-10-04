import java.io.*;
import java.net.*;

class Server
{
	Socket client;
	Server(Socket client)
	{
		this.client=client;
		Communicate();
	}
	void Communicate()
	{
		try
		{
			BufferedReader kRead = new BufferedReader(new InputStreamReader(System.in));
			PrintWriter serverOut = new PrintWriter(client.getoutputStream(), true);
			BufferedReader serverRead = new BufferedReader(new InputStreamReader
				(client.getInputStream()));
			while(true)
			{
				String c_cmd = serverRead.readLine();
				System.out.println("Client :: " + c_cmd);
				String s_cmd = kRead.readLine();
				serverOut.println(s_cmd);
			}
		}catch (Exception e)
		{
			System.out.println("Connection lost...");
		}
	}
	public static void main(String args[])
	{
		try
		{
			int portNumber = Integer.parseInt(args[0]);
			ServerSocket ss = new ServerSocket(portNumber);
			ServerSocket ss1 = new ServerSocket(portNumber);
			System.out.println("Server Loaded");
			Socket s = ss.accept();
			Socket s1 = ss1.accept();
			System.out.println("Client connected...");
			new Server(s1);
			new Server(s);
		}catch (Exception e)
		{
			System.out.println("Connection lost...");
		}
	}

}