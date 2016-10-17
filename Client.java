import java.io.*;
import java.net.*;

class Client
{
	public static void main(String args[])
	{
		try {
			// BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			Socket sock = new Socket("127.0.0.1", 1500);
			
			BufferedReader kRead = new BufferedReader(new InputStreamReader(System.in));
			System.out.print("Enter your  name: ");
			String name = kRead.readLine();
			PrintWriter out = new PrintWriter(sock.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			
			while(true)
			{
				System.out.print(name + " - "); 
				String c_cmd = kRead.readLine();
				out.println(c_cmd);
				
				String s_cmd = in.readLine();
				System.out.println("Server - " +s_cmd);
			}
		} catch (Exception e)
		{
			System.out.println("Connection lost...");
		}
		
	}
}