#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>

#define bufferSize 256

int main()
{
	char buffer[bufferSize];

	int fd;

	fd= open("/dev/mydevShaishav", O_RDONLY);
	if (fd < 0)
	{
		perror("ERROR in opening device");
		return 0;
	}

	int bytes_read ;
	while(bytes_read = read(fd, buffer, strlen(buffer)) > 0)
	{
		printf("%s\n", buffer);
		memset(buffer, 0, sizeof(char)*bufferSize);
	}
	if(bytes_read < 0)
	{
		perror("ERROR in reading from device");
		return 0;
	}
	close(fd);

	return 0;
}