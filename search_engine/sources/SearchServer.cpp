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
		auto wordsInQuery = wordSeparate(query);
		
		resultFull.push_back(searchOneQuery(wordsInQuery));
	}

#define MAX_ANSWERS_COUNT 5
	if (resultFull.size() > MAX_ANSWERS_COUNT)
	{
		resultFull.resize(MAX_ANSWERS_COUNT);
	}

	return resultFull;
}

vector<RelativeIndex> SearchServer::searchOneQuery(const vector<string>& words)
{
	vector<RelativeIndex> result;
	
	map<int, int> resultMap;
	map<int, int> buf;

	for (auto& word : words)
	{
		auto count = _index.GetWordCount(word);

		// если результирующий словарь пустой - просто заполняем его
		if (resultMap.empty())
		{
			for (auto& entr : count)
			{
				resultMap[entr.doc_id] = entr.count;
			}
			continue;
		}
		else
		{
			// иначе с помощью буффера фильтруем результаты вхождений
			// убиращий документы, в которых не встречается новое слово
			for (auto& entr : count)
			{
				if (resultMap.find(entr.doc_id) != resultMap.end())
				{
					resultMap[entr.doc_id] += entr.count;
					buf[entr.doc_id] = resultMap[entr.doc_id];
				}
			}
			resultMap.clear();
			resultMap = buf;
			buf.clear();
		}
	}

	// преобразуем словарь в вектор RelativeIndex, одновременно сортируя
	// по количеству вхождений
	while (!resultMap.empty())
	{
		auto maxEntry = findMax(resultMap);

		result.push_back(RelativeIndex(maxEntry.first, maxEntry.second));

		resultMap.erase(maxEntry.first);
	}

	// переводим количество вхождений в релевантность
	int maxCount = result.begin()->rank;
	// если максимальное кол-во вхождений == 0
	// значит возвращаем пустой вектор
	if (maxCount == 0)
	{
		return vector<RelativeIndex>{};
	}
	for (auto& i : result)
	{
		i.rank = i.rank / maxCount;
		i.rank = round(i.rank * 10) / 10;
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

