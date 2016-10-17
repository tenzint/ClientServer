import java.io.*;
import java.net.*;
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
				this.yield();
				System.out.print(name + " - ");
				n = br.readLine();
				out.println(n);
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
				System.out.println();
				System.out.println("From server - " +str);
				System.out.print(name + " - ");
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
			Socket s = new Socket("127.0.0.1", 1500);
			
			Scanner sc = new Scanner(System.in);
			System.out.print("Enter your first name: ");
			String name = sc.next();
			
			SendMsg sMsg = new SendMsg(name, s);
			RecvMsg rMsg = new RecvMsg(name, s);
			
			System.out.print(name + " - ");
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