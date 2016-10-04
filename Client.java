import java.io.*;
import java.net.*;

class Client
{
	public static void main(String args[])
	{
		String hostName = args[0];
		int portNumber = Integer.parseInt(args[1]);
		String fromServer="";
		try {
			Socket sock = new Socket(hostName, portNumber);
			
			BufferedReader kRead = new BufferedReader(new InputStreamReader(System.in));
			PrintWriter out = new PrintWriter(sock.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			
			while(true)
			{
				String c_cmd = kRead.readLine();
				out.println(c_cmd);
				
				String s_cmd = in.readLine();
				System.out.println("Server :: " +s_cmd);
			}
		} catch (Exception e)
		{
			System.out.println("Connection lost...");
		}
		
	}
}