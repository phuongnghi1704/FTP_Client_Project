#include "ftp_client.h"


/*Initialize ftp_client
> Create new socket
> Set tranmission mode to active as default*/
ftp_client::ftp_client()
{
	_pSocket = new CSocket;
	memset(buf, 0, BUFSIZE);
	_pSocket->Create();
	isPasv = 0;
}

/*Process user interface
> Receive user commands
> Make function calls*/
void ftp_client::UserHandler()
{
	char command[STR_LENGTH];
	this->Login(_T("127.0.0.1"));
	do
	{
		cout << "ftp> ";
		cin >> command;
		if (command == NULL) continue;
		int flag = this->CommandHandler(command);
		switch (flag)
		{
		case CODE_ERROR:
			cout << "Command invalid!\n";
			continue;
			break;
		case CODE_LOGIN:
			this->_pSocket->Close();
			this->_pSocket->Create();
			this->Login();
			break;
		case CODE_LS:
			this->Ls();
			break;
		case CODE_CD:
			this->Cd();
			break;
		case CODE_LCD:
			this->Lcd();
			break;
		case CODE_DIR:
			this->Dir();
			break;
		case CODE_GET:
			this->Get();
			break;
		case CODE_PUT:
			this->Put();
			break;
		case CODE_MGET:
			this->MGet();
			break;
		case CODE_MPUT:
			this->MPut();
			break;
		case CODE_DELETE:
			this->Del();
			break;
		case CODE_MDELETE:
			this->MDel();
			break;
		case CODE_MKDIR:
			this->Mkdir();
			break;
		case CODE_RMDIR:
			this->Rmdir();
			break;
		case CODE_PWD:
			this->Pwd();
			break;
		case CODE_PASSIVE:
			this->Passive();
			break;
		case CODE_ACTIVE:
			this->Active();
			break;
		case CODE_QUIT:
			this->Quit();
			return;
		}
	} while (TRUE);
}

/*Process user commands
> return CODE of each command*/
int ftp_client::CommandHandler(char command[])
{

	if (!strcmp(command, "login"))
		return CODE_LOGIN;
	if (!strcmp(command, "ls"))
		return CODE_LS;
	if (!strcmp(command, "dir"))
		return CODE_DIR;
	if (!strcmp(command, "cd"))
		return CODE_CD;
	if (!strcmp(command, "lcd"))
		return CODE_LCD;
	if (!strcmp(command, "get"))
		return CODE_GET;
	if (!strcmp(command, "put"))
		return CODE_PUT;
	if (!strcmp(command, "mget"))
		return CODE_MGET;
	if (!strcmp(command, "mput"))
		return CODE_MPUT;
	if (!strcmp(command, "delete"))
		return CODE_DELETE;
	if (!strcmp(command, "mdelete"))
		return CODE_MDELETE;
	if (!strcmp(command, "mkdir"))
		return CODE_MKDIR;
	if (!strcmp(command, "rmdir"))
		return CODE_RMDIR;
	if (!strcmp(command, "pwd"))
		return CODE_PWD;
	if (!strcmp(command, "passive"))
		return CODE_PASSIVE;
	if (!strcmp(command, "active"))
		return CODE_ACTIVE;
	if (!strcmp(command, "quit"))
		return CODE_QUIT;
	return CODE_ERROR;
}

