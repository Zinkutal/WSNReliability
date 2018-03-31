//
// Created by Александр Кучеров on 07/03/2018.
//

#include <iostream>
#include <ctime>

using namespace std;

static string getFileContents(const char *filename)
{
    LOG_INFO << "Reading file - START";
    FILE *fp = fopen(filename, "rb");
    if (fp)
    {
        string contents;
        fseek(fp, 0, SEEK_END);
        contents.resize(ftell(fp));
        rewind(fp);
        fread(&contents[0], 1, contents.size(), fp);
        fclose(fp);
        LOG_INFO << "Reading file - END";
        return(contents);
    }
    LOG_ERROR << errno;
    throw(errno);
}

static void writeFileContents(const char *filename, const char *content){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    const char* extension = buffer;

    char* newFileName;
    newFileName = new char[strlen(filename)+strlen(extension)+1];
    strcpy(newFileName, filename);
    strcat(newFileName, extension);

    LOG_INFO << "Writing file - START";
    FILE * fp = fopen (newFileName, "wb");;
    if (fp)
    {
        fwrite (content , sizeof(char), sizeof(content), fp);
        fclose (fp);
        LOG_INFO << "Writing file - END";
    }
    LOG_ERROR << errno;
    throw(errno);
}


