#include <iostream>
#include "winsock2.h"
#include <string.h>
#include<stdio.h>
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable : 4996)
struct sinhvien {
	char username[50];
	char password[50];
	char fullname[50];
	char id[10];
};
const char *adminuser = "admin";
const char *adminpass = "admin";
struct sinhvien danhsach[50];
int num;
void parseJSON(char *buf, int numParam, char info[6][64]) {
	printf("\n===%s\n===", buf);
	int k;
	//lay vi tri bat dau cua chuoi json gui ve
	for (k = strlen(buf); k > 0; k--) {
		if (buf[k] == '\n') {
			break;
		}
	}
	//tach chuoi json
	char json[128];
	int m = 0;
	for (int h = k + 1; h < strlen(buf); h++) {
		json[m] = buf[h];
		m++;
		json[m] = 0;
	}
	//loai bo ki tu dac biet
	for (int h = 0; h < strlen(json); h++) {
		if (json[h] == '&' || json[h] == '=') {
			json[h] = ' ';
		}
	}
	//lay tung mang gia tri
	char tmp[256];
	//char** info = (char **)malloc(numParam * sizeof(char *));
	for (int i = 0; i < numParam; i++) {
		//info[i] = (char *)malloc(sizeof(char) * 64);
		sscanf(json, "%s %s", tmp, info[i]);
		char json_tmp[256];
		strncpy(json_tmp, json + strlen(tmp) + strlen(info[i]) + 2,
			strlen(json + strlen(tmp) + strlen(info[i]) + 2));
		strcpy(json, json_tmp);
	}
}
void output_file() {
	FILE *file;
	int line = 1;
	num = 0;
	file = fopen("users.txt", "r");
	if (file == NULL)
		printf("Can  not open file!!!\n");
	else if (file != NULL)
	{
		do {
			char buff[100];
			fgets(buff, sizeof(buff), file);
			//xoa ky tu xuong dong
			int ret = strlen(buff);
			buff[ret - 1] = 0;

			if (line % 4 == 1) {
				strcpy(danhsach[num].username, buff);
				line++;
			}
			else if (line % 4 == 2) {
				strcpy(danhsach[num].password, buff);
				line++;
			}
			else if (line % 4 == 3) {
				strcpy(danhsach[num].fullname, buff);
				line++;
			}
			else if (line % 4 == 0)
			{
				strcpy(danhsach[num].id, buff);
				line++;
				num++;
			}
		} while (!feof(file));
	}
	fclose(file);
}
void input_file() {
	FILE *file = fopen("users.txt", "r+");
	int k;
	for (k = 0;k < num;k++) {
		fprintf(file, "%s\n", danhsach[k].username);
		fprintf(file, "%s\n", danhsach[k].password);
		fprintf(file, "%s\n", danhsach[k].fullname);
		fprintf(file, "%s\n", danhsach[k].id);
	}
	fclose(file);
}
DWORD WINAPI ClientThread(LPVOID lpParam);
int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8080);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
	closesocket(listener);
	WSACleanup();
	return 0;
}
DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET *)lpParam;
	char buf[1024];
	int ret;
	//while (1) {
		// Nhan yeu cau tu trinh duyet
		ret = recv(client, buf, sizeof(buf), 0);
		/*if (ret <= 0) {
			break;
		}*/
		buf[ret] = 0;
		//GET
		if (strncmp(buf + 4, "/xinchao", 8) == 0)
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			char sendbuf[256];
			char tmp[256];
			FILE *f = fopen("client.html", "rb");
			//fgets(sendbuf, 255, (FILE *)f);
			while (1)
			{
				ret = fread(sendbuf, 1, sizeof(sendbuf), f);

				if (ret > 0)
					send(client, sendbuf, ret, 0);
				else
					break;
			}
			fclose(f);
		}
		else if (strncmp(buf + 5, "/savenote", 8) == 0) {
			CreateDirectoryA("history", NULL);
			char info[6][64];
			parseJSON(buf, 3, info);// username, ip, time
			char filepath[256];
			strcpy(filepath, "history\\");
			sprintf(filepath, "%s%s%s", filepath, info[0], ".txt");
			FILE *f = fopen(filepath, "a");
			char printbuf[256];
			sprintf(printbuf, "%s%s%s%s", info[2], "\n", info[1], "\n");
			fputs(printbuf, f);
		}
		//get
		else if (strncmp(buf + 4, "/register", 8) == 0)
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			char sendbuf[1024];
			FILE *f = fopen("signup.txt", "rb");

			while (1)
			{
				ret = fread(sendbuf, 1, sizeof(sendbuf), f);

				if (ret > 0)
					send(client, sendbuf, ret, 0);
				else
					break;
			}

			fclose(f);

		}
		else if (strncmp(buf + 5, "/signup", 7) == 0)
		{
#pragma region getJSON-Data
			printf("%s", buf);
			int k;
			for (k = strlen(buf); k > 0; k--) {
				if (buf[k] == '\n') {
					break;
				}
			}
			char json[128];
			int m = 0;
			for (int h = k + 1; h < strlen(buf); h++) {
				json[m] = buf[h];
				m++;
				json[m] = 0;


#pragma endregion
#pragma region seperateData
				for (int h = 0; h < strlen(json); h++) {
					if (json[h] == '&' || json[h] == '=') {
						json[h] = ' ';
					}
				}
			}


			// khai bao struct de luu thong tin sinh vien

			char name[64], nameValue[64], pass[64], passValue[64], fullname[64], fullnameValue[64], id[64], idValue[64];
			sscanf(json, "%s %s %s %s %s %s %s %s", name, nameValue, pass, passValue, fullname, fullnameValue, id, idValue);
			for (int index = 0; index < strlen(fullnameValue); index++) {
				if (fullnameValue[index] == '+') fullnameValue[index] == ' ';
			}

			char file[1024], buf_search[64];
			const char *success = "1";
			FILE *f = fopen("users.txt", "a+");
			if (f == NULL) printf("\n File khong ton tai!");
			else {
				fseek(f, 0, SEEK_END);
				int size = ftell(f);
				if (size == 0) { // neu file chua ton tai
					fprintf(f, "%s\n%s\n%s\n%s\n", nameValue, passValue, fullnameValue, idValue);
					const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
					send(client, header, strlen(header), 0);
					send(client, success, strlen(success), 0);
				}
				else { // neu file da ton tai
					int found = 0;
					fseek(f, 0, SEEK_SET);
					num = 0;
					sinhvien list[50];
					int line = 1;
					do {
						char buff[100];
						fgets(buff, sizeof(buff), f);
						//xoa ky tu xuong dong
						int ret = strlen(buff);
						buff[ret - 1] = 0;

						if (line % 4 == 1) {
							strcpy(list[num].username, buff);
							line++;
						}
						else if (line % 4 == 2) {
							strcpy(list[num].password, buff);
							line++;
						}
						else if (line % 4 == 3) {
							strcpy(list[num].fullname, buff);
							line++;
						}
						else if (line % 4 == 0)
						{
							strcpy(list[num].id, buff);
							line++;
							num++;
						}
					} while (!feof(f));
					// check tai khoan ton tai chua
					for (int index = 0; index < num; index++) {
						if (strcmp(list[index].username, nameValue) == 0) {
							found = 1;
							break;
						}
					}
					if (found == 1) { // neu username co ton tai trong file
						const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
						send(client, header, strlen(header), 0);
						const char *fail = "0";
						send(client, fail, strlen(fail), 0);
					}
					if (found == 0) { // neu username khong ton tai trong file
						fseek(f, 0, SEEK_END);
						fprintf(f, "%s\n%s\n%s\n%s\n", nameValue, passValue, fullnameValue, idValue);
						const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
						send(client, header, strlen(header), 0);
						send(client, success, strlen(success), 0);
					}

				}
				fclose(f);
			}
#pragma endregion
		}
		else if (strncmp(buf + 5, "/user", 5) == 0)
		{
#pragma region getJSON-Data
			int k;
			for (k = strlen(buf); k > 0; k--) {
				if (buf[k] == '\n') {
					break;
				}
			}
			char json[128];
			int m = 0;
			for (int h = k + 1; h < strlen(buf); h++) {
				json[m] = buf[h];
				m++;
			}
			json[m] = 0;
#pragma endregion
#pragma region seperateData
			for (int h = 0; h < strlen(json); h++) {
				if (json[h] == '&' || json[h] == '=') {
					json[h] = ' ';
				}
			}
			char name[64], tmp1[64];
			sscanf(json, "%s %s", tmp1, name);
			output_file();
#pragma endregion
			// Check đăng nhập
			printf("%s\n", buf);
			bool check = false;
			int j;
			for (j = 0;j < num;j++) {
				if (strcmp(danhsach[j].username, name) == 0)
				{
					//Tìm thấy sinh viên
					check = true;
					break;
				}
			}
			if (!check) {
				// Không tìm thấy sinh viên
				const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
				send(client, header, strlen(header), 0);
				char sendbuf[256];
				char tmp[256];
				FILE *f = fopen("client.txt", "rb");
				//fgets(sendbuf, 255, (FILE *)f);
				while (1)
				{
					ret = fread(sendbuf, 1, sizeof(sendbuf), f);
					if (ret > 0)
						send(client, sendbuf, ret, 0);
					else
						break;
				}
				fclose(f);
				char script[1024] = { 0 };
				strcat(script, "<script> $(\"#error\").html(\"Unknown user\") ; $(\"#error\").css({\"color\":\"red\",\"font-size\":\"15px\"})</script>");
				send(client, script, strlen(script), 0);
			}
			else {
				//Trả thông tinc ủa sinh viên cho trình duyệt
				// Tra ket qua cho trinh duyet
				const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
				send(client, header, strlen(header), 0);
				char sendbuf[256];
				char tmp[256];
				FILE *f = fopen("information.txt", "rb");
				//fgets(sendbuf, 255, (FILE *)f);
				while (1)
				{
					ret = fread(sendbuf, 1, sizeof(sendbuf), f);
					if (ret > 0)
						send(client, sendbuf, ret, 0);
					else
						break;
				}
				fclose(f);
				char script[1024 * 5] = { 0 };
				sprintf_s(script, "<script> $(\"#username-info\").val(\"%s\");$(\"#password-info\").val(\"%s\");$(\"#fullname-info\").val(\"%s\");$(\"#id\").val(\"%s\");</script>", danhsach[j].username, danhsach[j].password, danhsach[j].fullname, danhsach[j].id);
				send(client, script, strlen(script), 0);
			}
		}
		else if (strncmp(buf + 5, "/signin", 6) == 0)
		{
#pragma region getJSON-Data
			int k;
			for (k = strlen(buf); k > 0; k--) {
				if (buf[k] == '\n') {
					break;
				}
			}
			char json[128];
			int m = 0;
			for (int h = k + 1; h < strlen(buf); h++) {
				json[m] = buf[h];
				m++;
			}
			json[m] = 0;
#pragma endregion
#pragma region seperateData
			for (int h = 0; h < strlen(json); h++) {
				if (json[h] == '&' || json[h] == '=') {
					json[h] = ' ';
				}
			}
			char name[64], tmp1[64], tmp2[64], pass[64];
			sscanf(json, "%s %s %s %s", tmp1, name, tmp2, pass);
#pragma endregion
			// Check đăng nhập
			output_file();
			printf("%s\n", buf);
			if (strcmp(name, adminuser) == 0 && strcmp(pass, adminpass) == 0) {
				const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
				send(client, header, strlen(header), 0);
				send(client, "Danh sach User", 14, 0);
				for (int i = 0;i < num;i++) {
					char bufsend[1024];
					sprintf_s(bufsend, "<p>%s</p><hr>", danhsach[i].username);
					send(client, bufsend, strlen(bufsend), 0);
				}
			}
			else {
				bool check = false;
				int j;
				for (j = 0;j < num;j++) {
					if (strcmp(danhsach[j].username, name) == 0 && strcmp(danhsach[j].password, pass) == 0)
					{
						// Đăng nhập thành công
						check = true;
						break;
					}
				}
				const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
				send(client, header, strlen(header), 0);
				const char* sucess = "1";
				const char* falied = "0";
				if (check)
					send(client, sucess, strlen(sucess), 0);// chấp nhận đăng nhập
				else
				{
					send(client, falied, strlen(falied), 0);// không chấp nhận đăng nhập
				}
			}
		}
		else if (strncmp(buf + 5, "/update", 6) == 0)
		{
#pragma region getJSON-Data
			int k;
			for (k = strlen(buf); k > 0; k--) {
				if (buf[k] == '\n') {
					break;
				}
			}
			char json[128];
			int m = 0;
			for (int h = k + 1; h < strlen(buf); h++) {
				json[m] = buf[h];
				m++;
			}
			json[m] = 0;
#pragma endregion
#pragma region seperateData
			for (int h = 0; h < strlen(json); h++) {
				if (json[h] == '&' || json[h] == '=') {
					json[h] = ' ';
				}
			}
			char name[64], tmp1[64], tmp2[64], tmp3[64], tmp4[64], pass[64], fullname[64], id[64];
			sscanf(json, "%s %s %s %s %s %s %s %s", tmp1, name, tmp2, pass, tmp3, fullname, tmp4, id);
#pragma endregion
			// Check thông tin chỉnh sửa
			int i;
			for (int l = 0;l < strlen(fullname);l++) {
				if (fullname[l] == '+') {
					fullname[l] = ' ';
				}
			}
			bool check = false;
			for (i = 0;i < num;i++) {
				if (strcmp(danhsach[i].username, name) == 0) // Tìm thấy sinh viên 
				{
					check = true;
					strcpy(danhsach[i].password, pass);
					strcpy(danhsach[i].fullname, fullname);
					strcpy(danhsach[i].id, id);
					break;
				}
			}
			input_file();
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);
			const char *done = "1";
			const char *faled = "0";
			if (check)
				send(client, done, strlen(done), 0);// chấp nhận thay đổi thông tin
			else
			{
				send(client, faled, strlen(faled), 0);// không chấp nhận thay đổi thông tin do không tìm thấy user
			}
		}
		//GET
		else
		{
			// Tra ket qua cho trinh duyet
			const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
			send(client, header, strlen(header), 0);

			const char *content = "<html><body><h1>Yeu cau khong duoc ho tro</h1></body></html>";
			send(client, content, strlen(content), 0);
		}
	//}
	// Dong ket noi
	closesocket(client);
}