/*Connect to server*/
bool ftp_client::Connect(wchar_t IPAddr[])
{
	if (_pSocket->Connect(IPAddr, PORT) != 0)
	{
		cout << "Successfully connect to FTP Server!\n";
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*Login to FTP server*/
bool ftp_client::Login(wchar_t IPAddr[STR_LENGTH])
{
	if (IPAddr == NULL)
	{
		IPAddr = new wchar_t[STR_LENGTH];
		cout << "To: ";
		wcin >> IPAddr;
	}
	if (this->Connect(IPAddr) == FALSE)
	{
		return FALSE;
	}
	else
	{
		int tmpresult;
		int codeftp;
		char* str;
		memset(buf, 0, BUFSIZE);
		while ((tmpresult = _pSocket->Receive(buf, BUFSIZ, 0)) > 0)
		{
			sscanf(buf, "%d", &codeftp);
			cout << buf;
			if (codeftp != 220) //120, 240, 421: something wrong
			{
				return FALSE;
			}

			str = strstr(buf, "220");//DK dung
			if (str != NULL) {
				break;
			}
			memset(buf, 0, tmpresult);
		}

		//Send username
		char username[50];
		cout << "User name: ";
		cin >> username;

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "USER %s\r\n", username);
		if (this->SendCommand() == FALSE)
			return FALSE;

		
		sscanf(buf, "%d", &codeftp);
		if (codeftp != 331)
		{
			return FALSE;
		}

		//Send password
		char password[50];
		cout << "Password: ";
		cin >> password;

		memset(buf, 0, BUFSIZE);
		sprintf(buf, "PASS %s\r\n", password);
		if (this->SendCommand() == FALSE)
			return FALSE;
		
		sscanf(buf, "%d", &codeftp);
		if (codeftp != 230)
		{
			return FALSE;
		}
	}
	return TRUE;
	delete IPAddr;
}

/*List file from selected directory on server
> Open dataBuffer.txt. This buffer is to receive data from server
> Prepare NLST command
> Check if tranmission mode is active or passive then use dataTvAM of dataTvPM
> Put data from dataBuffer to specific file with the right file type*/
bool ftp_client::Ls()
{
	/*if (dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "NLST\r\n");
	if (isPasv == 0)
	{
		if (!this->dataTransfActM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTransfPasvM(0)) return FALSE;
	}

	dBuffer.clear();
	dBuffer.seekg(0, ios::beg);
	stringstream buffer;
	buffer << dBuffer.rdbuf();
	string contents(buffer.str());
	cout << contents;

	dBuffer.close();
	return TRUE;*/
	if(isPasv==0)
	ListActM("ls");
	else
	ListPasvM("ls");

	return true;
}

bool ftp_client::Dir()
{
	/*if (dBuffer.is_open() == FALSE)
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	else
	{
		dBuffer.close();
		dBuffer.open("dataBuffer.txt", ios::in | ios::out | ios::trunc | ios::binary);
	}
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "LIST\r\n");
	if (isPasv == 0)
	{
		if (!this->dataTransfActM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTransfPasvM(0)) return FALSE;
	}
	dBuffer.clear();
	dBuffer.seekg(0, ios::beg);
	stringstream buffer;
	buffer << dBuffer.rdbuf();
	string contents(buffer.str());
	cout << contents;

	dBuffer.close();
	return TRUE;*/
	if (isPasv == 0)
		ListActM("dir");
	else
		ListPasvM("dir");
	return true;
	
	
}

bool ftp_client::Put(char src_filename[STR_LENGTH], char dest_filename[STR_LENGTH])
{
	f.open("dataBuffer.dat", ios::in | ios::out | ios::trunc | ios::binary);

	if (src_filename == NULL)
	{
		src_filename = new char[STR_LENGTH];
		memset(src_filename, 0, sizeof src_filename);
		cout << "Local file: ";
		cin >> src_filename;
	}
	if (dest_filename == NULL)
	{
		dest_filename = new char[STR_LENGTH];
		memset(dest_filename, 0, sizeof dest_filename);
		cout << "Remote file: ";
		cin >> dest_filename;
	}

	fstream fout;
	fout.open(src_filename, ios::in | ios::binary);

	int filesize;
	fout.seekg(0, ios::end);
	filesize = fout.tellg();

	fout.clear();
	fout.seekg(0, ios::beg);

	char buff[BUFSIZ + 1];
	memset(buff, 0, sizeof buff);
	while (TRUE)
	{
		if (filesize <= 512)
		{
			fout.read(buff, filesize);
			f.write(buff, filesize);
			break;
		}
		fout.read(buff, BUFSIZ);
		f.write(buff, BUFSIZ);
		filesize = filesize - 512;
		memset(buff, 0, sizeof buff);
	}
	cout << "Filesize " << f.tellg() << " bytes\n";
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "STOR %s\r\n", dest_filename);
	if (isPasv == 0)
	{
		if (!this->dataTransfActM(1)) return FALSE;
	}
	else
	{
		if (!this->dataTransfPasvM(1)) return FALSE;
	}

	fout.close();

	f.close();
	return TRUE;
}

