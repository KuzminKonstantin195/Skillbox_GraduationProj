#include "ConverterJSON.h"

using namespace std;
using namespace nlohmann;


// считываение содержимого файла
string getTextFromFile(string fileName)
{
	
	ifstream file;
	string text, line;

	try
	{
		file.open(fileName);
	}
	catch (const std::exception&)
	{
		cerr << "" << endl;
		return NULL;
	}
	if (file.is_open())
	{
		while (getline(file, line))
		{
			text.append(line);
		}

		file.close();
	}
	return text;
}

vector<string> ConverterJSON::GetTextDocuments()
{
	setlocale(LC_ALL, "Russiаn");
	vector<string> result;

	ifstream configFile;
	json configJson;

	try
	{
		configFile.open("..//config.json");
	}
	catch (const std::exception&)
	{
		cerr << "Ошибка открытия файл config.json!";
		return vector<string>(NULL);
	}

	if (!configFile.is_open())
	{
		return vector<string>(NULL);
	}
	
	configJson = json::parse(configFile);

	json filesList = configJson["files"];

	for (auto &name : filesList)
	{
		result.push_back(getTextFromFile(name));
	}
	configFile.close();

	return result;
	
}

int ConverterJSON::GetResponsesLimit()
{
	ifstream configFile;
	json configJson;

	try
	{
		configFile.open("..//config.json");
	}
	catch (const std::exception&)
	{
		cerr << "Ошибка открытия файл config.json!";
		return NULL;
	}

	if (configFile.is_open())
	{
		configJson = json::parse(configFile);
		configFile.close();

		json config = configJson["config"];
		return 	config["max_responses"];
	}
	else
	{
		return NULL;
	}
}

vector<string> ConverterJSON::GetRequests()
{
	vector<string> toReturn;

	ifstream requestsFile;
	json requestsJson;

	try
	{
		requestsFile.open("..//requests.json");
	}
	catch (const std::exception&)
	{
		cerr << "Ошибка открытия файл requests.json!";
		return  vector<string>(NULL);
	}

	if (!requestsFile.is_open())
	{
		return  vector<string>(NULL);
	}

	requestsJson = json::parse(requestsFile);

	for (auto& words : requestsJson["requests"])
	{
		toReturn.push_back(words);
	}

	return toReturn;
}

string createRequestName(int num)
{
	if (num < 10)
	{
		return "request00" + to_string(num);
	}
	else if (num < 100)
	{
		return "request0" + to_string(num);
	}
	else
	{
		return "request" + to_string(num);
	}
}

string createRelevance(vector<pair<int, float>> &answers)
{
	string result;
	char buffer [100];
	string fl_str;

	for (auto i : answers)
	{
		fl_str = to_string( round(i.second * 1000) / 1000);

		sprintf(buffer, "\n\t\"docid\": %d, \"rank\": %s,", i.first, fl_str);

		result.append(buffer);
	}

	result.pop_back(); // убираем последнюю запятую
	return result;
}

void ConverterJSON::putAnswers(vector<vector<pair<int, float>>>
	answers)
{

	
	json answerJson;

	string requestName;

	unsigned short reqNum (1);
	for (auto request : answers)
	{
		json requestResultJson;
		
		switch (request.size())
		{
		case 0:
			requestResultJson["result"] = "false";
			break;
		case 1:
			requestResultJson["result"] = "true";
			requestResultJson["docid"] = request[0].first;
			requestResultJson["rank"] = round(request[0].second * 1000) / 1000;
			break;
		default:
			requestResultJson["result"] = "true";
			requestResultJson["relevance"] = createRelevance(request);
			break;
		}

		requestName = createRequestName(reqNum);
		answerJson[requestName] = requestResultJson;
		reqNum++;
		requestResultJson.clear();
	}
	ofstream answersFile("..//answers.json");
	answersFile << answerJson;
	answersFile.close();
}
