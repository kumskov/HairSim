#include <fstream>
#include <string>
#include <vector>
#include <iostream>

char** loadSource(const char* flname)
{
	std::ifstream ifs(flname, std::ifstream::in);
	std::string line;
	std::vector<std::string> lines;

	while (std::getline(ifs, line))
	{
		lines.push_back(line);
	}

	char** ret = (char**) malloc((lines.size()+1)*sizeof(char*));
	for(int i = 0; i < lines.size(); ++i)
	{
		ret[i] = (char*) malloc(lines[i].length()*sizeof(char));
		lines[i]+='\n';
		ret[i] = lines[i].c_str();
		std::cout << ret[i];
	}
	ret[lines.size()] = (char*) malloc(sizeof(char));
	ret[lines.size()][0] = '\0';

	return ret;
}

char** loadSource(std::string flname)
{
	return loadSource(flname.c_str());
}