bool ftp_client::Get(char src_filename[STR_LENGTH], char des_filename[STR_LENGTH])
{
	if (f.is_open() == FALSE)
	{
		f.open("dataBuffer.dat", ios::in | ios::out | ios::trunc | ios::binary);
	}
	else
	{
		f.close();
		f.open("dataBuffer.dat", ios::in | ios::out | ios::trunc | ios::binary);
	}
	if (src_filename == NULL)
	{
		src_filename = new char[STR_LENGTH];
		memset(src_filename, 0, sizeof src_filename);
		cout << "Source filename: ";
		cin >> src_filename;
	}
	if (des_filename == NULL)
	{
		des_filename = new char[STR_LENGTH];
		memset(des_filename, 0, sizeof des_filename);
		cout << "Destination filename: ";
		cin >> des_filename;
	}
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "RETR %s\r\n", src_filename);
	if (isPasv == 0)
	{
		if (!this->dataTransfActM(0)) return FALSE;
	}
	else
	{
		if (!this->dataTransfPasvM(0)) return FALSE;
	}
	fstream fin;
	fin.open(des_filename, ios::out | ios::binary | ios::trunc);

	f.clear();
	f.seekg(0, ios::beg);

	fin <<f.rdbuf();
	fin.flush();

	fin.close();

	f.clear();
	f.close();
	return TRUE;
}

bool ftp_client::MPut(char filenames[STR_LENGTH])
{
	char* filename[8];

	if (filenames == NULL)
	{
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Local Files: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 8)
	{
		filename[i] = strtok(NULL, " ,-");
		if (filename[i] == NULL) break;
		i++;
	}

	for (int j = 0; j < i; j++)
	{
		char dst_filename[STR_LENGTH];
		cout << "Remote file for " << filename[j] << ": ";
		cin >> dst_filename;
		this->Put(filename[j], dst_filename);
	}
	delete filenames;
	return FALSE;
}

bool ftp_client::MGet(char filenames[STR_LENGTH])
{
	char* filename[8];

	if (filenames == NULL)
	{
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Filenames: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 8)
	{
		filename[i] = strtok(NULL, " ,-");
		if (filename[i] == NULL) break;
		i++;
	}

	for (int j = 0; j < i; j++)
	{
		char dst_filename[STR_LENGTH];
		cout << "Destination file for " << filename[j] << ": ";
		cin >> dst_filename;
		this->Get(filename[j], dst_filename);
	}
	delete filenames;
	return FALSE;
}

bool ftp_client::Del(char filename[STR_LENGTH])
{
	if (filename == NULL)
	{
		filename = new char[STR_LENGTH];
		memset(filename, 0, sizeof filename);
		cout << "Filename: ";
		cin >> filename;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "DELE %s\r\n", filename);
	if (this->SendCommand() == FALSE)
		return FALSE;
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		return FALSE;
	}
	delete filename;
	return TRUE;
}

bool ftp_client::MDel(char filenames[STR_LENGTH])
{
	char* filename[8];

	if (filenames == NULL)
	{
		//char filenames[STR_LENGTH];
		filenames = new char[STR_LENGTH];
		cin.ignore();
		cout << "Filenames: ";
		cin.getline(filenames, 256);
	}

	filename[0] = strtok(filenames, " ,-");

	int i = 1;
	while (i < 8)
	{
		filename[i] = strtok(NULL, " ,-");
		if (filename[i] == NULL) break;
		i++;
	}

	for (int j = 0; j < i; j++)
	{
		char permission;
		cout << "Delete " << filename[j] << "? (Y/n): ";
		permission = _getch();
		if (permission == 'Y' || permission == 'y')
		{
			memset(buf, 0, BUFSIZE);
			sprintf(buf, "DELE %s\r\n", filename[j]);
			cout << endl;
			if (this->SendCommand() == FALSE) continue;
			int codeftp;
			sscanf(buf, "%d", &codeftp);
			if (codeftp != 250)
			{
				return FALSE;
			}
			memset(buf, 0, BUFSIZE);
		}
		else
		{
			continue;
		}
	}
	delete filenames;
	return TRUE;
}

bool ftp_client::Cd(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "CWD %s\r\n", directory);
	if (this->SendCommand() == FALSE) return FALSE;
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		return FALSE;
	}
	return TRUE;
}

bool ftp_client::Lcd(char directory[STR_LENGTH])
{
	char dirbuf[BUFSIZ + 1];
	if (directory == NULL)
	{
		_getcwd(dirbuf, sizeof dirbuf);
		cout << dirbuf << endl;

		directory = new char[BUFSIZ];
		cout << "Directory ";
		cin >> directory;
		_chdir(directory);
		_getcwd(dirbuf, sizeof dirbuf);
	}

	cout << dirbuf << endl;
	return TRUE;
}

bool ftp_client::Mkdir(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory name ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "MKD %s\r\n", directory);
	if (this->SendCommand() == FALSE) return FALSE;
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 257)
	{
		return FALSE;
	}
	return TRUE;

}

