import java.io.*;
import java.net.*;
import java.util.*;
// thread class under java.lang


class SendMsg extends Thread
{
	String name;
	Socket s;
	SendMsg(String _name, Socket _s)
	{
		this.name = _name;
		this.s = _s;
	}
	public void run()
	{
		try{
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			String n;
			while(true)
			{
				System.out.print(name + " - ");
				n = br.readLine();
				out.println(n);
				this.yield();
			}	
		} catch(Exception e)
		{
			System.out.println(" * * * Unable to send message * * * ");
		}
	}
	
}
class RecvMsg extends Thread
{
	String name;
	Socket s;
	RecvMsg(String _name, Socket _s)
	{
		this.name = _name;
		this.s = _s;
	}
	public void run()
	{
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
			String str;
			while(true)
			{
				this.yield();
				str  = in.readLine();
				
				System.out.println(str);
			}	
		} catch (Exception e)
		{
			System.out.println(" * * * Unable to receive message * * * ");
		}
	}
}
class MClient
{
	public static void main(String args[])
	{
		try
		{
			// BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			Socket s = new Socket("127.0.0.1", 1501);
			
			Scanner sc = new Scanner(System.in);
			System.out.print("Enter your first name: ");
			String name = sc.next();
			sc.close();				// close the Scanner... we use buffered reader from now on
			
			System.out.println("\n\n before sending name to server\n" );
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			out.println(name);					// send client's name. Server expects client's name once!
			
			System.out.println("\n\n after sending name to server\n" );
			SendMsg sMsg = new SendMsg(name, s);
			RecvMsg rMsg = new RecvMsg(name, s);
			
			while(true)
			{
				sMsg.start();
				rMsg.start();
			}		
		} catch (Exception e)
		{
			// doing nothing, main exception usually only in runnable thread classes
			// Server might not be set up yet though.
			System.out.println(" * * * * * * * * * * * * * * * * * * * * * * ");
		}
	}
}