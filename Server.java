import java.io.*;
import java.net.*;

class Server
{

	public static void main(String args[])
	{
		try
		{
			System.out.print("Enter your name: ");
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String name = br.readLine();
			
			ServerSocket ss = new ServerSocket(1500);
			System.out.println("Server Loaded");
			Socket s = ss.accept();
			System.out.println("Client connecting..." );
			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
			while(true){
				
				String str  = in.readLine();
				System.out.println("From client - " +str);
				// Intermediate book: "Complete Reference"
				System.out.print(name + " - ");
				String  n = br.readLine();
				out.println(n);
			}
		}catch (Exception e)
		{
			System.out.println("Connection lost...");
		}
	}

}