bool ftp_client::Rmdir(char directory[STR_LENGTH])
{
	if (directory == NULL)
	{
		directory = new char[STR_LENGTH];
		cout << "Directory name ";
		cin >> directory;
	}

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "RMD %s\r\n", directory);
	if (this->SendCommand() == FALSE) return FALSE;
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		return FALSE;
	}
	return TRUE;

}

bool ftp_client::Pwd()
{
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "PWD\r\n");
	if (this->SendCommand() == FALSE) return FALSE;
	int codeftp;
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		return FALSE;
	}
	return TRUE;

}

void ftp_client::Passive()
{
	this->isPasv = 1;
}

void ftp_client::Active()
{
	this->isPasv = 0;
}

bool ftp_client::Quit()
{
	sprintf(buf, "QUIT\r\n");
	if (this->SendCommand() == FALSE)
		return FALSE;
	return TRUE;
}

ftp_client::~ftp_client()
{
	_pSocket->Close();
	delete _pSocket;
}

bool ftp_client::SendCommand()
{
	if (_pSocket->Send(buf, BUFLEN, 0) == 0)
		return FALSE;
	memset(buf, 0, BUFSIZE);
	if (_pSocket->Receive(buf, BUFSIZ, 0) == 0)
		return FALSE;
	cout << buf;
	return TRUE;
}

bool ftp_client::dataTransfActM(bool stream)
{
	CSocket dSocket, serverConnect;
	CString dtIPAddr; UINT dtPort;
	//lưu lệnh thực thi
	char t_buf[BUFSIZ + 1];
	sprintf(t_buf, "%s", buf);
	//kết nối dSocket đến server, xử lý port và gửi lệnh cho server
	if (dSocket.Create(0U, SOCK_STREAM, _T("127.0.0.1")) == 0) return FALSE;
	if (dSocket.Listen() == 0) return FALSE;

	if (dSocket.GetSockName(dtIPAddr, dtPort) == 0) return FALSE;
	int x = dtPort / 256;
	int y = dtPort % 256;

	memset(buf, 0, BUFSIZE);
	sprintf(buf, "PORT 127,0,0,1,%d,%d\r\n", x, y);
	this->SendCommand();
	//lấy ra lệnh thực thi và gửi
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "%s", t_buf);
	this->SendCommand();

	memset(buf, 0, BUFSIZE);
	//chấp nhận kết nối
	dSocket.Accept(serverConnect);

	if (stream == 0)
	{
		int buf_count = serverConnect.Receive(buf, BUFSIZ, 0);
		while (buf_count != 0)
		{
			f.write(buf, buf_count);
			memset(buf, 0, BUFSIZE);
			buf_count = serverConnect.Receive(buf, BUFSIZ, 0);
		}
	}
	else
	{
		int filesize = f.tellg();
		f.clear();
		f.seekg(0, ios::beg);
		memset(buf, 0, BUFSIZE);
		while (filesize != 0)
		{

			if (filesize <= 512)
			{
				f.read(buf, filesize);
				serverConnect.Send(buf, filesize, 0);
				break;
			}
			f.read(buf, BUFSIZ);
			serverConnect.Send(buf, BUFSIZ, 0);
			filesize = filesize - 512;
			memset(buf, 0, BUFSIZE);
		}
	}

	serverConnect.Close();
	dSocket.Close();

	memset(buf, 0, BUFSIZE);

	if((_pSocket->Receive(buf, BUFSIZ, 0)==0)) return false;

	cout << buf;
	return TRUE;
}

bool ftp_client::dataTransfPasvM(bool stream)
{
	CSocket dSocket;
	int IPAddr[4];
	int Port[2];

	//lưu lệnh thực thi
	char t_buf[BUFSIZ + 1];	
	sprintf(t_buf, "%s", buf);

	//gửi lệnh PASV
	sprintf(buf, "PASV\r\n");
	this->SendCommand();

	sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", &IPAddr[0], &IPAddr[1], &IPAddr[2], &IPAddr[3], &Port[0], &Port[1]);
	//tạo port và kết nối với server
	int nPort = Port[0] * 256 + Port[1];

	dSocket.Create();
	dSocket.Connect(_T("127.0.0.1"), nPort);

	//lấy lệnh thực thi gửi cho server
	memset(buf, 0, BUFSIZE);
	sprintf(buf, "%s", t_buf);
	this->SendCommand();

	if (stream == 0)
	{
		int buf_count = dSocket.Receive(buf, BUFSIZ, 0);
		while (buf_count != 0)
		{
			f.write(buf, buf_count);
			memset(buf, 0, BUFSIZE);
			buf_count = dSocket.Receive(buf, BUFSIZ, 0);
		}
	}
	else
	{
		int filesize =f.tellg();
		f.clear();
		f.seekg(0, ios::beg);
		memset(buf, 0, BUFSIZE);
		while (filesize != 0)
		{
			if (filesize <= 512)
			{
				f.read(buf, filesize);
				dSocket.Send(buf, filesize, 0);
				break;
			}
			f.read(buf, BUFSIZ);
			dSocket.Send(buf, BUFSIZ, 0);
			filesize = filesize - 512;
			memset(buf, 0, BUFSIZE);
		}
	}
	dSocket.Close();

	memset(buf, 0, BUFSIZE);
	_pSocket->Receive(buf, BUFSIZ, 0);
	cout << buf;

	return TRUE;
}

