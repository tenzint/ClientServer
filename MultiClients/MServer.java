import java.io.*;
import java.net.*;
import java.util.*;
/**
 * 
 *
 */
 
// Don't need RecvMsg, only SendMsg class is enough. Keeping the code in case we need it
/*
class RecvMsg extends Thread
{
	MetaData md;					// md.name & md.socket
	RecvMsg(MetaData _md)
	{
		md = _md;
	}
	public void run()
	{
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(md.s.getInputStream()));
			
			String str;
			while(true)
			{
				str  = in.readLine();
				System.out.println();
				System.out.println(md.name + " - " +str);
				// okay, client's message printed on server
				// now print the client's message to every client
				MetaData cMd;
				for(int i=0; i<MServer.cList.size(); i++)
				{
					cMd = MServer.cList.get(i);
					PrintWriter out = new PrintWriter(cMd.s.getOutputStream(), true);
			
					out.println(md.name + " - " + str);
				}
				this.yield();
			}	
		} catch (Exception e)
		{
			System.out.println(" * * * Unable to receive message * * * ");
		}
	}
}
 */
class s_SendMsg extends Thread
{
	MetaData md;					// md.name & md.socket
	s_SendMsg(MetaData _md)
	{
		md = _md;
	}
	public void run()
	{
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(md.s.getInputStream()));
			
			String str;
			while(true)
			{
				str  = in.readLine();
				
				System.out.println();
				System.out.println(md.name + " - " +str);
				// okay, client's message printed on server
				// now print the client's message to every client
				MetaData cMd;
				for(int i=0; i<MServer.cList.size(); i++)
				{
					cMd = MServer.cList.get(i);
					PrintWriter out = new PrintWriter(cMd.s.getOutputStream(), true);
			
					out.println(md.name + " - " + str);
				}
				this.yield();
			}	
		} catch (Exception e)
		{
			System.out.println(" * * * Unable to receive message * * * ");
		}
	}
}


class Serve extends Thread
{
	MetaData md;
	ServerSocket ss;
	Socket s;				// s initialized in run() method, not in constructor
	Serve(ServerSocket _ss)
	{
		md = new MetaData();
		ss = _ss;
	}
	Serve(MetaData _md, ServerSocket _ss)
	{
		md = _md;
		ss = _ss;
	}
	public void run()
	{
		try
		{
			s = ss.accept();				// wait until a new client connects to server
			BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
			md.name = in.readLine();
			md.s = s;
			// Assuming the first line from client is always client's name/ID
			// This idea is established in MClient.java
			System.out.println(md.name + " connecting..." );
			
			MServer.cList.add(md);						// add all incoming client's metadata(name, and socket) to the list
			
			Serve next_srv = new Serve(ss);
			
			next_srv.start();			// start a new thread to 'prepare' for any new client that may connect to server
			
			s_SendMsg sMsg = new s_SendMsg(md);
			
			//RecvMsg rMsg = new RecvMsg(md);
			
			while(true)
			{
				sMsg.start();
				//rMsg.start();
			}
		} catch (Exception e)
		{
			
		}
	}
}

class MetaData
{
	String name;
	Socket s;
	MetaData()
	{
		
	}
	MetaData(String _name, Socket _s)
	{
		this.name = _name;
		this.s = _s;
	}
}
class MServer
{
	// Class type linked list that stores info of all the client's sockets
	// called using "MServer.cList"
	static List<MetaData> cList = Collections.synchronizedList(new LinkedList<MetaData>());
	public static void main(String args[])
	{
		try
		{
			MetaData md = new MetaData();
			ServerSocket ss = new ServerSocket(1502);
			System.out.println("Server Loaded");
			
			Serve srv = new Serve(ss);
			srv.start();
			/////////////////////////////////////////
			// accept sockets in a new thread..
			
			while(true)
			{
				// do nothing
			}
		} catch (Exception e)
		{
			// doing nothing, main exception usually only in runnable thread classes
			System.out.println(" ------------------------------------------------------- ");
		}
	}

}