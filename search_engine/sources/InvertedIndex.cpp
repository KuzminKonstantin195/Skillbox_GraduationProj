#include "InvertedIndex.h"

#define THREADS_MAX_COUNT 4				// задаем максимальное количество потоков

using namespace std;

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs)
{
	// расширяем кол-во буфферных словарей согласно количеству документов
	subDictionariesBuffer.resize(input_docs.size());

	// пулл потоков организуем в виде очереди (FIFO)
	queue<shared_ptr<thread>> threadsPull;

	shared_ptr<thread> updaterThread;
	shared_ptr<thread> joinerThread;
	int threadsRunnedCount = 0;

	updaterThread = make_shared<thread>([&]() {
		for (int i = 0; i < input_docs.size(); i++)
		{

			while (threadsPull.size() >= THREADS_MAX_COUNT) 
			{}

			threadsRunnedCount++;						//ресурс ни с кем не делим - мьютекс ему не нужен
			
			int doublei (i);
			
			threadsPull.push(make_shared<thread>([&]()
				{
					makeIndexFromText(input_docs.at(doublei), doublei);
				}));
			this_thread::sleep_for(chrono::milliseconds(100));
		}
		});

	/*	попытки смыкания потоков будут продолжаться, пока
	*	количество запущенных потоков не будет совпадать с 
	*	количеством документов на разбор
	*	и пока есть потоки в очереди
	*/	

	joinerThread = make_shared<thread>([&]() {
		this_thread::sleep_for(chrono::milliseconds(100));

		int joinedThreads = 0;
		while (joinedThreads < input_docs.size())
		{
			// ставим защиту от слишком быстрого закрытия потоков
			if (threadsPull.size() > 0)
			{
				threadsPull.front()->join();
				threadsPull.pop();
				joinedThreads++;
			}
			
		}
		});

	updaterThread->join();
	joinerThread->join();

	// когда все потоки индексирования завершены, делаем слияние 
	dictionariesMerge(subDictionariesBuffer);

	// зачищаем буфферный словарь
	subDictionariesBuffer.clear();
	subDictionariesBuffer.resize(0);

}

void InvertedIndex::makeIndexFromText(string text, int textNum)
{
	subDictionariesBuffer[textNum] = Indexing(text, textNum);		// заполняем буфферный словарь
}

string InvertedIndex::reformatingWord(string word)
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

map<string, Entry> InvertedIndex::Indexing (string text, int textNum)
{
	// будет создаваться локальный словарь для одного текста,
	// а в дальнейшем, они будут сливаться в один
	map<string, Entry> localDict;
	stringstream ssText(text);
	string word;

	do
	{
		ssText >> word;													// извлекаем из текста слово
		word = reformatingWord(word);

		if (!localDict.count(word))
		{
			localDict[word] = Entry(textNum, 1);
		}
		else
		{
			localDict[word].count += 1;
		}

	} while (!ssText.eof());

	return localDict;
}

vector<Entry> InvertedIndex::GetWordCount(const string& word)
{
	auto result = freq_dictionary.find(word);
	// если не найдено результатов по искомому слову
	// возвращаем "нулевой" результат

	if (result == freq_dictionary.end())
	{
		return vector<Entry>{ Entry(0, 0) };
	} 
	return result->second;
}

void InvertedIndex::dictionariesMerge(vector<map<string, Entry>>& subDicts)
{
	// словари расположены в порядке нумерации документов
	// поэтому просто вливаем их по порядку

	for (auto& doc : subDicts)
	{
		for (auto& dictPair : doc)
		{
			// если слово в словаре есть, то добавялем в конец
			if (freq_dictionary.find(dictPair.first) != freq_dictionary.end())
			{
				freq_dictionary[dictPair.first].push_back(dictPair.second);
			}
			else
			{
				// если слово в словаре пока не встречалось, то создаем для него вектор
				freq_dictionary[dictPair.first] = vector<Entry>{dictPair.second};
			}
		}
	}
}