bool ftp_client::ListActM(char str[STR_LENGTH])
{
	CSocket dSocket, serverConnect; //kenh data
	CString dIPAddr; unsigned int dPort;
	if (dSocket.Create(0U, SOCK_STREAM, _T("127.0.0.1")) == 0) return FALSE;
	if (dSocket.Listen() == 0) return FALSE;

	if (dSocket.GetSockName(dIPAddr, dPort) == 0) return FALSE;
	int x = dPort / 256;
	int y = dPort % 256;
	

	sprintf(buf, "PORT 127,0,0,1,%d,%d\r\n", x, y);
	int size =_pSocket->Send(buf, BUFSIZ, 0);

	memset(buf, '\0', BUFSIZE);
	size = _pSocket->Receive(buf, BUFSIZ, 0);
	int codeReply;
	sscanf(buf, "%d", &codeReply);

	if (codeReply != 200) //"Port command NOT successful"
	{
		return false;
	}

	cout << buf << endl;

	memset(buf, '\0', size);
	
		if (str == "ls")
			sprintf(buf, "NLST\r\n");
	else if (str == "dir")
			sprintf(buf, "LIST\r\n");

	size = _pSocket->Send(buf, BUFSIZ, 0);
	memset(buf, '\0', size);

	size = _pSocket->Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeReply);
	if (codeReply != 150) //cannot open data channel for directory listing of 
	{
		return false;
	}
	cout << buf << endl;

	//Chap nhan ket noi
	dSocket.Accept(serverConnect);

	memset(buf, '\0', BUFSIZE);
		int buf_count;
		while (buf_count = serverConnect.Receive(buf, BUFSIZ) > 0)
		{
			cout << buf;
			memset(buf, 0, BUFSIZE);
		}

	//Reply cua Server
	size = _pSocket->Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeReply);
	if (codeReply != 226) //226 Unsuccessful transferred 
	{
		return false;
	}
	cout << buf << endl;
	memset(buf, '\0', size);

	serverConnect.Close();
	dSocket.Close();

	return true;
}

bool ftp_client::ListPasvM(char str[STR_LENGTH])
{
	CSocket dSocket;
	int IPAddr[4];
	int Port[2];

	//gửi lệnh PASV
	sprintf(buf, "PASV\r\n");
	this->SendCommand();

	sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", &IPAddr[0], &IPAddr[1], &IPAddr[2], &IPAddr[3], &Port[0], &Port[1]);
	//tạo port và kết nối với server
	int nPort = Port[0] * 256 + Port[1];

	dSocket.Create();
	dSocket.Connect(_T("127.0.0.1"), nPort);

	memset(buf, '\0', BUFSIZE);

	if (str == "ls")
		sprintf(buf, "NLST\r\n");
	else if (str == "dir")
		sprintf(buf, "LIST\r\n");

	int size=_pSocket->Send(buf, BUFLEN, 0);

	memset(buf, '\0', size);

	size = _pSocket->Receive(buf, BUFSIZ, MSG_PEEK);

	int codeReply;
	sscanf(buf, "%d", &codeReply);
	if (codeReply != 150) //cannot open data channel for directory listing of 
	{
		return false;
	}
	cout << buf << endl;

	//Chap nhan ket noi

	memset(buf, '\0', BUFSIZE);
	int buf_count;
	while (buf_count = dSocket.Receive(buf, BUFSIZ) > 0)
	{
		cout << buf;
		memset(buf, 0, BUFSIZE);
	}

	//Reply cua Server
	size = _pSocket->Receive(buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeReply);
	if (codeReply != 226) //226 Unsuccessful transferred 
	{
		return false;
	}
	cout << buf << endl;
	memset(buf, '\0', size);

	dSocket.Close();

	return true;
}
