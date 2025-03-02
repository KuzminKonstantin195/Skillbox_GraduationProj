#include "SearchServer.h"

using namespace std;

vector<string> SearchServer::wordSeparate (const string& line)
{
	vector<string> result;
	stringstream ss (line);
	string word;

	do {
		ss >> word;
		word = reformatingWord(word);
		
		if (result.empty())
		{
			result.push_back(word);
		}


		for (auto i=0; i < result.size() ; i++)
		{
			if (i == result.size() - 1 && result[i] != word)			
			{
				result.push_back(word);						// добавляем только не повторяющиеся слова
			}
		}

	} while (!ss.eof());

	return result;
}

string SearchServer::reformatingWord(string word)
{
	string result;

	for (auto symb : word)
	{
		if ((symb >= '1' && symb <= '9') ||
			(symb >= 'A' && symb <= 'Z') ||
			(symb >= 'a' && symb <= 'z'))
		{
			// приводим всё к нижнему регистру (для удобства)
			if (symb >= 'A' && symb <= 'Z')
			{
				symb = symb + ('a' - 'A');
			}
			result.push_back(symb);
		}
	}
	return result;
}

vector<vector<RelativeIndex>> SearchServer::search (const vector<string>& queries_input)
{
	vector<vector<RelativeIndex>> resultFull;

	for (auto &query : queries_input)
	{
		auto wordsInQuery = wordSeparate(query);			// разделяем запрос на отдельные слова, убирая повторы
		
		wordsInQuery = sortingByEntry(wordsInQuery);		// сортируем список слов по кол-ву документов вхождения

		resultFull.push_back(searchOneQuery(wordsInQuery));
	}
	/*
#define MAX_ANSWERS_COUNT 5
	if (resultFull.size() > MAX_ANSWERS_COUNT)
	{
		resultFull.resize(MAX_ANSWERS_COUNT);
	}
	*/
	return resultFull;
}

vector<RelativeIndex> SearchServer::searchOneQuery(const vector<string>& words)
{
	vector<RelativeIndex> result;
	
	map<int, int> resultMap;		// номер документа, кол-во вхождений
	//map<int, int> buf;				

	for (auto& word : words)
	{
		auto entrys = _index.GetWordCount(word);		// получаем все вхождения по слову
		if (!entrys.empty())
		{
			for (auto& i : entrys)
			{
				resultMap[i.doc_id] += i.count;
			}
		}
	}
	
	// результат - список со всеми вхождениями слов
	// список отсортирован по номерам документов
	// теперь надо отсортировать результаты по кол-ву вхождений

	Entry maxEntr(0, 0);

	// определим, сколько ответов выводить (5 по умолчанию)
	int answersCount;
	maxAnswerCount ? answersCount = maxAnswerCount : answersCount = 5;

	// пока не исчерпаем словарь и/или не получим maxAnswerCount результатов, переносим словарь 
	while (!resultMap.empty() && result.size() < maxAnswerCount)
	{
		for (auto& i : resultMap)
		{
			if (maxEntr.count < i.second)
			{
				maxEntr.doc_id = i.first;
				maxEntr.count = i.second;
			}
			if (maxEntr.count == 0)
			{
				return vector<RelativeIndex>{};
			}
		}

		result.push_back(RelativeIndex(maxEntr.doc_id, maxEntr.count));		// добавляем значение в результат
		resultMap.erase(maxEntr.doc_id);									// извлекаем добавленное значние
		maxEntr = Entry(0, 0);												// обнуляем значения максимума
	}

	// последнее - переводим кол-во вхождений в релевантность
	auto maxCount = result[0].rank;

	for (auto& i : result)
	{
		i.rank /= maxCount;
	}

	return result;
}

pair<int,int> SearchServer::findMax (map<int, int> dict)
{
	pair<int,int> maxEntryPair;

	for (auto& i : dict)
	{
		if (i.second > maxEntryPair.second)
		{
			maxEntryPair.first = i.first;
			maxEntryPair.second = i.second;
		}
	}

	return maxEntryPair;
}

vector<string> SearchServer::sortingByEntry(vector<string> words)
{
	vector<string> result;
	pair <int, int> maxEntryParameters (0,0);			// хранит кол-во вхождений и позицию слова в words

	while (!words.empty())
	{
		for (auto i = 0; i < words.size(); i++)
		{
			string word = words[i];
			int count = _index.GetWordCount(word).size();

			if (count > maxEntryParameters.first)
			{
				maxEntryParameters.first = count;
				maxEntryParameters.second = i;
			}
		}
		result.push_back(words[maxEntryParameters.second]);			// добавляем слово с максимальным кол-вом вхождений в результат
		words.erase(words.begin() + maxEntryParameters.second);		// убираем добавленное слово из начального вектора
		maxEntryParameters = { 0,0 };								// обнуляем максимум
	}
	
	return result;
}
