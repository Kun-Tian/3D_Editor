#include "profile.h"

int strkv(char *src, char *key, char *value)
{
	char *p, *q;
	p = strchr(src, '=');	// p找到等号
	q = strchr(src, '\n');	// q找到换行

	// 如果有等号有换行
	if (p != NULL && q != NULL)
	{
		*q = '\0'; // 将换行设置为字符串结尾
		strncpy_s(key, 50, src, p - src); // 将等号前的内容拷贝到 key 中
		strcpy_s(value, 50, p + 1);	// 将等号后的内容拷贝到 value 中
		return 1;
	}
	else
	{
		return 0;
	}
}

bool ctb(const char *a){
	if (a[0] == 'T')return true;
	if (a[0] == 'F')return false;
}

profile *read_settings(const char *filename){
	profile *p = new profile;
	FILE *fd = new FILE;
	fopen_s(&fd, filename, "r");
	if (fd == NULL)return p;
	char buf[256] = "";
	char key[50] = "";	// 配置变量名
	char value[50] = "";	// 配置变量值	
	while (fgets(buf, 256, fd))
	{
		if (strkv(buf, key, value)){
			if (strcmp(key, "Quality") == 0){
				if (strcmp(value, "Low") == 0) p->Quality = 1;
				if (strcmp(value, "Standard") == 0) p->Quality = 2;
				if (strcmp(value, "High") == 0) p->Quality = 3;
			}
			if (strcmp(key, "Raft") == 0) p->Raft = ctb(value);
			if (strcmp(key, "Support") == 0) p->Support = ctb(value);
			if (strcmp(key, "LayerHeight") == 0) strcpy_s(p->LayerHeight, value);
			if (strcmp(key, "Infill") == 0) strcpy_s(p->Infill, value);
			if (strcmp(key, "NumofShells") == 0) strcpy_s(p->NumofShells, value);
			if (strcmp(key, "Temperature") == 0) strcpy_s(p->Temperature, value);
			if (strcmp(key, "ModelHeight") == 0) strcpy_s(p->ModelHeight, value);
		}
		memset(key, 0, strlen(key));
	}
	fclose(fd);
	return p;
}